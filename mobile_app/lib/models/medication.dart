class Medication {

  String name;
  int drawerId;
  int pillCount;

  Medication({
    required this.name,
    required this.drawerId,
    required this.pillCount,
  });

  Map<String, dynamic> toJson() {
    return {
      'name': name,
      'drawerId': drawerId,
      'pillCount': pillCount,
    };
  }

  factory Medication.fromJson(
      Map<String, dynamic> json) {

    return Medication(
      name: json['name'],
      drawerId: json['drawerId'],
      pillCount: json['pillCount'] ?? 0,
    );
  }
}