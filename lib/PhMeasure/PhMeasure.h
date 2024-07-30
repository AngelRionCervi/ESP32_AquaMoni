class PhMeasure {
 public:
  float debug_phVoltage;
  int phSensorPin;
  int phNeutralVoltage;
  int phAcidVoltage;

 public:
  PhMeasure(int phSensorPin, int phNeutralVoltage, int phAcidVoltage);
  void begin();
  float mesurePh();
};
