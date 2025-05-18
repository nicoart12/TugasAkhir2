#ifndef PARSING_H
#define PARSING_H

#include "var.h"

void parsingRecipe(String dtClean[], int c) {
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
            //if ((pouringVolumes[countwater]<30) nanti yaaaa
            totalVolume += pouringVolumes[countwater];
            countwater++;
        } else if ((k == 4) || (k == 7) || (k == 10) || (k == 13) || (k == 16)) {
            pouringDurations[countbrew] = dtClean[k].toInt();
            Serial.print("Duration: ");
            Serial.println(dtClean[k].toInt());
            total_time += pouringDurations[countbrew];
            countbrew++;
        } else if ((k == 5) || (k == 8) || (k == 11) || (k == 14) || (k == 17)) {
            pouringIntervals[countwait] = dtClean[k].toInt();
            Serial.print("Interval: ");
            Serial.println(dtClean[k].toInt());
            total_time += pouringIntervals[countwait];
            countwait++;
            parsingFlag = true;
            Serial.print("parsingFlag: ");
            Serial.println(parsingFlag);
        }
    }
    countwait = countwait - 1; //mengurangi satu karena diujung ada penambahan. 
    countbrew = countbrew - 1;
    countwater = countwater - 1;
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
      Serial.print(dtClean[0]);
      Serial.print(dtClean[1]);
        if (String(dtClean[1]) == String(1)) {
            srinseFlag = true;
        } else {
            Serial.print("TIDAK ADA RINSE, RINSE DULU YA");
        }
    }
    //MEMULAI TARE
    else if (dtClean[0] == "tare") {
      tareFlag = true;
        if (String(dtClean[1]) == String(1)) {
            tareFlag = true;
        } else {
            Serial.print("TIDAK ADA TARE, BUANG DULU YA");
        }
    }
}
#endif