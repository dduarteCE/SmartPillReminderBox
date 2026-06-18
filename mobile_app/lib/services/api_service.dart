import '../services/storage_service.dart';

import 'dart:convert';

import 'package:http/http.dart' as http;

import 'storage_service.dart';

class ApiService {

  static Future<String> getBaseUrl() async {

    final ip =
    await StorageService.loadEsp32Ip();

    final port =
    await StorageService.loadEsp32Port();

    return "http://$ip:$port";
  }

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
}