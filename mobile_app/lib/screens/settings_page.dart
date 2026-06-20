import 'package:flutter/material.dart';
import '../services/api_service.dart';
import '../services/connection_service.dart';
import '../services/storage_service.dart';
import '../services/websocket_service.dart';

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

  final wsPortController =
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

    final wsPort =
    await StorageService.loadEsp32WsPort();

    ipController.text = ip;
    portController.text = port;
    wsPortController.text = wsPort;
  }

  Future<void> saveSettings() async {

    await StorageService.saveEsp32Ip(
      ipController.text,
    );

    await StorageService.saveEsp32Port(
      portController.text,
    );

    await StorageService.saveEsp32WsPort(
      wsPortController.text,
    );

    await WebSocketService.disconnect();

    ConnectionService
        .esp32Connected = false;

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

  Future<void> testConnection() async {

    final health =
    await ApiService.getDeviceHealth();

    if (!mounted) return;

    if (health != null &&
        health["success"] == true) {

      ConnectionService
          .esp32Connected = true;

      await ApiService
          .setDeviceTime();

      await WebSocketService.connect();

      if (!mounted) return;

      ScaffoldMessenger.of(context)
          .showSnackBar(

        const SnackBar(
          content: Text(
            "ESP32 Connected and Time Synced",
          ),
        ),
      );
    } else {

      ConnectionService
          .esp32Connected = false;

      ScaffoldMessenger.of(context)
          .showSnackBar(

        const SnackBar(
          content: Text(
            "ESP32 Not Reachable",
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
              controller:
              ipController,

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
                labelText:
                "HTTP Port",
                border:
                OutlineInputBorder(),
              ),
            ),

            const SizedBox(
              height: 20,
            ),

            TextField(
              controller:
              wsPortController,

              keyboardType:
              TextInputType.number,

              decoration:
              const InputDecoration(
                labelText:
                "WebSocket Port",
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

            const SizedBox(
              height: 12,
            ),

            SizedBox(
              width: double.infinity,

              child: ElevatedButton(
                onPressed:
                testConnection,

                child: const Text(
                  "Test Connection",
                ),
              ),
            ),

            const SizedBox(
              height: 12,
            ),
          ],
        ),
      ),
    );
  }
}
