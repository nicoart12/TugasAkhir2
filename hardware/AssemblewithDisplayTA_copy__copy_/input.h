#ifndef INPUT_H
#define INPUT_H

#include "var.h"
#include "display.h"
#include "parsing.h"
#include "bluetooth.h"


bool isEmpty(String str) {
    return str.length() <= 5; // Mengembalikan true jika panjang string adalah 0
}

void askForInputs() {
  Serial.println("memulai minta input"); ///nanti dihapus
  delay(1000);
  totalVolume = 0;
  digitalWrite(Enable, LOW);
  //Serial.println("Enter desired temperature setpoint: ");
  if (dataReceived) {
        receiveData(pRxCharacteristic);
        Serial.print("dataIn Input: ");
        Serial.println(dataInBLE);
    parsing = true; //flag untuk memulai dan berhenti parsing
    bool gaada = isEmpty(dataInBLE);
    if ((parsing == true) && (!gaada))
    {
      parsingData(dataInBLE);
      Serial.println("input sudah masuk"); //nanti dihapus
      delay(1000);
      parsing=false;
      if (parsingFlag) {
        currentState = RINSE;
      }
      else {
        Serial.println("parsing gagal"); //nanti dihapus
      }
    }
    else {
      Serial.println("gaada data"); //nanti dihapus
    }
  }
    //Jika tidak menggunakan bluetooth
    else if (SerialPort.available()){ 
          receiveFlag = true;
          receiveDataD(receiveFlag);
          dataIn = dataReceive;
          parsing = true;
          bool gaada = isEmpty(dataIn);
          if (parsing == true && (not gaada))
          {
            parsingData(dataIn);
            Serial.println("input sudah masuk3"); //nanti dihapus
            delay(1000);
            parsing=false;
            if (parsingFlag) {
              currentState = RINSE;
            }
            else {
              Serial.println("parsing gagal"); //nanti dihapus
            }
          }
    } 
    else {
       Serial.println("gaada yang masuk"); //nanti dihapus
      // delay(1000);
       currentState = WAITING_FOR_INPUT;
    }
    dimmer.setPower(0);
  }

//Minta input selain data resep, dipisahkan karena awal dan akhirnya berbeda
void askForInputs2() {
  Serial.println("memulai minta input selain resep"); ///nanti dihapus
  inputFlag = false;
  delay(1000);
  digitalWrite(Enable, LOW);
  while (!inputFlag){
  if (dataReceived) {
        receiveData(pRxCharacteristic);
        Serial.println("*********");
        Serial.print("dataIn Input: ");
        Serial.println(dataInBLE);
        Serial.println("*********");
    parsing = true;
    bool gaada = isEmpty(dataInBLE);
    if (parsing == true && (not gaada))
    {
      parsingData(dataInBLE);
      Serial.println("input sudah masuk, selain resep bluetooth"); //nanti dihapus
      delay(1000);
      parsing=false;
      if (parsingFlag) {
        //currentState = POURING;
        Serial.println("parsing berhasil bluetooth");
        inputFlag = true;
      }
      else {
        Serial.println("parsing gagal");
        inputFlag = false;
      }
    }
    else {
      inputFlag = false;
      Serial.println("gaada data"); //nanti dihapus
    }
  }
    //Jika tidak menggunakan bluetooth
    else if (SerialPort.available()){ 
          receiveFlag = true;
          receiveDataD(receiveFlag);
          dataIn = dataReceive;
          parsing = true;
          bool gaada = isEmpty(dataIn);
          if (parsing == true && (not gaada))
          {
            parsingData(dataIn);
            Serial.println("input sudah masuk, selain resep display"); //nanti dihapus
            //delay(1000);
            parsing=false;
            if (parsingFlag) {
              Serial.println("parsing berhasil display");
              inputFlag = true;
            }
            else {
              Serial.println("parsing gagal"); //nanti dihapus
              inputFlag = false;
            }
          }
    } 
    else {
       Serial.println("gaada yang masuk"); //nanti dihapus
       delay(1000);
       inputFlag = false;
    }
  }
  }
#endif