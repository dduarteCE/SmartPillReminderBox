import 'dart:async';

import 'package:flutter/material.dart';
import 'screens/medication_page.dart';
import 'screens/schedule_page.dart';
import 'screens/dashboard_page.dart';
import 'screens/history_page.dart';
import 'screens/settings_page.dart';
import 'services/websocket_service.dart';
import 'services/connection_service.dart';

void main() async {

  WidgetsFlutterBinding
      .ensureInitialized();

  unawaited(WebSocketService.connect());

  runApp(
    const SmartPillApp(),
  );
}

class SmartPillApp extends StatelessWidget {
  const SmartPillApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Smart Pill Reminder',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        fontFamily: 'Satoshi',

        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.blue,
        ),
      ),
      home: const MainScreen(),
    );
  }
}



class MainScreen extends StatefulWidget {
  const MainScreen({super.key});

  @override
  State<MainScreen> createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen> {

  int currentIndex = 0;

  final pages = [
    const DashboardPage(),
    const MedicationPage(),
    const SchedulePage(),
    const HistoryPage(),
    const SettingsPage(),
  ];

  @override
  Widget build(BuildContext context) {

    return Scaffold(
      body: pages[currentIndex],

      bottomNavigationBar: NavigationBar(
        selectedIndex: currentIndex,

        onDestinationSelected: (index) {

          final requiresConnection =

              index == 1 || // Medication
                  index == 2 || // Schedule
                  index == 3;   // History

          if (requiresConnection &&
              !ConnectionService
                  .esp32Connected) {

            ScaffoldMessenger.of(context)
                .showSnackBar(

              const SnackBar(
                content: Text(
                  "Connect to ESP32 first",
                ),
              ),
            );

            return;
          }

          setState(() {
            currentIndex = index;
          });
        },
        destinations: const [

          NavigationDestination(
            icon: Icon(Icons.home),
            label: "Home",
          ),

          NavigationDestination(
            icon: Icon(Icons.medication),
            label: "Medication",
          ),

          NavigationDestination(
            icon: Icon(Icons.schedule),
            label: "Schedule",
          ),

          NavigationDestination(
            icon: Icon(Icons.history),
            label: "History",
          ),

          NavigationDestination(
            icon: Icon(Icons.settings),
            label: "Settings",
          ),
        ],
      ),
    );
  }
}
