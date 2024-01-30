/*
  © MK 18 Jan 2024
  LoRaSender_Serial_Input
  based on ESP32-TTGO sending packets on 868 MHz
*/

#include <SPI.h>
#include <LoRa.h>  // https://github.com/sandeepmistry/arduino-LoRa
//#include <U8g2lib.h>  // https://github.com/olikraus/U8g2_Arduino
#include "Arduino.h"
#include "VeDirectFrameHandler.h"

#define OFF 0  // For LED
#define ON 1

// SPI LoRa Radio
#define LORA_SCK 5    // GPIO5  - SX1276 SCK
#define LORA_MISO 19  // GPIO19 - SX1276 MISO
#define LORA_MOSI 27  // GPIO27 - SX1276 MOSI
#define LORA_CS 18    // GPIO18 - SX1276 CS
#define LORA_RST 14   // GPIO14 - SX1276 RST
#define LORA_IRQ 26   // GPIO26 - SX1276 IRQ (interrupt request)

VeDirectFrameHandler myve;

const int blueLED = LED_BUILTIN;
long rec_count = 0;
String rssi, packet;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 600;
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */

//Define Seria port #2 PIN 16(RX) - 17(TX - not used)
#define RXD2 16
#define TXD2 17
#define SERIAL_PORT Serial2


void setup() {
  Serial.begin(115200);  //MUST BE 19'200bps for VE.ports
  while (!Serial)
    ;

  SERIAL_PORT.begin(19200, SERIAL_8N1, RXD2, TXD2);  // input serial port (VE device)
  SERIAL_PORT.flush();

  pinMode(blueLED, OUTPUT);  // For LED feedback

  Serial.println("DEBUG-setup");

  Serial.println("LoRa Sender");

  // log helper
  myve.setErrorHandler(&LogHelper);
  // hex protocol callback
  myve.addHexCallback(&HexCallback, (void*)42);

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

  Serial.println("LoRa STARTED");
}

void loop() {

  ReadVEData();

  EverySecond();

  // try to parse packet
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    // received a packet

    Serial.print("Received packet '");

    digitalWrite(blueLED, ON);  // Turn blue LED on

    // read packet
    packet = "";  // Clear packet
    while (LoRa.available()) {
      packet += (char)LoRa.read();  // Assemble new packet
    }

    digitalWrite(blueLED, OFF);  // Turn blue LED off

    Serial.println(packet);

    PrintData();
  }

}
