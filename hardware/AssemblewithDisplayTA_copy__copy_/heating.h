#ifndef HEATING.H
#define HEATING.H

#include "var.h"
#include "loadcell.h"
#include "motor_control.h"
#include "bluetooth.h"

void choosePower(float difference){
  if (difference < 90) {
    dimmer.setPower(80);
  }
  else   if (difference < 40) {
    dimmer.setPower(40);
  }
    else   if (difference < 10) {
    dimmer.setPower(8);
  }
  else {
    Serial.println("ERROR DIFFERENCE ");
    dimmer.setPower(8);
  }
}

void updateTemperature() {
  // basic readout test, just print the current temp

  if (millis() - time_read_temp >= 500) 
  {
    time_read_temp = millis();
    temp_raw[currentIndex] = (thermocouple.readCelsius());
    currentIndex = (currentIndex + 1) % numReadings;
  }

  // temp_avg = 0; 

  for (int i = 0; i < numReadings; i++)
  {
    temp_avg += temp_raw[i];
  }

   temp_avg = (temp_avg / numReadings);
  // kalibrasi
   //temp_avg = temp_avg *2.43 - 63.3; 
   
   temp_avg =  temp_avg + 6;
   NilaiSuhu = temp_avg;
  //  Serial.print("temp: ");
  //  Serial.println(temp_avg  ,1);
}

void heating(int Setpoint) {
  Serial.println("mulai memanaskan"); //nanti dihapus
  //PAKAI PENGIRIMAN MULAI PEMANASAN

  
  //mengukur suhu awal 
  updateTemperature();
  float difference = NilaiSuhu - Setpoint;
  choosePower(difference);
  bool targetReached = (abs(difference) <= 1) || (NilaiSuhu >= Setpoint);
  while (!targetReached){
      
      // NilaiSuhu = thermocouple.readCelsius();
      Serial.println("sedang memanaskan"); //nanti dihapus
      updateTemperature();
      NilaiSuhu = Setpoint; //nanti dihapus
      difference = NilaiSuhu - Setpoint;
      targetReached = (abs(difference) <= 1) || (NilaiSuhu >= Setpoint);

  }

  if (abs(NilaiSuhu - Setpoint) <= 0.1 || (NilaiSuhu > Setpoint)) {
    dimmer.setPower(0);
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
  //delay(1000);
  int previousStartTime = millis();
  int currentStartTime = millis();
  int previousStartSend = millis();

while ((average < volwater) || (currentStartTime - previousStartTime) < (pourDuration * 1000)) {
    average = volwater; // nanti dihapus
    currentStartTime = millis();

    Serial.print("Poured : ");
    Serial.print(average, 1);
    Serial.println("mL");
        //PENGIRIMAN DATA SELAMA PROSES
    updateLoadCell();
    updateTemperature();
    int remainingTime = (pourDuration - millis())/1000;
    String message = "process," + String(countStep) + ",0," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval

    //interval pengiriman data 500ms
    if ((currentStartTime - previousStartSend) >= 500) {
    previousStartSend = currentStartTime;
    sendData(message.c_str());
    sendDataD(message);
    }

    //CEK APABILA POWER MOTOR TIDAK MENCAPAI MINIMAL
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motor2Pin);
    }
    else {
      if ((millis() - stepStartTime) > (pourIntervalLess * 1000)) { //Dibuat agar saat power motor tidak mencapai batas minimum, maka pour duration akan menurun dan kecepatan meningkat untuk mencapai volume yang tetap sama. Namun, saat sisa waktu (pourDuration - pourIntervalLess) , motor akan diam saja. 
        controlMotor3(0,motor2Pin); //nanti ganti
      }
    }
    controlMotor3(80, motor3Pin);
}
  //nanti dihapus
  updateLoadCell();
  controlMotor3(0, motor2Pin); // PWM 0(off) atau 255(on)
  controlMotor3(0, motor3Pin); // PWM 0(off) atau 255(on)
  Serial.println("sedang mematikan motor"); //nanti dihapus
  Serial.print("udah berhenti");
  Serial.println('\n');
  updateLoadCell();
  Serial.printf("total water weight: %.2f", average);
  Serial.println('\n');

  //INTERVAL
  unsigned long previousWaitInterval = 0;  // Should be declared at global scope
  pourInterval = pourInterval * 1000;  // Convert to milliseconds
  unsigned long currentTimeInterval = millis();
  unsigned long previousWaitSend = currentTimeInterval;
  Serial.println("wait for pour interval");
  previousWaitInterval = currentTimeInterval;  // Update the timer

  while ((currentTimeInterval - previousWaitInterval) >= pourInterval) {
    //PENGIRIMAN DATA SELAMA PROSES
    updateLoadCell();
    updateTemperature();
    currentTimeInterval = millis();
    int remainingTime = ((pourDuration - currentTimeInterval)/1000);
    String message = "process," + String(countStep) + ",0," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval
    
    //interval pengiriman data 500ms
    if ((currentTimeInterval - previousWaitSend) >= 500) {
    previousWaitSend = currentTimeInterval;
    sendData(message.c_str());
    sendDataD(message);
    }
  }
  controlMotor3(0, motorPin);
  Serial.println("pour interval done");
}

