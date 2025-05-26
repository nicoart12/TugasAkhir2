#ifndef PARSING_H
#define PARSING_H

#include "var.h"
#include "bluetooth.h"

void parsingRecipe(String dtClean[], int c) {
  String message_error;
    for (int k = 1; k <= c - 1; k++) { //mulai dari 1 karena 0 sebagai header
        //NAMA RESEP
        if (k == 1) {
            namaResep = dtClean[1];
            if (namaResep.isEmpty()){
              errorNameFlag = true;
              Serial.print("NAMA SALAH ");
            }
            else {
              errorNameFlag = false;
            }
        } 
        //SETPOINT
        else if (k == 2) {
            Setpoint = dtClean[2].toDouble();
            
            if ((Setpoint > 95) || (Setpoint < 80)) {
              errorSetpointFlag = true;
              Serial.print("Setpoint SALAH ");
            }
            else {
              errorSetpointFlag = false;
            }
            Serial.print("Setpoint: ");
            Serial.println(Setpoint);
        } 
        //VOLUME
        else if ((k == 3) || (k == 6) || (k == 9) || (k == 12) || (k == 15)) {
            pouringVolumes[countwater] = dtClean[k].toInt();
            Serial.print("Volume : ");
            Serial.println(dtClean[k].toInt());
            if ((pouringVolumes[countwater]<30) ||  (pouringVolumes[countwater]>200)) {
                errorVolumeFlag = true;
                Serial.print("Volume SALAH ");
            }
            else {
              errorVolumeFlag = false;
            }
            totalVolume += pouringVolumes[countwater];
            countwater++;
        } else if ((k == 4) || (k == 7) || (k == 10) || (k == 13) || (k == 16)) {
            pouringDurations[countbrew] = dtClean[k].toInt();
            Serial.print("Duration: ");
            Serial.println(dtClean[k].toInt());
                        if ((pouringDurations[countbrew]<1) ||  (pouringDurations[countbrew]>60)){
                errorPouringFlag = true;
                Serial.print("Pouring SALAH ");
            }
            else{
              errorPouringFlag = false;
            }
            total_time += pouringDurations[countbrew];
            countbrew++;
        } else if ((k == 5) || (k == 8) || (k == 11) || (k == 14) || (k == 17)) {
            pouringIntervals[countwait] = dtClean[k].toInt();
            Serial.print("Interval: ");
            Serial.println(dtClean[k].toInt());
              if ((pouringIntervals[countwait] < 1) ||  (pouringIntervals[countwait]>60)){
              errorIntervalFlag = true;
              Serial.print("Interval SALAH ");
        }
            else {
              errorIntervalFlag = false;
            }
            total_time += pouringIntervals[countwait];
            countwait++;

        }
    }
    countwait = countwait - 1; //mengurangi satu karena diujung ada penambahan. 
    countbrew = countbrew - 1;
    countwater = countwater - 1;

    if (errorNameFlag || errorSetpointFlag || errorVolumeFlag || errorPouringFlag || errorIntervalFlag){ //jika satu saja true maka ada kesalahan
      message_error = "recipe,0"; //0 untuk salah 1 untuk benar
        if (errorNameFlag) {
          message_error += ",errorName";
        }
                if (errorSetpointFlag) {
          message_error += ",errorSetpoint";
        }
                if (errorVolumeFlag) {
          message_error += ",errorVolume";
        }
                if (errorPouringFlag) {
          message_error += ",errorPouring";
        }
                if (errorIntervalFlag) {
          message_error += ",errorInterval";
        }
    }
    else {
            message_error = "recipe,1"; //0 untuk salah 1 untuk benar
            parsingFlag = true;
            Serial.print("parsingFlag: ");
            Serial.println(parsingFlag);
    }
    sendData(message_error.c_str());
    sendDataD(message_error);
}

void parsingData(String dataIn) {
    int j = 0;
    String dt[100];
    String dtClean[100]; // Adjust size as needed
    dt[j] = "";
    Serial.println("Mulai parsing bang");
    Serial.println("*********");
    Serial.print("dataIn: ");
    Serial.println(dataIn);
    Serial.println("*********");

    for (int i = 0; i < dataIn.length(); i++) {
        if ((dataIn[i] == '#') || (dataIn[i] == ',')) {
            if (dt[j] != "0" && dt[j] != "") {
                j++;
                dt[j] = "";
            }
        } else {
            dt[j] = dt[j] + dataIn[i];
        }
    }

    int c = 0; //counter utk tau panjang array data bersih
    for (int k = 0; k <= j; k++) {
        if (dt[k] != "0" && dt[k] != "" && dt[k] != "00") {
            dtClean[k] = dt[k];
            c++;
        }
    }
    //PENGIRIMAN RESEP
    if (dtClean[0] == "send") {
        parsingRecipe(dtClean, c);
    }
    //MEMULAI RINSE
    else if (dtClean[0] == "srinse") {
      srinseFlag = true;
    }
    //MEMULAI TARE
    else if (dtClean[0] == "tare") {
      tareFlag = true;
    }
}
#endif