boolean initWifi() {
  boolean connected = false;
  WiFi.disconnect();
  delay(200);
  //for (int idx = 0; (!connected && (idx < NUM_SSIDS)); ++idx) {
  debugMessage("Trying Internet: " + (String)ssid);
  WiFi.begin(ssid, password);  //connect to Wi-Fi network
  for (int i = 0; i < 10 && (WiFi.status() != WL_CONNECTED); ++i) {
    delay(300);
  }
  if (WiFi.status() == WL_CONNECTED) {
    connected = true;
    debugMessage("Connected to " + (String)ssid);
    connectedSSID = (char *)ssid;
    return connected;
  }
  // }
  return connected;
}

const char *ID = "M5AirQ";
const char *server = MQTT_SERVER;  // Server URL

void initMQTT() {
  byte mac[6];
  WiFi.macAddress(mac);
  String uniq = "M5AirQ" + String(mac[0], HEX) + String(mac[1], HEX) + String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
  debugMessage("Unique ID: " + uniq);
  log_d(uniq);
  client.setServer(server, 1883);
  client.setKeepAlive(60);
  client.setSocketTimeout(60);
  client.connect((char *)uniq.c_str(), "", "");
  if (client.connected()) {
    debugMessage("Connect to MQTT");
  } else {
    int ret = client.state();
    debugMessage("Cannot conect to MQTT" + String(ret));
  }
}

boolean sendMessageWifi(String message) {
  long timeToStart = millis();
  connectedInternet = initWifi();
  if (connectedInternet) {
    initMQTT();
    //setup time client
    timeClient.begin();
    if (connectedInternet && client.loop()) {
      client.publish("Casting/AirQ/Unit1", message.c_str());
      debugMessage("Message " + String(message));
      debugMessage("Time to send message" + String(millis() - timeToStart));
      //Time client stuff
      timeClient.update();
      timeString = timeClient.getFormattedTime();
      epoch = timeClient.getEpochTime();
      return true;
    }
  }
  WiFi.disconnect();
  debugMessage("Not able to send message over wifi");
  return false;
}