import 'dart:async';
import 'dart:convert';

import 'package:web_socket_channel/web_socket_channel.dart';

import '../models/dose_record.dart';
import 'api_service.dart';
import 'storage_service.dart';

class WebSocketService {
  static WebSocketChannel? _channel;
  static StreamSubscription<dynamic>? _subscription;
  static Future<void> _eventQueue = Future<void>.value();

  static bool connected = false;

  static Future<void> connect() async {
    await disconnect();

    final recovery = recoverPendingEvents();

    try {
      final ip = await StorageService.loadEsp32Ip();
      final wsPort = await StorageService.loadEsp32WsPort();
      final url = 'ws://$ip:$wsPort/';

      print('Connecting WebSocket to $url');

      final channel = WebSocketChannel.connect(Uri.parse(url));
      _channel = channel;
      _subscription = channel.stream.listen(
        _enqueueMessage,
        onDone: () {
          if (identical(_channel, channel)) {
            connected = false;
            print('WebSocket disconnected');
          }
        },
        onError: (Object error) {
          if (identical(_channel, channel)) {
            connected = false;
            print('WebSocket error: $error');
          }
        },
      );

      await channel.ready.timeout(const Duration(seconds: 3));

      if (identical(_channel, channel)) {
        connected = true;
        print('WebSocket connected');
      }
    } catch (error) {
      print('WebSocket connect error: $error');
      await disconnect();
    }

    await recovery;
  }

  static Future<void> recoverPendingEvents() async {
    final events = await ApiService.getPendingEvents();
    if (events == null) {
      return;
    }

    for (final event in events) {
      await _enqueueEvent(event);
    }
  }

  static void sendTestMessage() {
    if (!connected || _channel == null) {
      print('WebSocket not connected');
      return;
    }

    _channel!.sink.add(jsonEncode({'message': 'Hello from Flutter'}));
  }

  static Future<void> disconnect() async {
    connected = false;

    final subscription = _subscription;
    final channel = _channel;
    _subscription = null;
    _channel = null;

    await subscription?.cancel();
    await channel?.sink.close();
  }

  static void _enqueueMessage(dynamic message) {
    _eventQueue = _eventQueue.then((_) async {
      try {
        final decoded = jsonDecode(message);
        if (decoded is! Map<String, dynamic>) {
          throw const FormatException('Event payload must be a JSON object');
        }

        await _processEvent(decoded);
      } catch (error) {
        print('WebSocket parse error: $error');
      }
    });
  }

  static Future<void> _enqueueEvent(Map<String, dynamic> event) {
    _eventQueue = _eventQueue.then((_) => _processEvent(event)).catchError(
      (Object error, StackTrace stackTrace) {
        print('Event recovery error: $error');
      },
    );

    return _eventQueue;
  }

  static Future<void> _processEvent(Map<String, dynamic> data) async {
    final type = data['type']?.toString();
    final eventId = _readInt(data['id']);
    var handled = false;

    switch (type) {
      case 'DOSE_COMPLETED':
        final stored = await _saveDoseRecord(data, 'Taken', eventId);
        final drawerId = _readInt(data['drawerId']);
        if (stored && drawerId != null) {
          await _decrementPillCount(drawerId);
        }
        handled = true;
        break;
      case 'DOSE_MISSED':
        await _saveDoseRecord(data, 'Missed', eventId);
        handled = true;
        break;
      case 'DRAWER_EMPTY':
        await _saveDoseRecord(data, 'Empty', eventId);
        handled = true;
        break;
      case 'REMINDER_STARTED':
      case 'DRAWER_OPENED':
      case 'DRAWER_CLOSED':
        handled = true;
        break;
      default:
        print('Unknown ESP32 event type: $type');
    }

    if (handled && eventId != null) {
      final acknowledged = await ApiService.acknowledgeEvents([eventId]);
      if (!acknowledged) {
        print('Could not acknowledge ESP32 event $eventId');
      }
    }

    print('ESP32 event received: $data');
  }

  static int? _readInt(dynamic value) {
    if (value is int) {
      return value;
    }
    if (value is num) {
      return value.toInt();
    }
    return int.tryParse(value?.toString() ?? '');
  }

  static Future<void> _decrementPillCount(int drawerId) async {
    final medications = await StorageService.loadMedications();

    for (final medication in medications) {
      if (medication.drawerId == drawerId && medication.pillCount > 0) {
        medication.pillCount--;
        await StorageService.saveMedications(medications);
        return;
      }
    }
  }

  static Future<bool> _saveDoseRecord(
    Map<String, dynamic> data,
    String status,
    int? eventId,
  ) {
    return StorageService.appendHistory(
      DoseRecord(
        eventId: eventId,
        medicationName: data['medicationName']?.toString() ?? 'Unknown',
        status: status,
        timestamp: data['timestamp']?.toString() ??
            DateTime.now().toIso8601String(),
      ),
    );
  }
}
