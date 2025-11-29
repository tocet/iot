#include<WiFi.h>
#include<PubSubClient.h>
#include<ArduinoJson.h>

#define BAUDRATE 115200

const char* ssid = "Laboratorium-IoT";
const char* pass = "IoTL@bolatorium";
const char* mqttServer = "10.23.81.95";
const int mqttPort = 1883;
const char* esp32_board = "esp32_board";

WiFiClient esp_board;
PubSubClient mqtt_client(esp_board);

const int capacity = JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> board;
char json_buffer[256];

StaticJsonDocument<JSON_OBJECT_SIZE(1)> nodered;

void setup() {
  Serial.begin(BAUDRATE);
  board["name"] = esp32_board;
  board["value"] = 0;
  board["unit"] = "my_unit";

  WiFi.begin(ssid, pass);
  Serial.print("Connecting to "); Serial.print(ssid); Serial.print(" with password "); Serial.println(pass);
  Serial.print("Starting connection ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nESP32 board IP "); Serial.println(WiFi.localIP());

  mqtt_client.setServer(mqttServer, mqttPort);
  mqtt_client.setCallback(callback);

  Serial.print("Connecting to MQTT server/broker ");
  Serial.println(mqttServer);
  while (!mqtt_client.connected()) {
    if (mqtt_client.connect(esp32_board)) {
      Serial.println("... connected");
    } else {
      Serial.print("... failed ");
      Serial.println(mqtt_client.state());
      delay(250);
    }
  }
  mqtt_client.subscribe("send2esp32");
}

long p_millis = 0;
int i = 0;
String message;
#define MAIN_DELAY 1000

void loop() {
  if (millis() - p_millis > MAIN_DELAY) {
    board["value"] = random(100);
    serializeJson(board, json_buffer);
    mqtt_client.publish("send_json", json_buffer);
    p_millis = millis();
  }
  mqtt_client.loop();
}

void callback(char* topic, byte* payload, unsigned int len) {
  String json_string = "";

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message content: ");
  for (int i = 0; i < len; i++)
    json_string += (char)payload[i];
  Serial.print(json_string);
  Serial.println(" End of message");

  DeserializationError error = deserializeJson(nodered, payload);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }
  int num = nodered["number"];
  Serial.println(num);
}
