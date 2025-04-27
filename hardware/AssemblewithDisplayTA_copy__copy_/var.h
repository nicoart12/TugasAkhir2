#ifndef VAR_H
#define VAR_H

#include <HardwareSerial.h>
#include <RBDdimmer.h>
#include "max6675.h"
#include <PID_v1.h>
#include <HX711_ADC.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <iostream>
#include <cmath>

// Deklarasi variabel sebagai extern
extern BLEServer *pServer;
extern BLECharacteristic *pTxCharacteristic;
extern BLECharacteristic *pRxCharacteristic;
extern bool deviceConnected;
extern bool oldDeviceConnected;
extern bool dataReceived;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Sudah terhubung dengan bluetooth");
        deviceConnected = true;

        BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Terputus dengan bluetooth");
        deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        dataReceived = true;
    }
};

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

extern HardwareSerial SerialPort;

extern String receivedData;
extern const char *receivedDataconvert;

// Tambahkan definisi enum ProgramState
enum ProgramState {
  WAITING_FOR_INPUT,
  RINSE,
  POURING,
  COMPLETED
};

// Deklarasi variabel sebagai extern
extern ProgramState currentState;

extern String dataIn;
extern String dataInBLE;
extern String dataReceive;

extern bool dataSendBLE;
extern bool dataSendD;
extern bool inputFlag;
extern bool tareFlag;
extern bool srinseFlag;
extern bool frinseFlag;
extern bool receiveFlag; 

extern int totalVolume;
extern int numSteps;

extern int countwater;
extern int countbrew;
extern int countwait;

extern double Setpoint, NilaiSuhu, Output;

extern String namaResep; 
extern int pouringVolumes[10];
extern int pouringDurations[10];
extern int pouringIntervals[10];

extern bool parsing;
extern bool parsingFlag;

extern int TotalLoadCell;

extern float sum;
extern unsigned long t;
extern int motorSpeed;

extern int slave_1_id;

extern uint32_t lastCommand;

extern String dt[10];
extern int i;

extern float average;

extern int pourstep;
extern int stepStartTime;

extern int PreviousMillisSend;
extern unsigned long previousMillis;
extern const long interval;
extern unsigned long previousMillis_print;
extern int previousheatingmillis;
extern int currentheatingmillis;
extern int total_time;

extern double Kp, Ki, Kd;

extern dimmerLamp dimmer;

extern MAX6675 thermocouple;

extern PID myPID;

extern HX711_ADC LoadCell;

// Definisi pin
#define outputPin 27
#define zerocross 26
#define thermoCLK 5
#define thermoCS 23
#define thermoSO 19
#define HX711_dout 34
#define HX711_sck 14
#define tx_pin 15  
#define rx_pin 17
#define Enable 18
#define motorPin 16
#define motor2Pin 4
#define motor3Pin 25


#endif