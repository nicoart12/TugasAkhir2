#ifndef HEATING.H
#define HEATING.H

#include "var.h"
#include "loadcell.h"
#include "motor_control.h"
#include "bluetooth.h"

void heating(int Setpoint) {
  Serial.println("mulai memanaskan"); //nanti dihapus
  //delay(1000);
  digitalWrite(Enable, HIGH);
  SerialPort.print(slave_1_id);
  SerialPort.print("STOP");
  //Serial.println("terkirim");
  SerialPort.flush();
  digitalWrite(Enable, LOW);

  while (NilaiSuhu < Setpoint){
      NilaiSuhu = thermocouple.readCelsius();
      Serial.println("sedang memanaskan"); //nanti dihapus
      //delay(1000);
      myPID.Compute();
      int newVal = (int)Output;
      int preVal = dimmer.getPower();
      dimmer.setPower(newVal);
      //NilaiSuhu = Setpoint; //nanti dihapus
  }

  if (abs(NilaiSuhu - Setpoint) <= 0.1 || (NilaiSuhu > Setpoint)) {
    dimmer.setPower(0);
    //SEND DATA KE DISPLAY DAN APLIKASI KALAU SUDAH MULAI
    sendData("MULAI");
    Serial.println("kirim data ke display"); //nanti dihapus
    //delay(1000);
    digitalWrite(Enable, HIGH);
    SerialPort.print(slave_1_id);
    SerialPort.print("MULAI");
    Serial.println("terkirim");
    SerialPort.flush();
    digitalWrite(Enable, LOW);
    Serial.println("air sudah panas"); //nanti dihapus
    //delay(1000);
  }
  // else {
  //         // setpointReachedMillis = 0;
  // newVal = (int)Output;
  // }

}

void pouring(int volwater, int pourDuration, int pourInterval) {
  Serial.println("hanya pour"); //nanti dihapus
  //delay(1000);
  updateLoadCell();
  Serial.println("mulai pouring"); //nanti dihapus
  //delay(1000);
  int previousStartTime = 0;
  int currentStartTime = millis();
  while (average < volwater){
      controlMotor(volwater, pourDuration, motor2Pin);
      controlMotor3(255, motor3Pin); // PWM 0(off) atau 255(on)
      Serial.println("motor menyala"); //nanti dihapus
      //delay(1000);
      //average = volwater + 1;//nanti dihapus
  }

  updateLoadCell();
  if ((average == volwater) && (currentStartTime - stepStartTime) <= (pourDuration*1000)) {
    previousStartTime = currentStartTime;
    controlMotor3(0, motor2Pin); // PWM 0(off) atau 255(on)
    controlMotor3(0, motor3Pin); // PWM 0(off) atau 255(on)
    Serial.println("sedang mematikan motor"); //nanti dihapus
    //delay(1000);
    Serial.print("udah berhenti");
    Serial.println('\n');
    updateLoadCell();
    Serial.printf("total water weight: %.2f", average);
    Serial.println('\n');
    //SEND DATA KE DISPLAY KALAU SUDAH SELESAI 1 STEP
    digitalWrite(Enable, HIGH);
    SerialPort.print(slave_1_id);
    SerialPort.print("SELESAI");
    Serial.println("terkirim");
    SerialPort.flush();
    digitalWrite(Enable, LOW);
    delay(pourInterval * 1000);
  }
}

void pouringandHeat(int volwater, int pourDuration, int pourInterval, int Setpoint) {
  Serial.println("pour and heating"); //nanti dihapus
  //delay(1000);
  updateLoadCell();
    while (average <= volwater){
      controlMotor(volwater, pourDuration, motor2Pin);
      controlMotor3(255, motor3Pin); // PWM 0(off) atau 255(on)
      Serial.println("sedang menyalakan motor"); //nanti dihapus
      //delay(1000); // nanti dihapus
      //average = volwater + 1; // nanti dihapus
      updateLoadCell();
  }
  //   if ((millis() - stepStartTime >= pourDuration * 1000)) 
  // { // coba untuk baca data load cell ya
  updateLoadCell();
  if (average >= volwater) {
    Serial.println("sedang mematikan motor"); //nanti dihapus
    //delay(1000);
    controlMotor3(0, motor2Pin); // PWM 0(off) atau 255(on)
    controlMotor3(0, motor3Pin); // PWM 0(off) atau 255(on)
    Serial.print("udah berhenti");
    Serial.println('\n');
    updateLoadCell();
    Serial.printf("total water weight: %.2f", average);
    Serial.println('\n');
    //SEND DATA KE DISPLAY KALAU SUDAH SELESAI 1 STEP
    digitalWrite(Enable, HIGH);
    Serial.println("sedang mengirim data ke display"); //nanti dihapus
    //delay(1000);
    SerialPort.print(slave_1_id);
    SerialPort.print("SELESAI");
    //Serial.println("terkirim");
    SerialPort.flush();
    digitalWrite(Enable, LOW);
    //controlMotor(0, motorPin);
    //Memanaskan air pada saat pour interval
    currentheatingmillis = millis();
    if (currentheatingmillis - previousheatingmillis <= (pourInterval * 1000)) {
      heating(Setpoint);
    }
    else {
      previousheatingmillis = currentheatingmillis;
    }
  }
}

#endif