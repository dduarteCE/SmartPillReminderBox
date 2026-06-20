class DoseRecord {

  int? eventId;
  String medicationName;
  String status;
  String timestamp;

  DoseRecord({
    this.eventId,
    required this.medicationName,
    required this.status,
    required this.timestamp,
  });

  Map<String, dynamic> toJson() {
    return {
      if (eventId != null) 'eventId': eventId,
      'medicationName': medicationName,
      'status': status,
      'timestamp': timestamp,
    };
  }

  factory DoseRecord.fromJson(
      Map<String, dynamic> json) {

    return DoseRecord(
      eventId: json['eventId'] as int?,
      medicationName: json['medicationName'] as String,
      status: json['status'] as String,
      timestamp: json['timestamp'] as String,
    );
  }
}
