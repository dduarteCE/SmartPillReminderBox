import 'package:flutter/material.dart';
import 'screens/medication_page.dart';
import 'screens/schedule_page.dart';
import 'screens/dashboard_page.dart';
import 'screens/history_page.dart';

void main() {
  runApp(const SmartPillApp());
}

class SmartPillApp extends StatelessWidget {
  const SmartPillApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Smart Pill Reminder',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
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
  ];

  @override
  Widget build(BuildContext context) {

    return Scaffold(
      body: pages[currentIndex],

      bottomNavigationBar: NavigationBar(
        selectedIndex: currentIndex,

        onDestinationSelected: (index) {
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
        ],
      ),
    );
  }
}
