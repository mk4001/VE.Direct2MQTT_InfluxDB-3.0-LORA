# VE.Direct2MQTT_or_InfluxDB_using-Lora
Victron Energy VE.Direct UART data output capture via MQTT and/or InfluxDB Cloud based on Lilygo TTGO ESP32-Lora (based on the Arduino platform)

The idea of untying the Victron Energy Smart Solar MPPT 75/15 from the smartphone's BLE APP and being able to bring the data to the cloud on InfluxDB (even on premise is fine anyway) led me to carry out this project.

The VE.Direct interface, on most Victron Energy products, is in fact a UART that transmits at a record speed of one per second at 19200 baud.

It is necessary to create a simple adapter cable with the help of a JST 2.0 PH 4 connector easily available on both Amazon and Aliexpress.

For the pins of the UART port in question, I only used PINs 1 (GND) and 3 (TX), then connected to Serial2 of an ESP Wroom 32 module (GND and GPIO16 ports).

<img width="1369" alt="Screenshot 2023-07-20 at 09 47 55" src="https://github.com/mk4001/VE.Direct2InfluxDB/assets/50479511/780e0403-754e-42f9-90fa-479fb00701fc">

Data flows from the UART port at a rate of 1 record/sec.

Detailed documentation of the fields, their format and content is collected here:

https://www.victronenergy.com/upload/documents/VE.Direct-Protocol-3.33.pdf

Unlike versions 1.0 and 2.0 of my related projects, this time I wanted to use LoRa technology (with 2xTTGO ESP32) as it allows you to send data even several kilometers away (depending on places and obstacles that can influence the quality of data reception, Obviously)

https://lora-alliance.org

https://www.lilygo.cc/products/lora-v1-3

Once the record has been captured on the first TTGO-ESP32 (sender), we need to parse the fields and collect the data we need before sending it to both MQTT and InfluxDB.2

In this new version 2.0 I used the official Victron Energy library "VeDirectFrameHandler" which is able to manage the "Chesum" field very well at the end of each record coming from a Victron device.

The "ParsingData.ino" function takes on, in this new version, a more relevant role compared to the 1.0 and 2.0 projects as it analyzes the congruence of each field to minimize any transmission errors.
It is no longer invoked for each entire record but rather for each field every time an entire and correct record is validated at Checksum level.

"ParsingData.ino" runs on another TTGO-ESP32 which acts as a "Gateway" and is connected to your home WiFi.

It is easy to create a free account on InfluxDB in the cloud, while for the more daring it is possible to download for free the entire suite necessary for the on-premise installation of a Raspberry PI.

You can start directly in the Cloud from here:

https://cloud2.influxdata.com/signup

Once you have logged in to InfluxDB, you must create a new Bucket (in short, a DB) and related Token which you will then have to copy into the code attached here together with your organization's ID. Easy.

When your Bucket is finally full of data you can decide to create a wonderful Dashboard on Grafana (always in the cloud, always free)

https://grafana.com/auth/sign-up/create-user?pg=hp&plcmt=hero-btn1&cta=create-free-account

With a little imagination and above all practice, you can also create dashboards like this:

![image1](https://github.com/mk4001/VE.Direct2MQTT-2.0/blob/5b6c22a4f9a01b6e5497d393e531245881a66e8c/Screenshot%202024-01-26%20at%2010.20.00.png)
(Grafana Dashboard file: Victron Energy-1700659129739.json)

the obviously "interesting" thing is that you can consult the data of your Victron Energy devices quickly and easily from anywhere and customize them as you prefer.
