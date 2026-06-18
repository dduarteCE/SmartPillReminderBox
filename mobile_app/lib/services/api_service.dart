import 'dart:convert';

import 'package:http/http.dart' as http;

class ApiService {

  static String baseUrl =
      "http://192.168.1.100";

  static Future<Map<String, dynamic>?>
  getDeviceHealth() async {

    try {

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
      print(e);
    }

    return null;
  }
}