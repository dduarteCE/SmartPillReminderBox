import 'package:flutter/material.dart';

import '../models/medication.dart';
import '../models/schedule.dart';
import '../services/storage_service.dart';

class DashboardPage extends StatefulWidget {
  const DashboardPage({super.key});

  @override
  State<DashboardPage> createState() => _DashboardPageState();
}

class _DashboardPageState extends State<DashboardPage> {

  List<Medication> medications = [];
  List<Schedule> schedules = [];

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
        title: const Text("Smart Pill Reminder"),
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

            const SizedBox(height: 20),

            Card(
              child: ListTile(
                leading: const Icon(Icons.medication),
                title: const Text("Total Medications"),
                subtitle:
                Text("${medications.length}"),
              ),
            ),

            Card(
              child: ListTile(
                leading: const Icon(Icons.schedule),
                title: const Text("Total Schedules"),
                subtitle:
                Text("${schedules.length}"),
              ),
            ),

            Card(
              child: ListTile(
                leading:
                const Icon(Icons.access_time),
                title: const Text("Next Dose"),
                subtitle: Text(
                  getNextDose(),
                ),
              ),
            ),

            const Card(
              child: ListTile(
                leading: Icon(Icons.wifi),
                title: Text("ESP32 Status"),
                subtitle: Text("Disconnected"),
              ),
            ),
          ],
        ),
      ),
    );
  }
}