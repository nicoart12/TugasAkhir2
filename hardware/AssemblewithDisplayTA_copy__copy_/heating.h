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

void heating_rinse(int Setpoint) {
  Serial.println("mulai memanaskan rinse "); //nanti dihapus
  //PAKAI PENGIRIMAN MULAI PEMANASAN
  //mengukur suhu awal 
  //updateTemperature(); //nanti dihapus
  float difference = NilaiSuhu - Setpoint;
  choosePower(difference);
  bool targetReached = (abs(difference) <= 1) || (NilaiSuhu >= Setpoint);
  while (!targetReached){
      
      // NilaiSuhu = thermocouple.readCelsius();
      Serial.println("sedang memanaskan"); //nanti dihapus
        //updateTemperature(); //nanti dihapus
      NilaiSuhu = Setpoint; // nanti dihapus
      difference = NilaiSuhu - Setpoint;
      targetReached = (abs(difference) <= 1) || (NilaiSuhu >= Setpoint);
      message = "temp_rinse," + String(NilaiSuhu); //0 untuk pouring 1 untuk interval
      sendData(message.c_str());
  }

  if (abs(NilaiSuhu - Setpoint) <= 0.1 || (NilaiSuhu > Setpoint)) {
    dimmer.setPower(0);
    digitalWrite(Enable, LOW);
    Serial.println("air sudah panas rinse"); //nanti dihapus
    //delay(1000);
  }
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
      difference = NilaiSuhu - Setpoint;
      targetReached = (abs(difference) <= 1) || (NilaiSuhu >= Setpoint);
      message = "temp_pour," + String(NilaiSuhu); //0 untuk pouring 1 untuk interval
      sendData(message.c_str());
  }

  if (abs(NilaiSuhu - Setpoint) <= 0.1 || (NilaiSuhu > Setpoint)) {
    dimmer.setPower(0);
    digitalWrite(Enable, LOW);
    Serial.println("air sudah panas"); //nanti dihapus
    //delay(1000);
  }
}

