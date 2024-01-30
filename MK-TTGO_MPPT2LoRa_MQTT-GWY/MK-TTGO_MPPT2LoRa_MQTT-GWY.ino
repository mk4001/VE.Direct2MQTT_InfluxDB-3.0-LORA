/*
  © MK 18 Jan 2024
  LoRaReceiver MQTT Gateway
  based on ESP32-TTGO sending packets on 868 MHz
*/

#include <SPI.h>
#include <LoRa.h>     // https://github.com/sandeepmistry/arduino-LoRa
#include <U8g2lib.h>  // https://github.com/olikraus/U8g2_Arduino
// #include <U8x8lib.h>

/* WiFi parameters and credentials */
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>
const char* mqtt_server = "mqtt.aaaaa.org"; /your MQTT broker

WiFiClient espClient;
PubSubClient client(espClient);

String clientId = "VE.Direct";
String Rssi = "";
String topic = "";
String StringOne;
String msgOne = "";

#define OFF 0  // For LED
#define ON 1

// SPI LoRa Radio
#define LORA_SCK 5    // GPIO5 - SX1276 SCK
#define LORA_MISO 19  // GPIO19 - SX1276 MISO
#define LORA_MOSI 27  // GPIO27 - SX1276 MOSI
#define LORA_CS 18    // GPIO18 - SX1276 CS
#define LORA_RST 14   // GPIO14 - SX1276 RST
#define LORA_IRQ 26   // GPIO26 - SX1276 IRQ (interrupt request)

// I2C OLED Display works with SSD1306 driver
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

// UNCOMMENT one of the constructor lines below
//U8X8_SSD1306_128X64_NONAME_SW_I2C Display(/* clock=*/ OLED_SCL, /* data=*/ OLED_SDA, /* reset=*/ OLED_RST); // Unbuffered, basic graphics, software I2C
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C Display(U8G2_R0, /* clock=*/ OLED_SCL, /* data=*/ OLED_SDA, /* reset=*/ OLED_RST); // Page buffer, SW I2C
U8G2_SSD1306_128X64_NONAME_F_SW_I2C Display(U8G2_R0, /* clock=*/OLED_SCL, /* data=*/OLED_SDA, /* reset=*/OLED_RST);  // Full framebuffer, SW I2C

const int blueLED = LED_BUILTIN;

String rssi = "";
String packet = "";

void setup() {
  Serial.begin(115200);

  Serial.println("LoRa Receiver & GWY");

  pinMode(blueLED, OUTPUT);  // For LED feedback

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP", "password");  // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();

  Serial.println("Program Start");

  Display.begin();
  Display.enableUTF8Print();  // enable UTF8 support for the Arduino print() function
  Display.setFont(u8g2_font_ncenB10_tr);

  // Very important for SPI pin configuration!
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

  // Very important for LoRa Radio pin configuration!
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);


  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  // The larger the spreading factor the greater the range but slower data rate
  // Send and receive radios need to be set the same
  LoRa.setSpreadingFactor(7);  // ranges from 6-12, default 7 see API docs

  // Change the transmit power of the radio
  // Default is LoRa.setTxPower(17, PA_OUTPUT_PA_BOOST_PIN);
  // Most modules have the PA output pin connected to PA_BOOST, gain 2-17
  // TTGO and some modules are connected to RFO_HF, gain 0-14
  // If your receiver RSSI is very weak and little affected by a better antenna, change this!
  LoRa.setTxPower(14, PA_OUTPUT_RFO_PIN);
  LoRa.enableCrc();


  // Display Info
  Display.clearBuffer();
  Display.setCursor(0, 12);
  Display.print("LoRa UP");
  Display.setCursor(0, 26);
  Display.print("WiFi UP");
  Display.setCursor(0, 42);
  Display.print("MQTT UP");
  Display.setCursor(0, 58);
  Display.print("Waiting for LoRa");
  Display.sendBuffer();

  delay(10000);
  Display.clearDisplay();

  digitalWrite(blueLED, ON);  // Turn blue LED on
  LoRa.beginPacket();
  LoRa.print("ACK");
  LoRa.endPacket();
  digitalWrite(blueLED, OFF);  // Turn blue LED off
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long prev_millis;

  // try to parse packet
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    // received a packet

    digitalWrite(blueLED, ON);  // Turn blue LED on

    Serial.print("Received packet {");

    // read LoRa packet
    packet = "";  // Clear packet
    while (LoRa.available()) {
      packet += (char)LoRa.read();  // Assemble new packet
    }

    packet.trim();

    parseSerialData(packet);

    rssi = LoRa.packetRssi();
    Serial.println("GWY LoRa Signal Strength (dB):  " + rssi);

    topic = "VE.Direct/GWY-LoRa-RSSI";
    client.publish(topic.c_str(), rssi.c_str());


    //topic = "ESP32-LoRa-GWY/string";
    //client.publish(topic.c_str(), packet.c_str());


    WlanPower(clientId);

    LoRa.beginPacket();
    LoRa.print("ACK");
    LoRa.endPacket();
    prev_millis = millis();

    digitalWrite(blueLED, OFF);  // Turn blue LED off
  }

  else if (!packetSize && (millis() - prev_millis > 5000)) {
    LoRa.beginPacket();
    LoRa.print("ACK - SEND IT AGAIN");
    LoRa.endPacket();
    Serial.println("ACK - SEND IT AGAIN");
    prev_millis = millis();
  }
}
