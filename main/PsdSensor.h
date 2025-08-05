#ifndef PSDSENSOR_H
#define PSDSENSOR_H

class PsdSensor {
  private:
    int analogPin;
    float distanceCm;
    const float ADC_REF_VOLTAGE = 3.3;
    const int ADC_RESOLUTION = 4095;
    
  public:
    PsdSensor(int pin);
    void update();
    float getDistance() const;
};

#endif 