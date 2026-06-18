import 'package:flutter/material.dart';

import '../models/medication.dart';
import '../models/schedule.dart';
import '../services/storage_service.dart';
import '../services/api_service.dart';

class DashboardPage extends StatefulWidget {
  const DashboardPage({super.key});

  @override
  State<DashboardPage> createState() => _DashboardPageState();
}

class _DashboardPageState extends State<DashboardPage> {

  List<Medication> medications = [];
  List<Schedule> schedules = [];

  String esp32Status = "Disconnected";

  @override
  void initState() {
    super.initState();
    loadData();
  }

  Future<void> loadData() async {

    final loadedMedications =
    await StorageService.loadMedications();

    final loadedSchedules =
    await StorageService.loadSchedules();

    setState(() {

      medications = loadedMedications;
      schedules = loadedSchedules;
    });

    final health =
    await ApiService.getDeviceHealth();

    if (!mounted) return;

    setState(() {

      if (health != null &&
          health["success"] == true) {

        esp32Status = "Connected";

      } else {

        esp32Status = "Disconnected";
      }
    });
  }

  Future<void> syncSchedules() async {

    if (schedules.isEmpty) {

      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
          content: Text(
            "No schedules to sync",
          ),
        ),
      );

      return;
    }

    bool success = true;

    for (final schedule in schedules) {

      final result =
      await ApiService.createSchedule(
        schedule,
      );

      if (!result) {
        success = false;
      }
    }

    if (!mounted) return;

    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text(
          success
              ? "Schedules synchronized"
              : "Some schedules failed",
        ),
      ),
    );
  }

  String getNextDose() {

    if (schedules.isEmpty) {
      return "No schedules";
    }

    List<String> allTimes = [];

    for (var schedule in schedules) {
      allTimes.addAll(
        schedule.times,
      );
    }

    if (allTimes.isEmpty) {
      return "No schedules";
    }

    allTimes.sort();

    return allTimes.first;
  }

  @override
  Widget build(BuildContext context) {

    return Scaffold(

      appBar: AppBar(
        title: const Text(
          "Smart Pill Reminder",
        ),

        actions: [

          IconButton(
            icon: const Icon(
              Icons.sync,
            ),

            onPressed: syncSchedules,
          ),
        ],
      ),

      body: RefreshIndicator(
        onRefresh: loadData,

        child: ListView(
          padding: const EdgeInsets.all(16),

          children: [

            const Text(
              "Patient Status",
              style: TextStyle(
                fontSize: 24,
                fontWeight: FontWeight.bold,
              ),
            ),

            const SizedBox(
              height: 20,
            ),

            Card(
              child: ListTile(
                leading:
                const Icon(Icons.medication),

                title: const Text(
                  "Total Medications",
                ),

                subtitle: Text(
                  "${medications.length}",
                ),
              ),
            ),

            Card(
              child: ListTile(
                leading:
                const Icon(Icons.schedule),

                title: const Text(
                  "Total Schedules",
                ),

                subtitle: Text(
                  "${schedules.length}",
                ),
              ),
            ),

            Card(
              child: ListTile(
                leading:
                const Icon(Icons.access_time),

                title: const Text(
                  "Next Dose",
                ),

                subtitle: Text(
                  getNextDose(),
                ),
              ),
            ),

            Card(
              child: ListTile(
                leading: Icon(
                  esp32Status == "Connected"
                      ? Icons.wifi
                      : Icons.wifi_off,
                ),

                title: const Text(
                  "ESP32 Status",
                ),

                subtitle: Text(
                  esp32Status,
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}