class DoseRecord {

  String medicationName;
  String status;
  String timestamp;

  DoseRecord({
    required this.medicationName,
    required this.status,
    required this.timestamp,
  });

  Map<String, dynamic> toJson() {
    return {
      'medicationName': medicationName,
      'status': status,
      'timestamp': timestamp,
    };
  }

  factory DoseRecord.fromJson(
      Map<String, dynamic> json) {

    return DoseRecord(
      medicationName: json['medicationName'],
      status: json['status'],
      timestamp: json['timestamp'],
    );
  }
}