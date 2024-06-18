#include "PhMesure.h"
#include "Arduino.h"

PhMesure::PhMesure(int phSensorPin, int phNeutralVoltage, int phAcidVoltage)
    : phSensorPin(phSensorPin)
    , phNeutralVoltage(phNeutralVoltage)
    , phAcidVoltage(phAcidVoltage)
{
}

void PhMesure::begin() { pinMode(phSensorPin, INPUT); }

float PhMesure::mesurePh()
{
    int buffer_arr[10];
    float sumAvgVoltage = 0.0;

    for (int i = 0; i < 10; i++) {
        buffer_arr[i] = analogRead(phSensorPin);
        delay(10);
    }

    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (buffer_arr[i] > buffer_arr[j]) {
                int temp = buffer_arr[i];
                buffer_arr[i] = buffer_arr[j];
                buffer_arr[j] = temp;
            }
        }
    }

    for (int i = 2; i < 8; i++) {
        sumAvgVoltage += buffer_arr[i];
    }

    float avgVoltage = sumAvgVoltage / 6 / 4095 * 3300;

    debug_phVoltage = avgVoltage;

    float slope = (7.0 - 4.0) / ((phNeutralVoltage - 1500) / 3.0 - (phAcidVoltage - 1500) / 3.0);
    float intercept = 7.0 - slope * (phNeutralVoltage - 1500) / 3.0;

    float phValue = slope * (avgVoltage - 1500) / 3.0 + intercept;

    return phValue;
}
