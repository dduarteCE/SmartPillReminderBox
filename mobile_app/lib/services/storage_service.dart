import 'dart:async';
import 'dart:convert';

import 'package:shared_preferences/shared_preferences.dart';

import '../models/medication.dart';
import '../models/schedule.dart';
import '../models/dose_record.dart';

class StorageService {

  // ==========================
  // MEDICATIONS
  // ==========================

  static const String medicationKey = "medications";

  static Future<void> saveMedications(
      List<Medication> meds) async {

    final prefs =
    await SharedPreferences.getInstance();

    List<String> jsonList =
    meds.map((m) => jsonEncode(m.toJson())).toList();

    await prefs.setStringList(
      medicationKey,
      jsonList,
    );
  }

  static Future<List<Medication>> loadMedications()
  async {

    final prefs =
    await SharedPreferences.getInstance();

    final jsonList =
    prefs.getStringList(medicationKey);

    if (jsonList == null) {
      return [];
    }

    return jsonList
        .map(
          (e) => Medication.fromJson(
        jsonDecode(e),
      ),
    )
        .toList();
  }

  // ==========================
  // SCHEDULES
  // ==========================

  static const String scheduleKey = "schedules";

  static Future<void> saveSchedules(
      List<Schedule> schedules) async {

    final prefs =
    await SharedPreferences.getInstance();

    List<String> jsonList =
    schedules
        .map(
          (s) => jsonEncode(s.toJson()),
    )
        .toList();

    await prefs.setStringList(
      scheduleKey,
      jsonList,
    );
  }

  static Future<List<Schedule>> loadSchedules()
  async {

    final prefs =
    await SharedPreferences.getInstance();

    final jsonList =
    prefs.getStringList(scheduleKey);

    if (jsonList == null) {
      return [];
    }

    return jsonList
        .map(
          (e) => Schedule.fromJson(
        jsonDecode(e),
      ),
    )
        .toList();
  }

  // ==========================
  // HISTORY
  // ==========================

  static const String historyKey = "history";

  static final StreamController<void> _historyChanges =
      StreamController<void>.broadcast();

  static Future<void> _historyQueue = Future<void>.value();

  static Stream<void> get historyChanges => _historyChanges.stream;

  static Future<void> saveHistory(
      List<DoseRecord> records) {

    final snapshot = List<DoseRecord>.from(records);

    return _enqueueHistoryOperation(() async {
      await _writeHistory(snapshot);
    });
  }

  static Future<bool> appendHistory(
      DoseRecord record) {

    return _enqueueHistoryOperation(() async {
      final records = await loadHistory();

      final alreadyStored = record.eventId != null &&
          records.any(
            (existing) =>
                existing.eventId == record.eventId &&
                existing.timestamp == record.timestamp,
          );

      if (alreadyStored) {
        return false;
      }

      records.add(record);
      await _writeHistory(records);
      return true;
    });
  }

  static Future<T> _enqueueHistoryOperation<T>(
      Future<T> Function() operation) {

    final result = _historyQueue.then((_) => operation());

    _historyQueue = result.then<void>(
      (_) {},
      onError: (_, _) {},
    );

    return result;
  }

  static Future<void> _writeHistory(
      List<DoseRecord> records) async {

    final prefs =
    await SharedPreferences.getInstance();

    List<String> jsonList =
    records
        .map(
          (r) => jsonEncode(r.toJson()),
    )
        .toList();

    await prefs.setStringList(
      historyKey,
      jsonList,
    );

    _historyChanges.add(null);
  }

  static Future<List<DoseRecord>> loadHistory()
  async {

    final prefs =
    await SharedPreferences.getInstance();

    final jsonList =
    prefs.getStringList(historyKey);

    if (jsonList == null) {
      return [];
    }

    return jsonList
        .map(
          (e) => DoseRecord.fromJson(
        jsonDecode(e),
      ),
    )
        .toList();
  }

  // ==========================
  // ESP32 SETTINGS
  // ==========================

  static const String esp32IpKey =
      "esp32_ip";

  static const String esp32PortKey =
      "esp32_port";

  static const String esp32WsPortKey =
      "esp32_ws_port";

  static Future<void> saveEsp32Ip(
      String ip) async {

    final prefs =
    await SharedPreferences
        .getInstance();

    await prefs.setString(
      esp32IpKey,
      ip,
    );
  }

  static Future<String> loadEsp32Ip()
  async {

    final prefs =
    await SharedPreferences
        .getInstance();

    return prefs.getString(
        esp32IpKey) ??
        "192.168.4.1";
  }

  static Future<void> saveEsp32Port(
      String port) async {

    final prefs =
    await SharedPreferences
        .getInstance();

    await prefs.setString(
      esp32PortKey,
      port,
    );
  }

  static Future<String> loadEsp32Port()
  async {

    final prefs =
    await SharedPreferences
        .getInstance();

    return prefs.getString(
        esp32PortKey) ??
        "80";
  }

  static Future<void> saveEsp32WsPort(
      String port) async {

    final prefs =
    await SharedPreferences
        .getInstance();

    await prefs.setString(
      esp32WsPortKey,
      port,
    );
  }

  static Future<String> loadEsp32WsPort()
  async {

    final prefs =
    await SharedPreferences
        .getInstance();

    return prefs.getString(
        esp32WsPortKey) ??
        "81";
  }
}
