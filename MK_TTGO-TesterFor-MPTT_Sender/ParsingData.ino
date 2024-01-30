/*
  © MK 18 Jan 2024
  Data Parser from LoRa data sender
  based on ESP32-TTGO sending packets on 868 MHz

  Seral Data string Parser function
*/

void parseSerialData(String data) {

  String field;
  String value;
  String temp_str;

  int check, Vpos, Ipos, VPVpos, PPVpos, CSpos, MPPTpos, ORpos, ERRpos, LOADpos, ILpos, H19pos, H20pos, H21pos, H22pos, H23pos = 0;
  float voltage, current;

  //Serial.println(data);  //DEBUG ONLY


  //Main or channel 1 (battery) voltage (mV)
  if ((Vpos = data.indexOf("V\t")) > 0) {
    field = data.substring(Vpos, data.indexOf('\t', Vpos) + 1);
    value = data.substring(data.indexOf('\t', Vpos) + 1, data.indexOf('\n', Vpos));
    voltage = value.toFloat() / 1000;
    //point.addField("Battery_voltage", voltage);
    check++;
  }


  //Main or channel 1 battery current (mA)
  if ((Ipos = data.indexOf("I\t")) > 0) {
    field = data.substring(Ipos, data.indexOf('\t', Ipos) + 1);
    value = data.substring(data.indexOf('\t', Ipos) + 1, data.indexOf('\n', Ipos));
    current = value.toFloat() / 1000;
    //point.addField("Battery_current", current);
    check++;
  }


  //Panel voltage (mV)
  if ((VPVpos = data.indexOf("VPV\t")) > 0) {
    field = data.substring(VPVpos, data.indexOf('\t', VPVpos) + 1);
    value = data.substring(data.indexOf('\t', VPVpos) + 1, data.indexOf('\n', VPVpos));
    float vpv = value.toFloat() / 1000;
    //point.addField("PV_voltage", vpv);
    check++;
  }


  //Panel Power (W)
  if ((PPVpos = data.indexOf("PPV\t")) > 0) {
    field = data.substring(PPVpos, data.indexOf('\t', PPVpos) + 1);
    value = data.substring(data.indexOf('\t', PPVpos) + 1, data.indexOf('\n', PPVpos));
    float ppv = value.toFloat();
    //point.addField("PV_power", ppv);
    check++;
  }


  //State of Operation
  if ((CSpos = data.indexOf("CS\t")) > 0) {
    field = data.substring(CSpos, data.indexOf('\t', CSpos) + 1);
    value = data.substring(data.indexOf('\t', CSpos) + 1, data.indexOf('\n', CSpos));
    float state = value.toFloat();
    //point.addField("Operation_state", state);
    check++;
  }


  //Tracker operation mode
  if ((MPPTpos = data.indexOf("MPPT\t")) > 0) {
    field = data.substring(MPPTpos, data.indexOf('\t', MPPTpos) + 1);
    value = data.substring(data.indexOf('\t', MPPTpos) + 1, data.indexOf('\n', MPPTpos));
    float mppt = value.toFloat();
    //point.addField("Tracker_mode", mppt);
    check++;
  }

  //Off reason
  if ((ORpos = data.indexOf("OR\t")) > 0) {
    field = data.substring(ORpos, data.indexOf('\t', ORpos) + 1);
    value = data.substring(data.indexOf('\t', ORpos) + 1, data.indexOf('\n', ORpos));
    float off = value.toFloat();
    //point.addField("Off_reason", off);
    check++;
  }


  //Error code
  if ((ERRpos = data.indexOf("ERR\t")) > 0) {
    field = data.substring(ERRpos, data.indexOf('\t', ERRpos) + 1);
    value = data.substring(data.indexOf('\t', ERRpos) + 1, data.indexOf('\n', ERRpos));
    float err = value.toFloat();
    //point.addField("Error_code", err);
    check++;
  }


  //Load output state (ON/OFF)
  if ((LOADpos = data.indexOf("LOAD\t")) > 0) {
    field = data.substring(LOADpos, data.indexOf('\t', LOADpos) + 1);
    value = data.substring(data.indexOf('\t', LOADpos) + 1, data.indexOf('\n', LOADpos));
    if (value == "OFF") {
      //point.addField("Load_status", 0);
    } else if (value == "ON") {
      //point.addField("Load_status", 1);
    }
    check++;
  }


  //Load current (mA)
  if ((ILpos = data.indexOf("IL\t")) > 0) {
    field = data.substring(ILpos, data.indexOf('\t', ILpos) + 1);
    value = data.substring(data.indexOf('\t', ILpos) + 1, data.indexOf('\n', ILpos));
    float il = value.toFloat() / 1000;
    //point.addField("Load_current", il);
    check++;
  }


  //Yield total (0.01 kWh)
  if ((H19pos = data.indexOf("H19")) > 0) {
    field = data.substring(H19pos, data.indexOf('\t', H19pos) + 1);
    value = data.substring(data.indexOf('\t', H19pos) + 1, data.indexOf('\n', H19pos));
    float yeld19 = value.toFloat() / 1000;
    //point.addField("Yeld_total", yeld19);
    check++;
  }


  //Yield today (0.01 kWh)
  if ((H20pos = data.indexOf("H20\t")) > 0) {
    field = data.substring(H20pos, data.indexOf('\t', H20pos) + 1);
    value = data.substring(data.indexOf('\t', H20pos) + 1, data.indexOf('\n', H20pos));
    float yeld20 = value.toFloat() / 1000;
    //point.addField("Yeld_today", yeld20);
    check++;
  }


  //Maximum power today (W)
  if ((H21pos = data.indexOf("H21\t")) > 0) {
    field = data.substring(H21pos, data.indexOf('\t', H21pos) + 1);
    value = data.substring(data.indexOf('\t', H21pos) + 1, data.indexOf('\n', H21pos));
    float power21 = value.toFloat();
    //point.addField("Max_pw_today", power21);
    check++;
  }


  //Yield yesterday (0.01 kWh)
  if ((H22pos = data.indexOf("H22\t")) > 0) {
    field = data.substring(H22pos, data.indexOf('\t', H22pos) + 1);
    value = data.substring(data.indexOf('\t', H22pos) + 1, data.indexOf('\n', H22pos));
    float yeld22 = value.toFloat() / 1000;
    //point.addField("Yeld_yesterday", yeld22);

    check++;
  }

  //Maximum power yesterday (W)
  if ((H23pos = data.indexOf("H23\t")) > 0) {
    field = data.substring(H23pos, data.indexOf('\t', H23pos) + 1);
    value = data.substring(data.indexOf('\t', H23pos) + 1, data.indexOf('\n', H23pos));
    float power23 = value.toFloat();
    //point.addField("Max_pw_yesterday", power23);
    check++;
  }


  // InfluxDB Cloud flush on last field received
  if (check == 15) {
    //influxDB.writePoint(point);

    // Display Info
    Display.clearBuffer();
    Display.setCursor(0, 12);
    Display.print("Battery V: ");
    Display.print(voltage);
    Display.setCursor(0, 26);
    Display.print("Battery I: ");
    Display.print(current);
   
    Display.setCursor(0, 42);
    temp_str = String(rec_count);  //converting ftemp (the float variable above) to a string
    Display.print("Rec [" + temp_str + "]");
    Display.setCursor(0, 58);
    Display.print("RSSI " + rssi + " dB");
    Display.sendBuffer();
    rec_count++;
  } else {
    Display.clearBuffer();
    Display.setCursor(0, 30);
    Display.print("--- BAD FRAME ---");
    Display.setCursor(0, 58);
    Display.print("RSSI " + rssi + " dB");
    Display.sendBuffer();
    Serial.println("-------------BAD FRAME-------------");
  }

  check = 0;
}