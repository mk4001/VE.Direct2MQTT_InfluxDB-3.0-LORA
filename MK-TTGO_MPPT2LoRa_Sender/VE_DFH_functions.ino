void ReadVEData() {
  while (SERIAL_PORT.available()) {
    myve.rxData(SERIAL_PORT.read());
  }
  yield();
}

void EverySecond() {
  static unsigned long prev_millis;

  if (millis() - prev_millis > 5000) { //Every 5 secs
    PrintData();
    prev_millis = millis();
  }
}

void PrintData() {

  rec_count++;

  LoRa.beginPacket();

  for (int i = 0; i < myve.veEnd; i++) {
    digitalWrite(blueLED, ON);  // Turn blue LED on
    LoRa.print(myve.veName[i]);
    LoRa.print("\t");
    LoRa.print(myve.veValue[i]);
    LoRa.print("\n");
    digitalWrite(blueLED, OFF);  // Turn blue LED off
  }

  LoRa.endPacket();

  rssi = LoRa.packetRssi();
  Serial.println("Sender LoRa Signal Strength (dB):  " + rssi);

  Serial.println();
  Serial.print("[");
  Serial.print(rec_count);
  Serial.println("]");
}


// hex frame callback function
void HexCallback(const char* buffer, int size, void* data) {
  char tmp[100];
  memcpy(tmp, buffer, size * sizeof(char));
  tmp[size] = 0;
  Serial.print("received hex frame: ");
  Serial.println(tmp);
}

// log helper
void LogHelper(const char* module, const char* error) {
  Serial.print(module);
  Serial.print(":");
  Serial.println(error);
}