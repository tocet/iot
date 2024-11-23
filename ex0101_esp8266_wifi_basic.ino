#include<WiFi.h>
#define BAUDRATE 115200

const char* ssid = "Laboratorium-IoT";
const char* pass = "IoTL@bolatorium";

void setup() {
  Serial.begin(BAUDRATE);
  WiFi.begin(ssid,pass);
  Serial.print("Connecting .");
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
}
