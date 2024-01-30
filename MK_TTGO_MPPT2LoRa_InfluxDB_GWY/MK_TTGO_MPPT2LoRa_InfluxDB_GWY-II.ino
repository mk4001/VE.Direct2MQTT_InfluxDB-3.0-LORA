/*
  © MK 18 Jan 2024
  LoRaReceiver InfluxDB Gateway
  based on ESP32-TTGO sending packets on 868 MHz
*/

#include <SPI.h>
#include <LoRa.h>     // https://github.com/sandeepmistry/arduino-LoRa
#include <U8g2lib.h>  // https://github.com/olikraus/U8g2_Arduino


#include <WiFi.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

const char* ssid = "WLAN 7490-2.4GHz";
const char* password = "06431928919070006302";

// InfluxDB Cloud Configuration
const char* influxDBUrl = "https://eu-central-1-1.aws.cloud2.influxdata.com";
const char* influxDBToken = "vlYHoJH6PrcDepvl8MCt886dJhmIrfFwvS8I8M0d5OmAgxTIZEzA0wm1EQMrkkRZ9xpQZsV1nLdEVER0ilWHog==";
const char* influxDBOrg = "447ab0b6dc55f6f8";
const char* influxDBBucket = "Victron";

/*
// InfluxDB on Premise Configuration
const char* influxDBUrl = "http://rpi3bp:8086";
const char* influxDBToken = "kGVg7zzUkhQjoXBgSQ-tDXSVRl4j7XmRt16VVUsu_Imkd7lwjX2TMYycV3Pfa4FUZHR6Pmh46db2qXwwhJW-_A==";
const char* influxDBOrg = "ae1e888a12c8e19c";
const char* influxDBBucket = "Victron";
*/

// Time zone info
#define TZ_INFO "UTC2"

#define DEBUG //REMARK TO DEBUG-OFF

InfluxDBClient influxDB(influxDBUrl, influxDBOrg, influxDBBucket, influxDBToken, InfluxDbCloud2CACert);

// Declare Data point
Point point("SmartSolar");

String rssi;
String packet;
long rec_count;

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


void setup() {
  Serial.begin(115200);

  Serial.println("LoRa Receiver & GWY");

  pinMode(blueLED, OUTPUT);  // For LED feedback

  setup_wifi();

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

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");


  // Check server connection
  if (influxDB.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(influxDB.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxDB.getLastErrorMessage());
    Serial.println();
    Serial.println("**RESTART**");
    Serial.println();
    delay(10000);
    ESP.restart();
  }

  point.addTag("device", "mppt_victron");

  Serial.println("Program Start");

  Display.begin();
  Display.enableUTF8Print();  // enable UTF8 support for the Arduino print() function
  Display.setFont(u8g2_font_ncenB10_tr);

  // Display Info
  Display.clearBuffer();
  Display.setCursor(0, 12);
  Display.print("WiFi: ");
  Display.setCursor(40, 12);
  Display.print(WiFi.localIP());
  Display.setCursor(0, 26);
  Display.print("InfluxDB UP");
  Display.setCursor(0, 42);
  Display.print(influxDB.getServerUrl());
  Display.setCursor(0, 58);
  Display.print("LoRa UP");
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

  static unsigned long prev_millis;

  // try to parse packet
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    // received a packet

    digitalWrite(blueLED, ON);  // Turn blue LED on

    rssi = LoRa.packetRssi();
    Serial.println("GWY LoRa Signal Strength (dB):  " + rssi);


    // read LoRa packet
    packet = "";  // Clear packet
    while (LoRa.available()) {
      packet += (char)LoRa.read();  // Assemble new packet
    }

    packet.trim();

    parseSerialData(packet);

    digitalWrite(blueLED, OFF);  // Turn blue LED off

    delay(1000);  // to avoid broadcast overload

    LoRa.beginPacket();
    LoRa.print("ACK");
    LoRa.endPacket();
    prev_millis = millis();

  }

  else if (!packetSize && (millis() - prev_millis > 5000)) {
    LoRa.beginPacket();
    LoRa.print("ACK - SEND IT AGAIN");
    LoRa.endPacket();
    Serial.println("ACK - SEND IT AGAIN");
    prev_millis = millis();
  }
}