#include "input.h"
#include "var.h"
#include "heating.h"
#include <iostream>
#include <sstream>
#include <vector>

//BUAT CARA UNTUK KIRIM DATA PENYEDUHAN PAKE HEADER YANG KEMAREN (udah)
//BUAT UNTUK MENYATAKAN PENYEDUHAN SELESAI (udah)
//BUAT MODE UNTUK MENYATAKAN PENGGUNA NGATUR PAKE APLIKASI ATAU KONTROLLER 
//BUAT ALGORITAM UNTUK DINGININ AIR LEWAT MOTOR 1 menyala (gausah, takutnya nanti air membludak)
//COBA BUAT SEMISALNYA MOTOR 1 PUNYA KECEPATAN MINIMAL 70 PWM, nah nanti selama interval disesuaikan aja =- waternya pake pwm 70, gausah maksain dari waktu interval (tambah kondisional di control motor) (sudah)

//nanti dihapus (cari ini dan hapus line)
// ganti fill water jadi 25s


//tugas
//buat pengiriman untuk saat interval juga 
//pastiin kalau time remaining itu bener
//buat flag untuk masing masibg parsing
using namespace std;

// yang ini
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
  LoadCell.begin();
  float calibrationValue = 363.53;
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
      sendDataD("connect,0");
      Serial.println("Tidak konek");
    }
    askForInputs();
    dimmer.setPower(0);
    break;
  }

  case RINSE: //asumsi rinse itu 70 C dan 50 mL
  {
    unsigned long startTimeMotor = millis();  // Catat waktu mulai
    // FILL WATER
    Serial.println("MEMULAI FILL WATER");
    while (millis() - startTimeMotor < 2000) { // waktu 2s 
        controlMotor3(100, motorPin);  // Jalankan motor
    }
    controlMotor3(0, motorPin); // matikan motor
    Serial.println("SELESAI FILL WATER");
    Serial.println("Memulai RINSE");
    srinseFlag = false;
    askForInputs2();
    if (srinseFlag){ 
        Serial.println("MULAI RINSE");
        heating(70);
        rinse(50, 5, 3); // Volume rinse 50mL , selama 5 detik, dan interval 3 detik, dilakukan dalam suhu 70 derajat C
        // rinse dibuat fungsi tersendiri karena akan ada clash saat pengiriman data, dalam fungsi pouring sudah ada pengiriman data, sedangkan rinse tidak akan megirim data kecuali sudah selesai
        Serial.println("Selesai RINSE");
        delay(200);
        sendDataD("frinse");
        sendData("frinse");
      //MEMINTA INPUT UNTUK RESEP
    askForInputs2();
        if (tareFlag) { 
          Serial.println("Sedang tare");
          LoadCell.start(1000, true);
          // TARE UNTUK LOADCELL
          currentState = POURING;
        }
        else {
          Serial.println("MULAI TARE DULU");
        }
      }
    else {
      Serial.println("MULAI RINSE DULU");
    }

  }
  case POURING:
  //misalnya haisl tampungan dikurang sama hasil yang dikeluarin dibawah 50, maka akan ada waktu tambahan untuk mengisi air.
  //case 1: setiap sebelum mulai pemanasan di step berikutnya, air harus penuh (pakai yang ini)
  //case 2: harus hitung di setiap kali mau pemanasan
  {
    Serial.println("Mulai pouring");
    blockvolume = 220;
    Serial.print("Total Volume : ");
    Serial.print(totalVolume);
    Serial.println(" mL");
    if (totalVolume >= blockvolume) {
      for (countStep; countStep <= countwait ; countStep++) {
        heating(Setpoint);
        pouringandHeat(pouringVolumes[countStep],pouringDurations[countStep],pouringIntervals[countStep]);
      }
      currentState = COMPLETED;
    }
    else {
      heating(Setpoint);  //SEKALI MEMANASKAN SAJA
      for (countStep; countStep <= countwait ; countStep++) {
        pouring(pouringVolumes[countStep], pouringDurations[countStep],pouringIntervals[countStep]);
      }
      currentState = COMPLETED;
    }
    break;
  }
  case COMPLETED:
  {
    Serial.println("sudah selesai"); //nanti dihapus
    sendData("finish");
    sendDataD("finish");
    delay(1000);
    dimmer.setPower(0);
    countStep = 0;
    Serial.println("PROCESS COMPLETED");
    Serial.printf("Final temperature: %.2f", NilaiSuhu);
    Serial.println('\n');
    currentState = WAITING_FOR_INPUT;
    break;
  }
  }
}
