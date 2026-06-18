import 'dart:convert';

import 'package:http/http.dart' as http;

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
        Uri.parse(
          "$baseUrl/api/health",
        ),
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

  static Future<bool> createSchedule(
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

      return response.statusCode == 200 ||
          response.statusCode == 201;

    } catch (e) {

      print(
        "Create Schedule Error: $e",
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
}