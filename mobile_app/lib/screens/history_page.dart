import 'package:flutter/material.dart';

import '../models/dose_record.dart';
import '../services/storage_service.dart';
import '../models/medication.dart';

class HistoryPage extends StatefulWidget {
  const HistoryPage({super.key});

  @override
  State<HistoryPage> createState() =>
      _HistoryPageState();
}

class _HistoryPageState
    extends State<HistoryPage> {

  List<DoseRecord> records = [];
  List<Medication> medications = [];

  @override
  void initState() {
    super.initState();
    loadData();
  }

  Future<void> loadData() async {

    final loadedRecords =
    await StorageService.loadHistory();

    final loadedMedications =
    await StorageService.loadMedications();

    setState(() {
      records = loadedRecords;
      medications = loadedMedications;
    });
  }

  Future<void> addRecord() async {

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

    String selectedStatus =
        "Taken";

    await showDialog(
      context: context,

      builder: (context) {

        return StatefulBuilder(

          builder: (
              context,
              setDialogState,
              ) {

            return AlertDialog(

              title: const Text(
                "New Dose Record",
              ),

              content: Column(
                mainAxisSize:
                MainAxisSize.min,

                children: [

                  DropdownButton<String>(
                    value:
                    selectedMedication,

                    isExpanded: true,

                    items: medications
                        .map(
                          (med) =>
                          DropdownMenuItem(
                            value:
                            med.name,

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

                  DropdownButton<String>(
                    value:
                    selectedStatus,

                    isExpanded: true,

                    items: const [

                      DropdownMenuItem(
                        value: "Taken",
                        child:
                        Text("Taken"),
                      ),

                      DropdownMenuItem(
                        value: "Missed",
                        child:
                        Text("Missed"),
                      ),
                    ],

                    onChanged: (value) {

                      setDialogState(() {
                        selectedStatus =
                        value!;
                      });
                    },
                  ),
                ],
              ),

              actions: [

                TextButton(
                  onPressed: () {
                    Navigator.pop(
                      context,
                    );
                  },

                  child:
                  const Text("Cancel"),
                ),

                ElevatedButton(
                  onPressed: () async {

                    records.add(
                      DoseRecord(
                        medicationName:
                        selectedMedication,

                        status:
                        selectedStatus,

                        timestamp:
                        DateTime.now()
                            .toString(),
                      ),
                    );

                    await StorageService
                        .saveHistory(
                      records,
                    );

                    setState(() {});

                    if (mounted) {
                      Navigator.pop(
                        context,
                      );
                    }
                  },

                  child:
                  const Text("Save"),
                ),
              ],
            );
          },
        );
      },
    );
  }

  Icon getStatusIcon(
      String status) {

    if (status == "Taken") {
      return const Icon(
        Icons.check_circle,
        color: Colors.green,
      );
    }

    return const Icon(
      Icons.warning,
      color: Colors.orange,
    );
  }

  @override
  Widget build(BuildContext context) {

    return Scaffold(
      appBar: AppBar(
        title:
        const Text("Dose History"),
      ),

      body: records.isEmpty

          ? const Center(
        child: Text(
          "No records available",
        ),
      )

          : ListView.builder(
        itemCount:
        records.length,

        itemBuilder:
            (context, index) {

          final record =
          records[index];

          return Card(
            child: ListTile(

              leading:
              getStatusIcon(
                record.status,
              ),

              title: Text(
                record
                    .medicationName,
              ),

              subtitle: Text(
                "${record.status}\n${record.timestamp}",
              ),
            ),
          );
        },
      ),

      floatingActionButton:
      FloatingActionButton(
        onPressed: addRecord,
        child:
        const Icon(Icons.add),
      ),
    );
  }
}