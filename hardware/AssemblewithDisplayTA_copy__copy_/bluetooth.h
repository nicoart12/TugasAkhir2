//#include "var.h"
#ifndef BLUETOOTH.H
#define BLUETOOTH.H


void receiveData(BLECharacteristic *pCharacteristic) {
    Serial.println("data diterima");
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        dataInBLE = rxValue.c_str();
        Serial.println(dataInBLE);
        Serial.println("*********");

        dataReceived = false;  // Reset flag setelah data diproses
    }
        else {
      Serial.println("kosong");
    }
}

void sendData(const char* message) {
    if (deviceConnected) {
        pTxCharacteristic->setValue(message);
        pTxCharacteristic->notify();
        Serial.print("Send Value Bluetooth: ");
        Serial.println(message);
    }
    else {
      Serial.println("Tidak terhubung");
      delay(1000);
    }
}

#endif

