/*
  © MK 18 Jan 2024
  LoRaReceiver MQTT Gateway
  based on ESP32-TTGO sending packets on 868 MHz
*/

void WlanPower (String topicx)
{
  Serial.print("WiFi Signal Strength (dB): ");
  long rssi = WiFi.RSSI();
  Rssi = String(rssi);
  topicx += "/WiFi-RSSI";
  Serial.println(Rssi);

  client.publish(topicx.c_str(), Rssi.c_str()); //value to MQTT Topic WiFi Signal

}
