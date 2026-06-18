import 'package:flutter/material.dart';

import '../services/storage_service.dart';

class SettingsPage extends StatefulWidget {
  const SettingsPage({super.key});

  @override
  State<SettingsPage> createState() =>
      _SettingsPageState();
}

class _SettingsPageState
    extends State<SettingsPage> {

  final ipController =
  TextEditingController();

  final portController =
  TextEditingController();

  @override
  void initState() {
    super.initState();
    loadSettings();
  }

  Future<void> loadSettings() async {

    final ip =
    await StorageService.loadEsp32Ip();

    final port =
    await StorageService.loadEsp32Port();

    ipController.text = ip;
    portController.text = port;
  }

  Future<void> saveSettings() async {

    await StorageService.saveEsp32Ip(
      ipController.text,
    );

    await StorageService.saveEsp32Port(
      portController.text,
    );

    if (mounted) {

      ScaffoldMessenger.of(context)
          .showSnackBar(

        const SnackBar(
          content: Text(
            "Settings saved",
          ),
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {

    return Scaffold(

      appBar: AppBar(
        title:
        const Text("Settings"),
      ),

      body: Padding(

        padding:
        const EdgeInsets.all(16),

        child: Column(

          children: [

            TextField(
              controller: ipController,

              decoration:
              const InputDecoration(
                labelText: "ESP32 IP",
                border:
                OutlineInputBorder(),
              ),
            ),

            const SizedBox(
              height: 20,
            ),

            TextField(
              controller:
              portController,

              keyboardType:
              TextInputType.number,

              decoration:
              const InputDecoration(
                labelText: "Port",
                border:
                OutlineInputBorder(),
              ),
            ),

            const SizedBox(
              height: 20,
            ),

            SizedBox(
              width: double.infinity,

              child: ElevatedButton(
                onPressed:
                saveSettings,

                child: const Text(
                  "Save",
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}