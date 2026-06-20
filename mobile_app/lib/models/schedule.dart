class Schedule {

  int? id;

  int drawerId;

  List<String> times;

  List<String> daysOfWeek;

  bool enabled;

  Schedule({
    this.id,
    required this.drawerId,
    required this.times,
    required this.daysOfWeek,
    required this.enabled,
  });

  Map<String, dynamic> toJson() {
    return {
      'id': id,
      'drawerId': drawerId,
      'times': times,
      'daysOfWeek': daysOfWeek,
      'enabled': enabled,
    };
  }

  factory Schedule.fromJson(
      Map<String, dynamic> json) {

    return Schedule(

      id: json['id'],

      drawerId: json['drawerId'],

      times: List<String>.from(
        json['times'],
      ),

      daysOfWeek: List<String>.from(
        json['daysOfWeek'],
      ),

      enabled: json['enabled'],
    );
  }
}