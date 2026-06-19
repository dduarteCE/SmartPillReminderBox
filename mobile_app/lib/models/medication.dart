class Medication {

  String name;
  String dosage;
  int drawerId;
  int pillCount;

  Medication({
    required this.name,
    required this.dosage,
    required this.drawerId,
    required this.pillCount,
  });

  Map<String, dynamic> toJson() {
    return {
      'name': name,
      'dosage': dosage,
      'drawerId': drawerId,
      'pillCount': pillCount,
    };
  }

  factory Medication.fromJson(
      Map<String, dynamic> json) {

    return Medication(
      name: json['name'],
      dosage: json['dosage'],
      drawerId: json['drawerId'],

      pillCount:
      json['pillCount'] ?? 0,
    );
  }
}