void rinse(int volwater, int pourDuration, int pourInterval) {
  Serial.println("hanya pour"); //nanti dihapus
  //delay(1000);
  updateLoadCell();
  //delay(1000);
  int previousStartTime = millis();
  int currentStartTime = millis();
  int previousStartSend = millis();
  controlMotor(volwater, pourDuration, motor2Pin);

while ((average < volwater) || (currentStartTime - previousStartTime) < (pourDuration * 1000)) {
      currentStartTime = millis();

    //CEK APABILA POWER MOTOR TIDAK MENCAPAI MINIMAL
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motor2Pin);
    }
    else {
      if ((millis() - stepStartTime) > (pourIntervalLess * 1000)) { //Dibuat agar saat power motor tidak mencapai batas minimum, maka pour duration akan menurun dan kecepatan meningkat untuk mencapai volume yang tetap sama. Namun, saat sisa waktu (pourDuration - pourIntervalLess) , motor akan diam saja. 
        controlMotor3(0,motor2Pin); //nanti ganti
      }
    }
    // updateLoadCell(); //nanti dihapus
    // updateTemperature(); //nanti dihapus
    average = volwater; //nanti dihapus
    NilaiSuhu = Setpoint; // nanti dihapus
    Serial.print("Poured : ");
    Serial.print(average, 1);
    Serial.println("mL");
    //PENGIRIMAN DATA SELAMA PROSES

    // Serial.print("waktu berjalan: ");
    // Serial.println(currentStartTime - previousStartTime);
    // Serial.print("waktu tersisa: ");
    // Serial.println( (pourDuration * 1000) - (currentStartTime - previousStartTime) );
    // Serial.print("average: ");
    // Serial.println(average);
    // int remainingTime = ((pourDuration * 1000) - (currentStartTime - previousStartTime));
    // Serial.print("remaining time: ");
    // Serial.println(remainingTime);
    // message = "process," + String(countStep + 1) + ",0," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval

    // if ((currentStartTime - previousStartSend) >= 500) {
    // previousStartSend = currentStartTime;
    // sendData(message.c_str());
    // sendDataD(message);
    // }
    
    // updateLoadCell();// nanti dihapus
    controlMotor3(80, motor3Pin);
}

  //nanti dihapus
  // updateLoadCell(); //nanti dihapus
  controlMotor3(0, motor2Pin); // PWM 0(off) atau 255(on)
  controlMotor3(0, motor3Pin); // PWM 0(off) atau 255(on)
  Serial.println("sedang mematikan motor"); //nanti dihapus
  Serial.print("udah berhenti");
  Serial.println('\n');
  // updateLoadCell();//nanti dihapus
  Serial.printf("total water weight: %.2f", average);
  Serial.println('\n');

  //INTERVAL
  unsigned long previousWaitInterval = 0;  // Should be declared at global scope
  pourInterval = pourInterval * 1000;  // Convert to milliseconds
  unsigned long currentTimeInterval = millis();
  unsigned long previousWaitSend = currentTimeInterval;
  Serial.println("wait for pour interval");
  previousWaitInterval = currentTimeInterval;  // Update the timer

  while ((currentTimeInterval - previousWaitInterval) >= (pourInterval*1000)) {
    //PENGIRIMAN DATA SELAMA PROSES
    // updateLoadCell();//nanti dihapus
    // updateTemperature(); //nanti dihapus
    currentTimeInterval = millis();
    int remainingTime = (((pourInterval*1000) - currentTimeInterval)/1000);
    message = "process," + String(countStep + 1) + ",1," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval
    
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

//POUR

void pouring(int volwater, int pourDuration, int pourInterval) {
  Serial.println("hanya pour"); //nanti dihapus
  //delay(1000);
  updateLoadCell();
  //delay(1000);
  int previousStartTime = millis();
  int currentStartTime = millis();
  int previousStartSend = millis();
  controlMotor(volwater, pourDuration, motor2Pin);

while ((average < volwater) || (currentStartTime - previousStartTime) < (pourDuration * 1000)) {
      currentStartTime = millis();

    //CEK APABILA POWER MOTOR TIDAK MENCAPAI MINIMAL
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motor2Pin);
    }
    else {
      if ((millis() - stepStartTime) > (pourIntervalLess * 1000)) { //Dibuat agar saat power motor tidak mencapai batas minimum, maka pour duration akan menurun dan kecepatan meningkat untuk mencapai volume yang tetap sama. Namun, saat sisa waktu (pourDuration - pourIntervalLess) , motor akan diam saja. 
        controlMotor3(0,motor2Pin); //nanti ganti
      }
    }
    // updateLoadCell(); //nanti dihapus
    // updateTemperature(); //nanti dihapus
    average = volwater; //nanti dihapus
    NilaiSuhu = Setpoint; // nanti dihapus
    Serial.print("Poured : ");
    Serial.print(average, 1);
    Serial.println("mL");
    //PENGIRIMAN DATA SELAMA PROSES

    Serial.print("waktu berjalan: ");
    Serial.println(currentStartTime - previousStartTime);
    Serial.print("waktu tersisa: ");
    Serial.println( (pourDuration * 1000) - (currentStartTime - previousStartTime) );
    Serial.print("average: ");
    Serial.println(average);
    float remainingTime_f = ((pourDuration * 1000) - (currentStartTime - previousStartTime))/1000;
    int remainingTime = round(remainingTime_f);
    Serial.print("remaining time: ");
    Serial.println(remainingTime);
    message = "process," + String(countStep + 1) + ",0," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval

    if ((currentStartTime - previousStartSend) >= 500) {
    previousStartSend = currentStartTime;
    sendData(message.c_str());
    sendDataD(message);
    }
    
    // updateLoadCell(); //nanti dihapus
    controlMotor3(80, motor3Pin);
}

  //nanti dihapus
  // updateLoadCell(); //nanti dihapus
  controlMotor3(0, motor2Pin); // PWM 0(off) atau 255(on)
  controlMotor3(0, motor3Pin); // PWM 0(off) atau 255(on)
  Serial.println("sedang mematikan motor"); //nanti dihapus
  Serial.print("udah berhenti");
  Serial.println('\n');
  Serial.printf("total water weight: %.2f", average);
  Serial.println('\n');

  //INTERVAL
  unsigned long previousWaitInterval = 0;  // Should be declared at global scope
  pourInterval = pourInterval * 1000;  // Convert to milliseconds
  unsigned long currentTimeInterval = millis();
  unsigned long previousWaitSend = currentTimeInterval;
  Serial.println("wait for pour interval");
  previousWaitInterval = currentTimeInterval;  // Update the timer

  while ((currentTimeInterval - previousWaitInterval) >= (pourInterval*1000)) {
    //PENGIRIMAN DATA SELAMA PROSES
    // updateLoadCell();//nanti dihapus
    // updateTemperature(); //nanti dihapus
    currentTimeInterval = millis();
    int remainingTime = (((pourInterval*1000) - currentTimeInterval)/1000);
    message = "process," + String(countStep + 1) + ",1," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval
    
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

//POUR AND HEAT

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
  Serial.println("akan kontrol motor");
  controlMotor(volwater, pourDuration, motor2Pin);
  Serial.println("telah kontrol motor");
  
while ((average < volwater) || (currentStartTime - previousStartTime) < (pourDuration * 1000)) {
      currentStartTime = millis();

    //CEK APABILA POWER MOTOR TIDAK MENCAPAI MINIMAL
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motor2Pin);
    }
    else {
      if ((millis() - stepStartTime) > (pourIntervalLess * 1000)) { //Dibuat agar saat power motor tidak mencapai batas minimum, maka pour duration akan menurun dan kecepatan meningkat untuk mencapai volume yang tetap sama. Namun, saat sisa waktu (pourDuration - pourIntervalLess) , motor akan diam saja. 
        controlMotor3(0,motor2Pin); //nanti ganti
      }
    }

    average = volwater; //nanti dihapus
    NilaiSuhu = Setpoint; // nanti dihapus
    Serial.print("Poured : ");
    Serial.print(average, 1);
    Serial.println("mL");
    //PENGIRIMAN DATA SELAMA PROSES
    //updateLoadCell(); nanti dihapus
    //updateTemperature(); nanti dihapus
    Serial.print("waktu berjalan: ");
    Serial.println(currentStartTime - previousStartTime);
    Serial.print("waktu tersisa: ");
    Serial.println( (pourDuration * 1000) - (currentStartTime - previousStartTime) );
    Serial.print("average: ");
    Serial.println(average);
    float remainingTime_f = ((pourDuration * 1000) - (currentStartTime - previousStartTime))/1000;
    int remainingTime = round(remainingTime_f);
    Serial.print("remaining time: ");
    Serial.println(remainingTime);
    message = "process," + String(countStep + 1) + ",0," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval

    if ((currentStartTime - previousStartSend) >= 500) {
    previousStartSend = currentStartTime;
    sendData(message.c_str());
    sendDataD(message);
    }
    
    // updateLoadCell(); //nanti dihapus
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

  Serial.println("wait for pour interval");
  controlMotor((200 - volwater), pourInterval, motorPin); // Menyalakan motor 1 untuk mengisi hingga 200mL
  while ((currentTimeInterval - previousWaitInterval) >= (pourInterval*1000)) {
    if (!lessSpeed){
      controlMotor(volwater, pourDuration, motorPin);
    }
    else {
      if ((millis() - stepStartTime) >= (pourIntervalLess * 1000)) {
        controlMotor3(0,motorPin); //nanti ganti
      }
    }
    // updateLoadCell(); //nanti dihapus
    // updateTemperature(); //nanti dihapus
    average = volwater; //nanti dihapus
    NilaiSuhu = Setpoint; // nanti dihapus
    currentTimeInterval = millis();
    //cek waktu
    unsigned long currentStartTime_interval = millis();
    unsigned long previousStartTime_interval = millis();
    Serial.print("waktu berjalan: ");
    Serial.println(currentStartTime_interval - previousStartTime_interval);
    Serial.print("waktu tersisa: ");
    Serial.println( (pourDuration * 1000) - (currentStartTime_interval - previousStartTime_interval) );
    Serial.print("average: ");
    Serial.println(average);
    int remainingTime = ((pourDuration * 1000) - (currentStartTime_interval - previousStartTime_interval));
    Serial.print("remaining time: ");
    Serial.println(remainingTime);
    remainingTime = (((pourInterval*1000) - (currentStartTime_interval - previousStartTime_interval)));
    message = "process," + String(countStep + 1 + 1) + ",1," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval
    
    //interval pengiriman data 500ms
    if ((currentStartTime_interval - previousStartTime_interval) >= 500) {
    previousStartTime_interval = currentStartTime_interval;
    sendData(message.c_str());
    sendDataD(message);
    }
  }
  controlMotor3(0, motorPin);
  Serial.println("pour interval done");
}

