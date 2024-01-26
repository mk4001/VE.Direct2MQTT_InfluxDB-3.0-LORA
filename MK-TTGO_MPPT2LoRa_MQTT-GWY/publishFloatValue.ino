/*
  © MK 18 Jan 2024
  LoRaReceiver MQTT Gateway
  based on ESP32-TTGO sending packets on 868 MHz
*/

void publishFloatValue(const char* topic, float value) {
  char message[20];
  snprintf(message, sizeof(message), "%.2f", value);
  client.publish(topic, message);
}
