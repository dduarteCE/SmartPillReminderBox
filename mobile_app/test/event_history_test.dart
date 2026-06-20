import 'package:flutter_test/flutter_test.dart';
import 'package:mobile_app/models/dose_record.dart';
import 'package:mobile_app/services/storage_service.dart';
import 'package:shared_preferences/shared_preferences.dart';

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    SharedPreferences.setMockInitialValues({});
  });

  test('stores consecutive ESP32 events without losing either record', () async {
    final completed = DoseRecord(
      eventId: 10,
      medicationName: 'Aspirin',
      status: 'Taken',
      timestamp: '2026-06-19T10:30:00',
    );
    final empty = DoseRecord(
      eventId: 11,
      medicationName: 'Aspirin',
      status: 'Empty',
      timestamp: '2026-06-19T10:30:00',
    );

    await Future.wait([
      StorageService.appendHistory(completed),
      StorageService.appendHistory(empty),
    ]);

    final history = await StorageService.loadHistory();

    expect(history.map((record) => record.eventId), [10, 11]);
  });

  test('does not store a recovered event twice', () async {
    final event = DoseRecord(
      eventId: 20,
      medicationName: 'Vitamin D',
      status: 'Missed',
      timestamp: '2026-06-19T12:00:00',
    );

    expect(await StorageService.appendHistory(event), isTrue);
    expect(await StorageService.appendHistory(event), isFalse);
    expect(await StorageService.loadHistory(), hasLength(1));
  });

  test('uses the ESP32 access point ports by default', () async {
    expect(await StorageService.loadEsp32Ip(), '192.168.4.1');
    expect(await StorageService.loadEsp32Port(), '80');
    expect(await StorageService.loadEsp32WsPort(), '81');
  });
}
