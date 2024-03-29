/*
  © MK 18 Jan 2024
  LoRaReceiver MQTT Gateway
  based on ESP32-TTGO sending packets on 868 MHz
*/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  payload[length] = '\0';

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msgOne = String((char*)payload);
  }
  Serial.println();

  StringOne = String(topic);

  if (StringOne == "ESP3201/identify") {
    Serial.print(msgOne);
    //timeout = msgOne.toFloat();
    //Serial.println(timeout);
  }
}
