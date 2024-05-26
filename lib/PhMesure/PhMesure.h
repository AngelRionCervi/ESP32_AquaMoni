class PhMesure {
public:
    float debug_phVoltage;
    int phSensorPin;
    int phNeutralVoltage;
    int phAcidVoltage;

public:
    PhMesure(int phSensorPin, int phNeutralVoltage, int phAcidVoltage);
    void begin();
    float mesurePh();
};
