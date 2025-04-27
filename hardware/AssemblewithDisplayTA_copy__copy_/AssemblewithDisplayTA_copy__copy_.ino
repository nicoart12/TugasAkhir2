#include "input.h"
#include "var.h"
#include "heating.h"
// #include "display.h"
// #include "bluetooth"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

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

void setup()
{
  Serial.begin(115200);
    BLEDevice::init("BLE_ESP32_01");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pTxCharacteristic = pService->createCharacteristic(
                            CHARACTERISTIC_UUID_TX,
                            BLECharacteristic::PROPERTY_NOTIFY
                        );
    pTxCharacteristic->addDescriptor(new BLE2902());

    pRxCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID_RX,
                                        BLECharacteristic::PROPERTY_WRITE
                                    );
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");

  // Display communication
  SerialPort.begin(9600, SERIAL_8N1, rx_pin, tx_pin); 
  SerialPort.setTimeout(250);
  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  dimmer.begin(NORMAL_MODE, ON);
  dimmer.setPower(0);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 50);
  LoadCell.begin();
  float calibrationValue = 422.29;
  unsigned long stabilizingtime = 2000;
  bool _tare = true;
  LoadCell.start(stabilizingtime, _tare);

  if (LoadCell.getTareTimeoutFlag())
  {
    //Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1)
      ;
  }
  else
  {
      LoadCell.setCalFactor(calibrationValue);
      Serial.println("Loadcell working");
  }
    Serial.println("Startup is complete");
    pinMode(motorPin, OUTPUT);
  Serial.println("Mulai Proses");
  //currentState = WAITING_FOR_INPUT;
}

void loop()
{
  unsigned long currentMillis = millis();//unsigned long currentMillis = millis();
    //   if (!deviceConnected && oldDeviceConnected) { 2 1 3 3 1 1 3 3
    //     delay(500);
    //     pServer->startAdvertising();
    //     Serial.println("start advertising");
    //     oldDeviceConnected = deviceConnected;
    // }

    // if (deviceConnected && !oldDeviceConnected) {
    //     oldDeviceConnected = deviceConnected;
    // }
  switch (currentState)
  {
  case WAITING_FOR_INPUT:
  {
    if (deviceConnected) {
      sendDataD("connect,1");
    }
    else {
      Serial.println("Tidak konek");
    }
    askForInputs();
    dimmer.setPower(0);
    break;
  }

  case RINSE: //asumsi rinse itu 70 C dan 50 mL
  {
    srinseFlag = false;
    //frinseFlag = false; KAYANYA GAPERLU DEH
    askForInputs2();
        unsigned long startTimeMotor = millis();  // Catat waktu mulai
    while (millis() - startTimeMotor < 10000) { // waktu 10s 
        controlMotor3(220, motorPin);  // Jalankan motor
    }
    controlMotor3(0, motorPin); // matikan motor
    //controlMotor3(220, motorPin); //PIKIRKAN BERAPA DURASI UNTUK MENYALAKAN MOTOR 1
    if (inputFlag) {
          if (srinseFlag){
            pouringandHeat(100, 20, 3, 70); // Volume rinse 100mL , selama 20 detik, dan interval 3 detik, dilakukan dalam suhu 70 derajat C
            sendDataD("frinse");
            sendData("frinse");
            //digitalWrite(Enable, LOW);

          askForInputs2();
          if (inputFlag){
            if (tareFlag) {
              LoadCell.start(1000, true);
              // TARE UNTUK LOADCELL
              currentState = POURING;
      }
      else {
        Serial.println("MULAI TARE DULU");
      }
    }
    }
    }
 else {
      Serial.println("MULAI RINSE DULU");
    }

  }
  case POURING:
  {
    Serial.println("Mulai pouring");
    controlMotor3(220, motorPin); // PWM 0 - 255
    if ((totalVolume >= 250)) {
      heating(Setpoint);
      for (int i = 0; i <= countwait ; i++) {
        pouringandHeat(pouringVolumes[i],pouringDurations[i],pouringIntervals[i], Setpoint);
      }
      currentState = COMPLETED;
    }
    else {
      heating(Setpoint);
      for (int i = 0; i <= countwait ; i++) {
        pouring(pouringVolumes[i], pouringDurations[i],pouringIntervals[i]);
      }
      currentState = COMPLETED;
    }
    break;
  }
  case COMPLETED:
  {
    Serial.println("sudah selesai"); //nanti dihapus
    delay(1000);
    dimmer.setPower(0);
    Serial.println("PROCESS COMPLETED");
    Serial.printf("Final temperature: %.2f", NilaiSuhu);
    Serial.println('\n');
    currentState = WAITING_FOR_INPUT;
    break;
  }
  }
}