void pouringandHeat(int volwater, int pourDuration, int pourInterval) {
  Serial.println("memulai pouring dan heat");
  dimmer.setPower(0);
  //controlMotor3(80, motorPin); //MOTOR 1 MENYALA SELALU
  stepStartTime = millis();
  updateLoadCell();
  controlMotor(volwater, pourDuration, motor2Pin);//UNTUK MEMBUAT VARIABEL lessSpeed TRUE
  int previousStartTime = millis();
  int currentStartTime = millis();
  int previousStartSend = millis();
  
while ((average < volwater) || (currentStartTime - previousStartTime) < (pourDuration * 1000)) {
    average = volwater; //nanti dihapus
    currentStartTime = millis();
    Serial.print("Poured : ");
    Serial.print(average, 1);
    Serial.println("mL");
    //PENGIRIMAN DATA SELAMA PROSES
    updateLoadCell();
    updateTemperature();
    int remainingTime = (pourDuration - millis())/1000;
    String message = "process," + String(countStep) + ",0," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval

    if ((currentStartTime - previousStartSend) >= 500) {
    previousStartSend = currentStartTime;
    sendData(message.c_str());
    sendDataD(message);
    }
    
    updateLoadCell();  // Pastikan ini meng-update `average`
    
    //CEK APABILA POWER MOTOR TIDAK MENCAPAI MINIMAL
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motor2Pin);
    }
    else {
      if ((millis() - stepStartTime) > (pourIntervalLess * 1000)) { //Dibuat agar saat power motor tidak mencapai batas minimum, maka pour duration akan menurun dan kecepatan meningkat untuk mencapai volume yang tetap sama. Namun, saat sisa waktu (pourDuration - pourIntervalLess) , motor akan diam saja. 
        controlMotor3(0,motor2Pin); //nanti ganti
      }
    }

    controlMotor3(80, motor3Pin);
}

// Cek apakah loop berhenti karena waktu habis atau volume tercapai
if (average >= volwater) {
    Serial.println("Selesai: Volume tercapai!");
} else {
    Serial.println("Berhenti: Waktu habis sebelum volume tercapai!");
}
  Serial.println(" Pouring telah selesai");
  // Turn off all motors
  controlMotor3(0, motorPin);
  controlMotor3(0, motor2Pin);
  controlMotor3(0, motor3Pin);
  
  Serial.print("Poured volume final : ");
  Serial.print(average);
  Serial.println(" ml");
  

  // Wait for interval
  unsigned long previousWaitInterval = 0;  // Should be declared at global scope
  pourInterval = pourInterval * 1000;  // Convert to milliseconds
  
  unsigned long currentTimeInterval = millis();
  previousWaitInterval = currentTimeInterval;  // Update the timer
  unsigned long previousWaitSend = currentTimeInterval;
  Serial.println("wait for pour interval");

  while ((currentTimeInterval - previousWaitInterval) >= pourInterval) {
    updateLoadCell();
    updateTemperature();
    currentTimeInterval = millis();
    int remainingTime = ((pourDuration - currentTimeInterval)/1000);
    String message = "process," + String(countStep) + ",0," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval
    
    //interval pengiriman data 500ms
    if ((currentTimeInterval - previousWaitSend) >= 500) {
    previousWaitSend = currentTimeInterval;
    sendData(message.c_str());
    sendDataD(message);
    }
    controlMotor((200 - volwater), pourInterval, motorPin); // Menyalakan motor 1 untuk mengisi 
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motorPin);
    }
    else {
      if ((millis() - stepStartTime) >= (pourIntervalLess * 1000)) {
        controlMotor3(0,motorPin); //nanti ganti
      }
    }
  }
  controlMotor3(0, motorPin);
  Serial.println("pour interval done");
}

void rinse(int volwater, int pourDuration, int pourInterval) {
  Serial.println("hanya rinse"); //nanti dihapus
  //delay(1000);
  updateLoadCell();
  Serial.println("mulai pouring"); //nanti dihapus
  //delay(1000);
  int previousStartTime = millis();

while ((average < volwater) || (millis() - previousStartTime) < (pourDuration * 1000)) {
    average = volwater; // nanti dihapus

    Serial.print("Poured rinse: ");
    Serial.print(average, 1);
    Serial.println("mL");
    //CEK APABILA POWER MOTOR TIDAK MENCAPAI MINIMAL
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motor2Pin);
    }
    else {
      if ((millis() - stepStartTime) > (pourIntervalLess * 1000)) { //Dibuat agar saat power motor tidak mencapai batas minimum, maka pour duration akan menurun dan kecepatan meningkat untuk mencapai volume yang tetap sama. Namun, saat sisa waktu (pourDuration - pourIntervalLess) , motor akan diam saja. 
        controlMotor3(0,motor2Pin); //nanti ganti
      }
    }
    controlMotor3(80, motor3Pin);
}

  updateLoadCell();
  controlMotor3(0, motor2Pin); // PWM 0(off) atau 255(on)
  controlMotor3(0, motor3Pin); // PWM 0(off) atau 255(on)
  Serial.println("sedang mematikan motor"); //nanti dihapus
  Serial.print("udah berhenti");
  Serial.println('\n');
  updateLoadCell();
  Serial.printf("total water weight: %.2f", average);
  Serial.println('\n');
  unsigned long previousWaitInterval = 0;  // Should be declared at global scope
  pourInterval = pourInterval * 1000;  // Convert to milliseconds
  unsigned long currentTimeInterval = millis();
  Serial.println("wait for pour interval");
  while ((currentTimeInterval - previousWaitInterval) >= pourInterval) {
    previousWaitInterval = currentTimeInterval;  // Update the timer
  }
  controlMotor3(0, motorPin);
  Serial.println("pour interval done");
}

#endif