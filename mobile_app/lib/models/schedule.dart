class Schedule {

  String medicationName;
  String time;

  Schedule({
    required this.medicationName,
    required this.time,
  });

  Map<String, dynamic> toJson() {
    return {
      'medicationName': medicationName,
      'time': time,
    };
  }

  factory Schedule.fromJson(
      Map<String, dynamic> json) {

    return Schedule(
      medicationName: json['medicationName'],
      time: json['time'],
    );
  }
}