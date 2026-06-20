import 'dart:convert';

import 'package:http/http.dart' as http;
import '../models/medication.dart';
import '../models/schedule.dart';
import 'storage_service.dart';

class ApiService {

  static Future<String> getBaseUrl() async {

    final ip =
    await StorageService.loadEsp32Ip();

    final port =
    await StorageService.loadEsp32Port();

    return "http://$ip:$port";
  }

  // ==========================
  // HEALTH CHECK
  // ==========================

  static Future<Map<String, dynamic>?>
  getDeviceHealth() async {

    try {

      final baseUrl =
      await getBaseUrl();

      final response =
      await http.get(
        Uri.parse("$baseUrl/api/health"),
      ).timeout(
        const Duration(seconds: 3),
      );

      if (response.statusCode == 200) {

        return jsonDecode(
          response.body,
        );
      }

    } catch (e) {

      print(
        "Health check error: $e",
      );
    }

    return null;
  }

  // ==========================
  // CREATE SCHEDULE
  // ==========================

  static Future<Schedule?> createSchedule(
      Schedule schedule) async {

    try {

      final baseUrl =
      await getBaseUrl();

      List<Map<String, dynamic>>
      formattedTimes = [];

      for (String time in schedule.times) {

        final parts =
        time.split(":");

        formattedTimes.add({
          "hour":
          int.parse(parts[0]),

          "minute":
          int.parse(parts[1]),
        });
      }

      final body = {

        "drawerId":
        schedule.drawerId,

        "times":
        formattedTimes,

        "daysOfWeek":
        schedule.daysOfWeek,

        "enabled":
        schedule.enabled,
      };

      final response =
      await http.post(
        Uri.parse(
          "$baseUrl/api/schedules",
        ),

        headers: {
          "Content-Type":
          "application/json",
        },

        body: jsonEncode(
          body,
        ),
      );

      print(
        "Create Schedule Response: ${response.body}",
      );

      if (response.statusCode == 200 ||
          response.statusCode == 201) {

        final data =
        jsonDecode(response.body);

        final scheduleData =
        data["schedule"];

        return Schedule(

          id:
          scheduleData["id"],

          drawerId:
          scheduleData["drawerId"],

          times:
          schedule.times,

          daysOfWeek:
          schedule.daysOfWeek,

          enabled:
          schedule.enabled,
        );
      }

      return null;

    } catch (e) {

      print(
        "Create Schedule Error: $e",
      );

      return null;
    }
  }

  // ==========================
  // UPDATE DRAWER
  // ==========================

  static Future<bool> updateDrawer(
      Medication medication) async {

    try {

      final baseUrl =
      await getBaseUrl();

      final response =
      await http.put(

        Uri.parse(
          "$baseUrl/api/drawers/${medication.drawerId}",
        ),

        headers: {
          "Content-Type":
          "application/json",
        },

        body: jsonEncode({

          "medicationName":
          medication.name,

          "enabled":
          true,

          "pillCount":
          medication.pillCount,
        }),
      );

      print(
        "Update Drawer Response: ${response.body}",
      );

      return response.statusCode == 200;

    } catch (e) {

      print(
        "Update Drawer Error: $e",
      );

      return false;
    }
  }

  static Future<List<dynamic>?>
  getSchedules() async {

    try {

      final baseUrl =
      await getBaseUrl();

      final response =
      await http.get(
        Uri.parse(
          "$baseUrl/api/schedules",
        ),
      );

      if (response.statusCode == 200) {

        final data =
        jsonDecode(
          response.body,
        );

        if (data["success"] ==
            true) {

          return data[
          "schedules"];
        }
      }

    } catch (e) {

      print(
        "Get Schedules Error: $e",
      );
    }

    return null;
  }

  // ==========================
  // DELETE DRAWER
  // ==========================

  static Future<bool> deleteDrawer(
      int drawerId) async {

    try {

      final baseUrl =
      await getBaseUrl();

      final response =
      await http.delete(
        Uri.parse(
          "$baseUrl/api/drawers/$drawerId",
        ),
      );

      print(
        "Delete Drawer Response: ${response.body}",
      );

      return response.statusCode == 200;

    } catch (e) {

      print(
        "Delete Drawer Error: $e",
      );

      return false;
    }
  }
  // ==========================
  // SET DEVICE TIME
  // ==========================

  static Future<bool> setDeviceTime() async {

    try {

      final baseUrl =
      await getBaseUrl();

      final now =
      DateTime.now();

      const weekDays = [
        "MON",
        "TUE",
        "WED",
        "THU",
        "FRI",
        "SAT",
        "SUN",
      ];

      final currentDate =
          "${now.year.toString().padLeft(4, '0')}-"
          "${now.month.toString().padLeft(2, '0')}-"
          "${now.day.toString().padLeft(2, '0')}";

      final currentTime =
          "${now.hour.toString().padLeft(2, '0')}:"
          "${now.minute.toString().padLeft(2, '0')}";

      final dayOfWeek =
      weekDays[now.weekday - 1];

      final response =
      await http.put(

        Uri.parse(
          "$baseUrl/api/time",
        ),

        headers: {
          "Content-Type":
          "application/json",
        },

        body: jsonEncode({

          "currentDate":
          currentDate,

          "currentTime":
          currentTime,

          "dayOfWeek":
          dayOfWeek,
        }),
      );

      print(
        "Set Time Response: ${response.body}",
      );

      return response.statusCode == 200;

    } catch (e) {

      print(
        "Set Time Error: $e",
      );

      return false;
    }
  }

  // ==========================
  // DELETE SCHEDULE
  // ==========================

  static Future<bool> deleteSchedule(
      int scheduleId) async {

    try {

      final baseUrl =
      await getBaseUrl();

      final response =
      await http.delete(
        Uri.parse(
          "$baseUrl/api/schedules/$scheduleId",
        ),
      );

      print(
        "Delete Schedule Response: ${response.body}",
      );

      return response.statusCode == 200;

    } catch (e) {

      print(
        "Delete Schedule Error: $e",
      );

      return false;
    }
  }

  // ==========================
  // GET PENDING EVENTS
  // ==========================

  static Future<List<Map<String, dynamic>>?> getPendingEvents() async {

    try {
      final baseUrl = await getBaseUrl();

      final response = await http
          .get(Uri.parse("$baseUrl/api/events"))
          .timeout(const Duration(seconds: 3));

      if (response.statusCode != 200) {
        return null;
      }

      final data = jsonDecode(response.body);
      if (data is! Map<String, dynamic> || data["success"] != true) {
        return null;
      }

      final events = data["events"];
      if (events is! List) {
        return null;
      }

      return events
          .whereType<Map>()
          .map((event) => Map<String, dynamic>.from(event))
          .toList();

    } catch (e) {
      print("Get Events Error: $e");
      return null;
    }
  }

  // ==========================
  // ACKNOWLEDGE EVENTS
  // ==========================

  static Future<bool> acknowledgeEvents(
      List<int> eventIds) async {

    try {

      final baseUrl =
      await getBaseUrl();

      final response =
      await http.post(

        Uri.parse(
          "$baseUrl/api/events/ack",
        ),

        headers: {
          "Content-Type":
          "application/json",
        },

        body: jsonEncode({

          "eventIds":
          eventIds,
        }),
      ).timeout(
        const Duration(seconds: 3),
      );

      print(
        "ACK Response: ${response.body}",
      );

      return response.statusCode == 200;

    } catch (e) {

      print(
        "ACK Error: $e",
      );

      return false;
    }
  }

}
