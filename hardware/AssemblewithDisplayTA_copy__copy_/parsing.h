#ifndef PARSING_H
#define PARSING_H

#include "var.h"

void parsingRecipe(String dtClean[], int c);

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

    if (dtClean[0] == "send") {
        parsingRecipe(dtClean, c);
    } else if (dtClean[0] == "srinse") {
        if (dtClean[1] == "1") {
            srinseFlag = true;
        } else {
            Serial.print("TIDAK ADA RINSE, RINSE DULU YA");
        }
    } else if (dtClean[0] == "tare") {
        if (dtClean[1] == "1") {
            tareFlag = true;
        } else {
            Serial.print("TIDAK ADA TARE, BUANG DULU YA");
        }
    }
}

void parsingRecipe(String dtClean[], int c) {
    for (int k = 1; k <= c - 1; k++) { //mulai dari 1 karena 0 sebagai header
        if (k == 1) {
            namaResep = dtClean[0];
        } else if (k == 2) {
            Setpoint = dtClean[1].toDouble();
        } else if ((k == 3) || (k == 6) || (k == 9) || (k == 12) || (k == 15)) {
            pouringVolumes[countwater] = dtClean[k].toInt();
            totalVolume += pouringVolumes[countwater];
            countwater++;
        } else if ((k == 4) || (k == 7) || (k == 10) || (k == 13) || (k == 16)) {
            pouringDurations[countbrew] = dtClean[k].toInt();
            total_time += pouringDurations[countbrew];
            countbrew++;
        } else if ((k == 5) || (k == 8) || (k == 11) || (k == 14) || (k == 17)) {
            pouringIntervals[countwait] = dtClean[k].toInt();
            total_time += pouringIntervals[countwait];
            countwait++;
            parsingFlag = true;
            Serial.print("parsingFlag: ");
            Serial.println(parsingFlag);
        }
    }
    countwait = countwait - 1;
}

#endif