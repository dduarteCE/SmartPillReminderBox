import 'dart:convert';

import 'package:web_socket_channel/web_socket_channel.dart';

import '../models/dose_record.dart';
import 'storage_service.dart';

class WebSocketService {

  static WebSocketChannel? _channel;

  static bool connected = false;

  static Future<void> connect() async {

    try {

      final ip =
      await StorageService.loadEsp32Ip();

      final port =
      await StorageService.loadEsp32Port();

      final url =
          "ws://$ip:$port/ws/events";

      print(
        "Connecting WebSocket to $url",
      );

      _channel =
          WebSocketChannel.connect(
            Uri.parse(url),
          );

      connected = true;

      print(
        "WebSocket connected",
      );

      _channel!.stream.listen(

            (message) async {

          await _handleMessage(
            message,
          );
        },

        onDone: () {

          connected = false;

          print(
            "WebSocket disconnected",
          );
        },

        onError: (error) {

          connected = false;

          print(
            "WebSocket error: $error",
          );
        },
      );

      print(
        "WebSocket connected",
      );

    } catch (e) {

      connected = false;

      print(
        "WebSocket connect error: $e",
      );
    }
  }

  static void disconnect() {

    _channel?.sink.close();

    connected = false;
  }

  static Future<void>
  _handleMessage(
      dynamic message) async {

    try {

      final data =
      jsonDecode(message);

      final type =
      data["type"];

      if (type ==
          "DOSE_COMPLETED") {

        await _saveDoseRecord(
          data,
          "Taken",
        );
      }

      if (type ==
          "DOSE_MISSED") {

        await _saveDoseRecord(
          data,
          "Missed",
        );
      }

      print(
        "Event received: $type",
      );

    } catch (e) {

      print(
        "WebSocket parse error: $e",
      );
    }
  }

  static Future<void>
  _saveDoseRecord(
      Map<String, dynamic> data,
      String status) async {

    final history =
    await StorageService
        .loadHistory();

    history.add(

      DoseRecord(

        medicationName:
        data["medicationName"] ??
            "Unknown",

        status: status,

        timestamp:
        data["timestamp"] ??
            DateTime.now()
                .toIso8601String(),
      ),
    );

    await StorageService
        .saveHistory(
      history,
    );
  }
}