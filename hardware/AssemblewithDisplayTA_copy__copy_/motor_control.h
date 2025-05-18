#ifndef MOTOR_CONTROL.H
#define MOTOR_CONTROL.H

void controlMotor(int volwater, int pourDuration, int motorpin)
{
  Serial.println("Motor menyala controlMotor ");
  int waterspeed = volwater/pourDuration;
  //speed = 10*waterspeed + 2; // buat persamaan linear utk speed motor dan waterspeed 
  float aspeed = 38.394 + 5.033 * waterspeed;

  int speed = round(aspeed);

  if (speed < 70) {
    lessSpeed = true;
    speed = 70;
    pourIntervalLess = volwater * speed;
  }

  analogWrite(motorPin, speed);
  delay(20);
}

void controlMotor3(int speed, int motorpin)
{
  //int waterspeed = volwater/pourDuration;
  //speed = 10*waterspeed + 2; // buat persamaan linear utk speed motor dan waterspeed 
  //speed = 255;
  Serial.println("Motor menyala controlMotor3 ");
  analogWrite(motorPin, speed);
  delay(20);
}
#endif