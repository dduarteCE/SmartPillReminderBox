import 'package:flutter/material.dart';

import '../models/medication.dart';
import '../services/storage_service.dart';

class MedicationPage extends StatefulWidget {
  const MedicationPage({super.key});

  @override
  State<MedicationPage> createState() => _MedicationPageState();
}

class _MedicationPageState extends State<MedicationPage> {

  List<Medication> medications = [];

  @override
  void initState() {
    super.initState();
    loadData();
  }

  Future<void> loadData() async {

    final data =
    await StorageService.loadMedications();

    setState(() {
      medications = data;
    });
  }

  Future<void> addMedication() async {

    final nameController =
    TextEditingController();

    final dosageController =
    TextEditingController();

    int selectedDrawer = 1;


    await showDialog(
      context: context,

      builder: (context) {

        return AlertDialog(
          title: const Text("New Medication"),

          content: SingleChildScrollView(
            child: Column(
              mainAxisSize: MainAxisSize.min,

              children: [

                TextField(
                  controller: nameController,
                  decoration: const InputDecoration(
                    labelText: "Medication Name",
                  ),
                ),

                TextField(
                  controller: dosageController,
                  decoration: const InputDecoration(
                    labelText: "Dosage",
                  ),
                ),

                StatefulBuilder(
                  builder: (
                      context,
                      setDialogState,
                      ) {

                    return DropdownButtonFormField<int>(
                      value: selectedDrawer,

                      decoration:
                      const InputDecoration(
                        labelText: "Drawer",
                      ),

                      items: List.generate(
                        7,
                            (index) =>
                            DropdownMenuItem(
                              value: index + 1,
                              child: Text(
                                "Drawer ${index + 1}",
                              ),
                            ),
                      ),

                      onChanged: (value) {

                        setDialogState(() {
                          selectedDrawer = value!;
                        });
                      },
                    );
                  },
                ),
              ],
            ),
          ),

          actions: [

            TextButton(
              onPressed: () {
                Navigator.pop(context);
              },
              child: const Text("Cancel"),
            ),

            ElevatedButton(
              onPressed: () async {

                if (nameController.text.isEmpty) {
                  return;
                }

                medications.add(
                  Medication(
                    name: nameController.text,
                    dosage: dosageController.text,
                    drawerId: selectedDrawer,
                  ),
                );

                await StorageService
                    .saveMedications(
                    medications);

                setState(() {});

                if (mounted) {
                  Navigator.pop(context);
                }
              },

              child: const Text("Save"),
            ),
          ],
        );
      },
    );
  }

  Future<void> deleteMedication(int index)
  async {

    medications.removeAt(index);

    await StorageService.saveMedications(
      medications,
    );

    setState(() {});
  }

  @override
  Widget build(BuildContext context) {

    return Scaffold(
      appBar: AppBar(
        title: const Text("Medications"),
      ),

      body: medications.isEmpty

          ? const Center(
        child: Text(
          "No medications registered",
        ),
      )

          : ListView.builder(
        itemCount: medications.length,

        itemBuilder: (context, index) {

          final med =
          medications[index];

          return Card(
            child: ListTile(

              leading:
              const Icon(Icons.medication),

              title: Text(
                med.name,
              ),

              subtitle: Text(
                "${med.dosage} | Drawer ${med.drawerId}",
              ),

              trailing: IconButton(
                icon: const Icon(
                  Icons.delete,
                ),

                onPressed: () {
                  deleteMedication(
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
        onPressed: addMedication,
        child: const Icon(Icons.add),
      ),
    );
  }
}
