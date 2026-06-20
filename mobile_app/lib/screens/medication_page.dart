import 'package:flutter/material.dart';
import '../services/api_service.dart';
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

    final pillCountController =
    TextEditingController();

    final availableDrawers =
    List.generate(
      7,
          (index) => index + 1,
    )
        .where(
          (drawer) =>
      !medications.any(
            (med) =>
        med.drawerId ==
            drawer,
      ),
    )
        .toList();

    if (availableDrawers.isEmpty) {

      ScaffoldMessenger.of(context)
          .showSnackBar(

        const SnackBar(
          content: Text(
            "All drawers are occupied",
          ),
        ),
      );

      return;
    }

    int selectedDrawer =
        availableDrawers.first;

    await showDialog(
      context: context,

      builder: (context) {

        return AlertDialog(
          title: const Text(
            "New Medication",
          ),

          content: SingleChildScrollView(
            child: Column(
              mainAxisSize: MainAxisSize.min,

              children: [

                TextField(
                  controller:
                  nameController,

                  decoration:
                  const InputDecoration(
                    labelText:
                    "Medication Name",
                  ),
                ),

                TextField(
                  controller:
                  pillCountController,

                  keyboardType:
                  TextInputType.number,

                  decoration:
                  const InputDecoration(
                    labelText:
                    "Pill Count (1-30)",
                  ),
                ),

                StatefulBuilder(
                  builder: (
                      context,
                      setDialogState,
                      ) {

                    return DropdownButtonFormField<int>(
                      value:
                      selectedDrawer,

                      decoration:
                      const InputDecoration(
                        labelText:
                        "Drawer",
                      ),

                      items: List.generate(
                        7,
                            (index) => index + 1,
                      )
                          .where(
                            (drawer) =>
                        !medications.any(
                              (med) =>
                          med.drawerId ==
                              drawer,
                        ),
                      )
                          .map(
                            (drawer) =>
                            DropdownMenuItem(
                              value: drawer,
                              child: Text(
                                "Drawer $drawer",
                              ),
                            ),
                      )
                          .toList(),

                      onChanged: (value) {

                        setDialogState(() {
                          selectedDrawer =
                          value!;
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
                Navigator.pop(
                  context,
                );
              },

              child:
              const Text(
                "Cancel",
              ),
            ),

            ElevatedButton(
              onPressed: () async {

                if (nameController
                    .text
                    .isEmpty) {
                  return;
                }

                final pillCount =
                    int.tryParse(
                      pillCountController.text,
                    ) ?? 0;

                if (pillCount < 1 ||
                    pillCount > 30) {

                  ScaffoldMessenger
                      .of(context)
                      .showSnackBar(

                    const SnackBar(
                      content: Text(
                        "Pill count must be between 1 and 30",
                      ),
                    ),
                  );

                  return;
                }

                final newMedication =
                Medication(
                  name:
                  nameController.text,

                  drawerId:
                  selectedDrawer,

                  pillCount:
                  pillCount,
                );

                final drawerInUse =
                medications.any(
                      (med) =>
                  med.drawerId ==
                      selectedDrawer,
                );

                if (drawerInUse) {

                  ScaffoldMessenger.of(context)
                      .showSnackBar(

                    SnackBar(
                      content: Text(
                        "Drawer $selectedDrawer is already in use",
                      ),
                    ),
                  );

                  return;
                }

                medications.add(
                  newMedication,
                );

                await StorageService
                    .saveMedications(
                  medications,
                );

                final sentToEsp32 =
                await ApiService.updateDrawer(
                  newMedication,
                );

                if (!sentToEsp32) {

                  if (mounted) {

                    ScaffoldMessenger
                        .of(context)
                        .showSnackBar(

                      const SnackBar(
                        content: Text(
                          "Could not sync drawer with ESP32. Saved locally only.",
                        ),
                      ),
                    );
                  }
                }

                setState(() {});

                if (mounted) {
                  Navigator.pop(
                    context,
                  );
                }
              },

              child:
              const Text(
                "Save",
              ),
            ),
          ],
        );
      },
    );
  }

  Future<void> deleteMedication(
      int index) async {

    final medication =
    medications[index];

    final drawerId =
        medication.drawerId;

    final success =
    await ApiService.deleteDrawer(
      drawerId,
    );

    if (!success) {

      if (mounted) {

        ScaffoldMessenger.of(context)
            .showSnackBar(

          const SnackBar(
            content: Text(
              "Could not delete drawer on ESP32",
            ),
          ),
        );
      }

      return;
    }

    // Eliminar medicamento local
    medications.removeAt(
      index,
    );

    await StorageService
        .saveMedications(
      medications,
    );

    // Eliminar horarios asociados
    final schedules =
    await StorageService
        .loadSchedules();

    schedules.removeWhere(
          (schedule) =>
      schedule.drawerId ==
          drawerId,
    );

    await StorageService
        .saveSchedules(
      schedules,
    );

    if (mounted) {

      ScaffoldMessenger.of(context)
          .showSnackBar(

        const SnackBar(
          content: Text(
            "Drawer and schedules deleted",
          ),
        ),
      );
    }

    setState(() {});
  }

  @override
  Widget build(BuildContext context) {

    return Scaffold(

      appBar: AppBar(
        title: const Text(
          "Medications",
        ),
      ),

      body: medications.isEmpty

          ? const Center(
        child: Text(
          "No medications registered",
        ),
      )

          : ListView.builder(
        itemCount:
        medications.length,

        itemBuilder:
            (context, index) {

          final med =
          medications[index];

          return Card(
            child: ListTile(

              leading:
              const Icon(
                Icons.medication,
              ),

              title: Text(
                med.name,
              ),

              subtitle: Text(
                    "Drawer ${med.drawerId}\n"
                    "Pills: ${med.pillCount}",
              ),

              trailing:
              IconButton(

                icon:
                const Icon(
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
        onPressed:
        addMedication,

        child:
        const Icon(
          Icons.add,
        ),
      ),
    );
  }
}