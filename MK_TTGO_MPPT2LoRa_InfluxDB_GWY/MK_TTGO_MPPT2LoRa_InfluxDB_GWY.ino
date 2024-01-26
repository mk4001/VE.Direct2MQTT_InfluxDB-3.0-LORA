/*
  © MK 18 Jan 2024
  LoRaReceiver InfluxDB Gateway
  based on ESP32-TTGO sending packets on 868 MHz
*/

#include <SPI.h>
#include <LoRa.h>  // https://github.com/sandeepmistry/arduino-LoRa


#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


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


const int blueLED = LED_BUILTIN;


void setup() {
  Serial.begin(115200);

  Serial.println("LoRa Receiver & GWY");

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

  pinMode(blueLED, OUTPUT);  // For LED feedback


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
  }

  point.addTag("device", "mppt_victron");

  Serial.println("Program Start");
}

void loop() {


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
  }
}
