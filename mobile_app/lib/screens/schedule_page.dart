import 'package:flutter/material.dart';
import '../services/api_service.dart';
import '../models/schedule.dart';
import '../models/medication.dart';
import '../services/storage_service.dart';

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

    Medication selectedMedication =
        medications.first;

    List<String> selectedTimes = [];

    List<String> selectedDays = [];

    const days = [
      "MON",
      "TUE",
      "WED",
      "THU",
      "FRI",
      "SAT",
      "SUN",
    ];

    await showDialog(
      context: context,

      builder: (context) {

        return StatefulBuilder(

          builder:
              (context, setDialogState) {

            return AlertDialog(

              title: const Text(
                "New Schedule",
              ),

              content:
              SingleChildScrollView(
                child: Column(
                  mainAxisSize:
                  MainAxisSize.min,

                  children: [

                    DropdownButton<Medication>(
                      value:
                      selectedMedication,

                      isExpanded: true,

                      items:
                      medications
                          .map(
                            (med) =>
                            DropdownMenuItem<
                                Medication>(
                              value: med,

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

                    Wrap(
                      spacing: 8,

                      children:
                      selectedTimes
                          .map(
                            (time) =>
                            Chip(
                              label: Text(
                                time,
                              ),
                            ),
                      )
                          .toList(),
                    ),

                    ElevatedButton(
                      onPressed: () async {

                        final picked =
                        await showTimePicker(
                          context:
                          context,

                          initialTime:
                          const TimeOfDay(
                            hour: 8,
                            minute: 0,
                          ),
                        );

                        if (picked !=
                            null) {

                          String
                          formattedTime =
                              "${picked.hour.toString().padLeft(2, '0')}:${picked.minute.toString().padLeft(2, '0')}";

                          setDialogState(
                                () {
                              selectedTimes
                                  .add(
                                formattedTime,
                              );
                            },
                          );
                        }
                      },

                      child:
                      const Text(
                        "Add Time",
                      ),
                    ),

                    const SizedBox(
                      height: 20,
                    ),

                    Wrap(
                      spacing: 6,

                      children:
                      days.map((day) {

                        bool selected =
                        selectedDays
                            .contains(
                          day,
                        );

                        return FilterChip(
                          label:
                          Text(day),

                          selected:
                          selected,

                          onSelected:
                              (value) {

                            setDialogState(
                                  () {

                                if (value) {

                                  selectedDays
                                      .add(
                                    day,
                                  );

                                } else {

                                  selectedDays
                                      .remove(
                                    day,
                                  );
                                }
                              },
                            );
                          },
                        );
                      }).toList(),
                    ),
                  ],
                ),
              ),

              actions: [

                TextButton(
                  onPressed: () {
                    Navigator.pop(
                      context,
                    );
                  },

                  child: const Text(
                    "Cancel",
                  ),
                ),

                ElevatedButton(
                  onPressed: () async {

                    if (selectedTimes.isEmpty) {

                      ScaffoldMessenger
                          .of(context)
                          .showSnackBar(
                        const SnackBar(
                          content: Text(
                            "Add at least one time",
                          ),
                        ),
                      );

                      return;
                    }

                    if (selectedDays.isEmpty) {

                      ScaffoldMessenger
                          .of(context)
                          .showSnackBar(
                        const SnackBar(
                          content: Text(
                            "Select at least one day",
                          ),
                        ),
                      );

                      return;
                    }

                    final newSchedule =
                    Schedule(
                      drawerId:
                      selectedMedication.drawerId,

                      times:
                      selectedTimes,

                      daysOfWeek:
                      selectedDays,

                      enabled:
                      true,
                    );

                    // Guardar localmente primero

                    final createdSchedule =
                    await ApiService.createSchedule(
                      newSchedule,
                    );

                    if (createdSchedule == null) {

                      if (mounted) {

                        ScaffoldMessenger.of(context)
                            .showSnackBar(

                          const SnackBar(
                            content: Text(
                              "Failed to create schedule on ESP32",
                            ),
                          ),
                        );
                      }

                      return;
                    }

                    schedules.add(
                      createdSchedule,
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

    final schedule =
    schedules[index];

    if (schedule.id == null) {

      ScaffoldMessenger.of(context)
          .showSnackBar(

        const SnackBar(
          content: Text(
            "Schedule ID not found",
          ),
        ),
      );

      return;
    }

    final success =
    await ApiService.deleteSchedule(
      schedule.id!,
    );

    if (!success) {

      if (mounted) {

        ScaffoldMessenger.of(context)
            .showSnackBar(

          const SnackBar(
            content: Text(
              "Could not delete schedule on ESP32",
            ),
          ),
        );
      }

      return;
    }

    schedules.removeAt(
      index,
    );

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
                "Drawer ${schedule.drawerId}",
              ),

              subtitle: Text(
                "Times: ${schedule.times.join(', ')}\n"
                    "Days: ${schedule.daysOfWeek.join(', ')}",
              ),

              trailing:
              IconButton(

                icon:
                const Icon(
                  Icons.delete,
                ),

                onPressed: () {
                  deleteSchedule(
                    index,
                  );
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