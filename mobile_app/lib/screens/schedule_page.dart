import 'package:flutter/material.dart';
import '../models/schedule.dart';
import '../services/storage_service.dart';
import '../models/medication.dart';

class SchedulePage extends StatefulWidget {
  const SchedulePage({super.key});

  @override
  State<SchedulePage> createState() =>
      _SchedulePageState();
}

class _SchedulePageState
    extends State<SchedulePage> {

  List<Schedule> schedules = [];
  List<Medication> medications = [];

  @override
  void initState() {
    super.initState();
    loadData();
  }

  Future<void> loadData() async {

    final loadedSchedules =
    await StorageService.loadSchedules();

    final loadedMedications =
    await StorageService.loadMedications();

    setState(() {

      schedules = loadedSchedules;

      medications = loadedMedications;
    });
  }
  Future<void> addSchedule() async {

    if (medications.isEmpty) {

      ScaffoldMessenger.of(context)
          .showSnackBar(
        const SnackBar(
          content: Text(
            "Add medications first",
          ),
        ),
      );

      return;
    }

    String selectedMedication =
        medications.first.name;

    TimeOfDay selectedTime =
    const TimeOfDay(
      hour: 8,
      minute: 0,
    );

    await showDialog(
      context: context,

      builder: (context) {

        return StatefulBuilder(

          builder: (context, setDialogState) {

            return AlertDialog(

              title: const Text(
                "New Schedule",
              ),

              content: Column(
                mainAxisSize: MainAxisSize.min,

                children: [

                  DropdownButton<String>(
                    value: selectedMedication,

                    isExpanded: true,

                    items: medications
                        .map(
                          (med) =>
                          DropdownMenuItem(
                            value: med.name,

                            child: Text(
                              med.name,
                            ),
                          ),
                    )
                        .toList(),

                    onChanged: (value) {

                      setDialogState(() {
                        selectedMedication =
                        value!;
                      });
                    },
                  ),

                  const SizedBox(
                    height: 20,
                  ),

                  Text(
                    selectedTime.format(
                      context,
                    ),
                  ),

                  ElevatedButton(
                    onPressed: () async {

                      final picked =
                      await showTimePicker(
                        context: context,
                        initialTime:
                        selectedTime,
                      );

                      if (picked != null) {

                        setDialogState(() {
                          selectedTime =
                              picked;
                        });
                      }
                    },

                    child: const Text(
                      "Select Time",
                    ),
                  ),
                ],
              ),

              actions: [

                TextButton(
                  onPressed: () {
                    Navigator.pop(context);
                  },

                  child: const Text(
                    "Cancel",
                  ),
                ),

                ElevatedButton(
                  onPressed: () async {

                    schedules.add(
                      Schedule(
                        medicationName:
                        selectedMedication,

                        time:
                        "${selectedTime.hour.toString().padLeft(2, '0')}:${selectedTime.minute.toString().padLeft(2, '0')}",
                      ),
                    );

                    await StorageService
                        .saveSchedules(
                      schedules,
                    );

                    setState(() {});

                    if (mounted) {
                      Navigator.pop(
                        context,
                      );
                    }
                  },

                  child: const Text(
                    "Save",
                  ),
                ),
              ],
            );
          },
        );
      },
    );
  }

  Future<void> deleteSchedule(
      int index) async {

    schedules.removeAt(index);

    await StorageService.saveSchedules(
      schedules,
    );

    setState(() {});
  }

  @override
  Widget build(BuildContext context) {

    return Scaffold(
      appBar: AppBar(
        title:
        const Text("Schedules"),
      ),

      body: schedules.isEmpty

          ? const Center(
        child: Text(
          "No schedules configured",
        ),
      )

          : ListView.builder(
        itemCount:
        schedules.length,

        itemBuilder:
            (context, index) {

          final schedule =
          schedules[index];

          return Card(
            child: ListTile(
              leading:
              const Icon(
                Icons.schedule,
              ),

              title: Text(
                schedule
                    .medicationName,
              ),

              subtitle: Text(
                schedule.time,
              ),

              trailing:
              IconButton(
                icon:
                const Icon(
                  Icons.delete,
                ),

                onPressed: () {
                  deleteSchedule(
                      index);
                },
              ),
            ),
          );
        },
      ),

      floatingActionButton:
      FloatingActionButton(
        onPressed: addSchedule,

        child:
        const Icon(Icons.add),
      ),
    );
  }
}