// void rinse(int volwater, int pourDuration, int pourInterval) {
//   Serial.println("hanya rinse"); //nanti dihapus
//   //delay(1000);
//   updateLoadCell();
//   Serial.println("mulai pouring"); //nanti dihapus
//   //delay(1000);
//   int previousStartTime = millis();
//   controlMotor(volwater, pourDuration, motor2Pin);

// while ((average < volwater) || (millis() - previousStartTime) < (pourDuration * 1000)) {
//     average = volwater; // nanti dihapus
//     //CEK APABILA POWER MOTOR TIDAK MENCAPAI MINIMAL
//     if (!lessSpeed){
//       controlMotor(volwater, pourDuration, motor2Pin);
//     }
//     else {
//       if ((millis() - stepStartTime) > (pourIntervalLess * 1000)) { //Dibuat agar saat power motor tidak mencapai batas minimum, maka pour duration akan menurun dan kecepatan meningkat untuk mencapai volume yang tetap sama. Namun, saat sisa waktu (pourDuration - pourIntervalLess) , motor akan diam saja. 
//         controlMotor3(0,motor2Pin); //nanti ganti
//       }
//     }
//         average = volwater; //nanti dihapus
//     NilaiSuhu = Setpoint; // nanti dihapus
//     unsigned long currentTimeInterval = millis();
//     //cek waktu
//     unsigned long currentStartTime_interval = millis();
//     unsigned long previousStartTime_interval = millis();
//     Serial.print("waktu berjalan: ");
//     Serial.println(currentStartTime_interval - previousStartTime_interval);
//     Serial.print("waktu tersisa: ");
//     Serial.println( (pourDuration * 1000) - (currentStartTime_interval - previousStartTime_interval) );
//     Serial.print("average: ");
//     Serial.println(average);
//     int remainingTime = ((pourDuration * 1000) - (currentStartTime_interval - previousStartTime_interval));
//     Serial.print("remaining time: ");
//     Serial.println(remainingTime);
//     remainingTime = (((pourInterval*1000) - (currentStartTime_interval - previousStartTime_interval)));
//     message = "process," + String(countStep + 1 + 1) + ",1," + String(NilaiSuhu) + "," +String(remainingTime) + "," + String(average); //0 untuk pouring 1 untuk interval
    
//     //interval pengiriman data 500ms
//     if ((currentStartTime_interval - previousStartTime_interval) >= 500) {
//     previousStartTime_interval = currentStartTime_interval;
//     sendData(message.c_str());
//     sendDataD(message);
//     }
// }

//   updateLoadCell();
//   controlMotor3(0, motor2Pin); // PWM 0(off) atau 255(on)
//   controlMotor3(0, motor3Pin); // PWM 0(off) atau 255(on)
//   Serial.println("sedang mematikan motor"); //nanti dihapus
//   Serial.print("udah berhenti");
//   Serial.println('\n');
//   updateLoadCell();
//   Serial.printf("total water weight: %.2f", average);
//   Serial.println('\n');
//   unsigned long previousWaitInterval = 0;  // Should be declared at global scope
//   pourInterval = pourInterval * 1000;  // Convert to milliseconds
//   unsigned long currentTimeInterval = millis();
//   Serial.println("wait for pour interval");
//   while ((currentTimeInterval - previousWaitInterval) >= pourInterval) {
//     previousWaitInterval = currentTimeInterval;  // Update the timer
//   }
//   controlMotor3(0, motorPin);
//   Serial.println("pour interval done");
// }

#endif