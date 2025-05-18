#ifndef DISPLAY_H
#define DISPLAY_H

void sendDataD(String dataSend) {
    digitalWrite(Enable, HIGH);
    SerialPort.print(1);
    Serial.println("******\n");
    Serial.print("Data send Display: ");
    Serial.println(dataSend);
    Serial.println("******\n");
    SerialPort.print(dataSend);
    SerialPort.flush();
    digitalWrite(Enable, LOW);
}

void receiveDataD(bool receiveFlag){
  if (receiveFlag) {
      dataReceive = SerialPort.readString();
      //digitalWrite(Enable, LOW);
      Serial.println("******\n");
      Serial.printf("Data received: %s\n", dataReceive );
      Serial.println("******\n");
      SerialPort.print(dataReceive);
      SerialPort.flush();
      digitalWrite(Enable, HIGH);
      //return dataReceive;
  }
  else {
    Serial.println("Data tidak diterima");
  }

}
#endif