void publishFloatValue(const char* topic, float value) {
  char message[20];
  snprintf(message, sizeof(message), "%.2f", value);
  client.publish(topic, message);
}