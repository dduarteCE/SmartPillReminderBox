import 'dart:async';

import 'package:flutter/material.dart';
import '../models/dose_record.dart';
import '../services/storage_service.dart';

class HistoryPage extends StatefulWidget {
  const HistoryPage({super.key});

  @override
  State<HistoryPage> createState() =>
      _HistoryPageState();
}

class _HistoryPageState
    extends State<HistoryPage> {

  List<DoseRecord> records = [];
  StreamSubscription<void>? historySubscription;

  @override
  void initState() {
    super.initState();
    historySubscription =
        StorageService.historyChanges.listen((_) => loadData());
    loadData();
  }

  @override
  void dispose() {
    historySubscription?.cancel();
    super.dispose();
  }

  Future<void> loadData() async {

    final loadedRecords =
    await StorageService.loadHistory();

    if (!mounted) {
      return;
    }

    setState(() {
      records = loadedRecords;
    });
  }

  Future<void> clearHistory() async {

    final confirm =
    await showDialog<bool>(
      context: context,

      builder: (context) {

        return AlertDialog(

          title: const Text(
            "Clear History",
          ),

          content: const Text(
            "Delete all dose records?",
          ),

          actions: [

            TextButton(
              onPressed: () {

                Navigator.pop(
                  context,
                  false,
                );
              },

              child: const Text(
                "Cancel",
              ),
            ),

            ElevatedButton(
              onPressed: () {

                Navigator.pop(
                  context,
                  true,
                );
              },

              child: const Text(
                "Delete",
              ),
            ),
          ],
        );
      },
    );

    if (confirm != true) {
      return;
    }

    records.clear();

    await StorageService.saveHistory(
      records,
    );

    setState(() {});
  }

  Icon getStatusIcon(
      String status) {

    if (status == "Taken") {
      return const Icon(
        Icons.check_circle,
        color: Colors.green,
      );
    }

    if (status == "Empty") {
      return const Icon(
        Icons.inventory_2,
        color: Colors.red,
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
        title: const Text("Dose History"),

        actions: [

          IconButton(
            icon: const Icon(
              Icons.delete_sweep,
            ),

            onPressed: clearHistory,
          ),
        ],
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


    );
  }
}
