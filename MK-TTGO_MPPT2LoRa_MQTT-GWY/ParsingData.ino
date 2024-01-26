/*
  © MK 18 Jan 2024
  Data Parser from LoRa data sender
  based on ESP32-TTGO sending packets on 868 MHz

  Seral Data string Parser function
*/

void parseSerialData(String data) {

  String field;
  String value;
  String topic;

  int Vpos, Ipos, VPVpos, PPVpos, CSpos, MPPTpos, ORpos, ERRpos, LOADpos, ILpos, H19pos, H20pos, H21pos, H22pos, H23pos = 0;

  //Serial.println(data);  //DEBUG ONLY


  //Main or channel 1 (battery) voltage (V)
  if ((Vpos = data.indexOf("V\t")) > 0) {
    field = data.substring(Vpos, data.indexOf('\t', Vpos) + 1);
    value = data.substring(data.indexOf('\t', Vpos) + 1, data.indexOf('\n', Vpos));
    float voltage = value.toFloat() / 1000;
    topic = clientId + "/Battery Voltage";
    publishFloatValue(topic.c_str(), voltage);
  }


  //Main or channel 1 battery current (A)
  if ((Ipos = data.indexOf("I\t")) > 0) {
    field = data.substring(Ipos, data.indexOf('\t', Ipos) + 1);
    value = data.substring(data.indexOf('\t', Ipos) + 1, data.indexOf('\n', Ipos));
    float current = value.toFloat() / 1000;
    topic = clientId + "/Battery Current";
    publishFloatValue(topic.c_str(), current);
  }

  //Panel voltage (V)
  if ((VPVpos = data.indexOf("VPV")) > 0) {
    field = data.substring(VPVpos, data.indexOf('\t', VPVpos) + 1);
    value = data.substring(data.indexOf('\t', VPVpos) + 1, data.indexOf('\n', VPVpos));
    float vpv = value.toFloat() / 1000;
    topic = clientId + "/PV voltage";
    publishFloatValue(topic.c_str(), vpv);
  }

  //Panel Power (W)
  if ((PPVpos = data.indexOf("PPV")) > 0) {
    field = data.substring(PPVpos, data.indexOf('\t', PPVpos) + 1);
    value = data.substring(data.indexOf('\t', PPVpos) + 1, data.indexOf('\n', PPVpos));
    float ppv = value.toFloat();
    topic = clientId + "/PV Power";
    publishFloatValue(topic.c_str(), ppv);
  }

  //State of Operation
  if ((CSpos = data.indexOf("CS")) > 0) {
    field = data.substring(CSpos, data.indexOf('\t', CSpos) + 1);
    value = data.substring(data.indexOf('\t', CSpos) + 1, data.indexOf('\n', CSpos));
    topic = clientId + "/State of operation";
    client.publish(topic.c_str(), value.c_str());
  }

  //Tracker operation mode
  if ((MPPTpos = data.indexOf("MPPT")) > 0) {
    field = data.substring(MPPTpos, data.indexOf('\t', MPPTpos) + 1);
    value = data.substring(data.indexOf('\t', MPPTpos) + 1, data.indexOf('\n', MPPTpos));
    topic = clientId + "/Operation mode";
    client.publish(topic.c_str(), value.c_str());
  }

  //Off reason
  if ((ORpos = data.indexOf("OR")) > 0) {
    field = data.substring(ORpos, data.indexOf('\t', ORpos) + 1);
    value = data.substring(data.indexOf('\t', ORpos) + 1, data.indexOf('\n', ORpos));
    topic = clientId + "/Off reason";
    client.publish(topic.c_str(), value.c_str());
  }

  //Error code
  if ((ERRpos = data.indexOf("ERR")) > 0) {
    field = data.substring(ERRpos, data.indexOf('\t', ERRpos) + 1);
    value = data.substring(data.indexOf('\t', ERRpos) + 1, data.indexOf('\n', ERRpos));
    topic = clientId + "/Error code";
    client.publish(topic.c_str(), value.c_str());
  }

  //Load output state (ON/OFF)
  if ((LOADpos = data.indexOf("LOAD")) > 0) {
    field = data.substring(LOADpos, data.indexOf('\t', LOADpos) + 1);
    value = data.substring(data.indexOf('\t', LOADpos) + 1, data.indexOf('\n', LOADpos));
    topic = clientId + "/Load State";
    client.publish(topic.c_str(), value.c_str());
  }

  //Load current (mA)
  if ((ILpos = data.indexOf("IL")) > 0) {
    field = data.substring(ILpos, data.indexOf('\t', ILpos) + 1);
    value = data.substring(data.indexOf('\t', ILpos) + 1, data.indexOf('\n', ILpos));
    float il = value.toFloat() / 1000;
    topic = clientId + "/Load current";
    publishFloatValue(topic.c_str(), il);
  }

  //Yield total (0.01 kWh)
  if ((H19pos = data.indexOf("H19")) > 0) {
    field = data.substring(H19pos, data.indexOf('\t', H19pos) + 1);
    value = data.substring(data.indexOf('\t', H19pos) + 1, data.indexOf('\n', H19pos));
    float yeld19 = value.toFloat() / 1000;
    topic = clientId + "/Yield total";
    publishFloatValue(topic.c_str(), yeld19);
  }

  //Yield today (0.01 kWh)
  if ((H20pos = data.indexOf("H20")) > 0) {
    field = data.substring(H20pos, data.indexOf('\t', H20pos) + 1);
    value = data.substring(data.indexOf('\t', H20pos) + 1, data.indexOf('\n', H20pos));
    float yeld20 = value.toFloat() / 1000;
    topic = clientId + "/Yield today";
    publishFloatValue(topic.c_str(), yeld20);
  }

  //Maximum power today (W)
  if ((H21pos = data.indexOf("H21")) > 0) {
    field = data.substring(H21pos, data.indexOf('\t', H21pos) + 1);
    value = data.substring(data.indexOf('\t', H21pos) + 1, data.indexOf('\n', H21pos));
    topic = clientId + "/Max power today";
    client.publish(topic.c_str(), value.c_str());
  }

  //Yield yesterday (0.01 kWh)
  if ((H22pos = data.indexOf("H22")) > 0) {
    field = data.substring(H22pos, data.indexOf('\t', H22pos) + 1);
    value = data.substring(data.indexOf('\t', H22pos) + 1, data.indexOf('\n', H22pos));
    float yeld22 = value.toFloat() / 1000;
    topic = clientId + "/Yield yesterday";
    publishFloatValue(topic.c_str(), yeld22);
  }

  //Maximum power yesterday (W)
  if ((H23pos = data.indexOf("H23")) > 0) {
    field = data.substring(H23pos, data.indexOf('\t', H23pos) + 1);
    value = data.substring(data.indexOf('\t', H23pos) + 1, data.indexOf('\n', H23pos));
    topic = clientId + "/Max power yesterday";
    client.publish(topic.c_str(), value.c_str());
  }
}