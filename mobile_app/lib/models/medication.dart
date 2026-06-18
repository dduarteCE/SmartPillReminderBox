class Medication {
  String name;
  String dosage;
  int drawerId;

  Medication({
    required this.name,
    required this.dosage,
    required this.drawerId,
  });

  Map<String, dynamic> toJson() {
    return {
      'name': name,
      'dosage': dosage,
      'drawerId': drawerId,
    };
  }

  factory Medication.fromJson(Map<String, dynamic> json) {
    return Medication(
      name: json['name'],
      dosage: json['dosage'],
      drawerId: json['drawerId'],
    );
  }
}