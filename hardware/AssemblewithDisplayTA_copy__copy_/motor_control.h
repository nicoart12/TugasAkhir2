#ifndef MOTOR_CONTROL.H
#define MOTOR_CONTROL.H

void controlMotor(int volwater, int pourDuration, int motorpin)
{
  int waterspeed = volwater/pourDuration;
  //speed = 10*waterspeed + 2; // buat persamaan linear utk speed motor dan waterspeed 
  float aspeed = 45.895 +3.230*waterspeed +0.091*(waterspeed)*(waterspeed);

  int speed = round(aspeed);
  //int speed = 255;
  analogWrite(motorPin, speed);
  delay(20);
}

void controlMotor3(int speed, int motorpin)
{
  //int waterspeed = volwater/pourDuration;
  //speed = 10*waterspeed + 2; // buat persamaan linear utk speed motor dan waterspeed 
  //speed = 255;
  analogWrite(motorPin, speed);
  delay(20);
}
#endif