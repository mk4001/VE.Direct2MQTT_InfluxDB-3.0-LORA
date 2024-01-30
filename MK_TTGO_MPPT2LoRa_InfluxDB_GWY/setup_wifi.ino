/*
  © MK 18 Jan 2024
  LoRaReceiver InfluxDB Gateway
  based on ESP32-TTGO sending packets on 868 MHz
*/

void setup_wifi() {

  int cntx;

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    cntx++;
    if (cntx == 10) {
      Serial.println();
      Serial.println("**RESTART**");
      Serial.println();
      ESP.restart();
    }
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
