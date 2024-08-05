/* 
client - sends hex byte strings

ESP8266
NodeMCU ESP-12E

Use DEBUG 1 for serial output

*/
#define DEBUG 1

#if DEBUG == 1
#define sp(x) Serial.print(x)
#define spl(x) Serial.println(x)
#else
#define sp(x)
#define spl(x)
#endif

#include "conf.h"

// WIFI
#include <ESP8266WiFi.h>
#ifndef STASSID
#define STASSID "SSID here"
#define STAPSK "password here"
#endif
#define TCP_PORT 8282
const char *ssid = STASSID;
const char *password = STAPSK;
IPAddress server(192, 168, 178, 20);  // local python server
WiFiClient client;

// Timer
unsigned long ms_delay = 0;
unsigned long ms_current;

// DATA
struct data_struct {
  char type;
  union {
    int32_t value_i;
    float value_f;
  };
  char name[5];
};

// init 2 sets
data_struct data_sets[2];


void setup() {

#if DEBUG == 1
  Serial.begin(115200);
#endif

  spl();
  spl("los geht's");

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // WiFi.printDiag(Serial);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  init_data();

  spl(" setup done");
}


void loop() {
  ms_current = millis();
  if (ms_current - ms_delay >= 20000) {
    ms_delay = ms_current;
    int strength = WiFi.RSSI();
    update_data_set_int(1, strength);
    send_data_struct();
    int anread = analogRead(0);
    spl(anread);
  }
}

// DATA

void init_data() {
  data_sets[0].type = 'f';
  data_sets[0].value_f = PI;
  strcpy(data_sets[0].name, "pi  ");
  data_sets[1].type = 'i';
  data_sets[1].value_i = WiFi.RSSI();
  strcpy(data_sets[1].name, "wlan");
}

void update_data_set_int(int set, int value) {
  data_sets[set].type = 'i';
  data_sets[set].value_i = value;
}

void update_data_set_float(int set, float value) {
  data_sets[set].type = 'f';
  data_sets[set].value_f = value;
}

// SEND DATA

void send_data_struct() {
  unsigned char buffer[32], *ptr;
  ptr = serialize_data_struct(buffer, data_sets[0]);
  ptr = serialize_data_struct(buffer+10, data_sets[1]);
  String hex_string = "";
  for (uint8_t i = 0; i < 20; i++) {
    String add_string = String(buffer[i], HEX);
    if (add_string.length() == 1) {
      hex_string = hex_string + "0";
    }
    hex_string = hex_string + add_string;
  }
  int count_sets = hex_string.length() / 20;
  sp("Sending :"); sp(hex_string); sp(" - "); sp(count_sets); spl(" sets");
  client.connect(server, TCP_PORT);
  client.println(hex_string);
  client.flush();
}

// SERIALIZE

unsigned char *serialize_int(unsigned char *buffer, int32_t value) {
  buffer[0] = static_cast<unsigned char>((value >> 24) & 0xFF);
  buffer[1] = static_cast<unsigned char>((value >> 16) & 0xFF);
  buffer[2] = static_cast<unsigned char>((value >> 8) & 0xFF);
  buffer[3] = static_cast<unsigned char>(value & 0xFF);
  return buffer + 4;
}

unsigned char *serialize_float(unsigned char *buffer, float value) {
  unsigned long d = *(unsigned long *)&value;
  buffer[0] = (d & 0xFF000000) >> 24;
  buffer[1] = (d & 0xFF0000) >> 16;
  buffer[2] = (d & 0xFF00) >> 8;
  buffer[3] = d & 0x00FF;
  return buffer + 4;
}

unsigned char *serialize_char(unsigned char *buffer, char *value) {
  char temp[5] = "";
  strcpy(temp, value);  // copy directly ?
  for (uint8_t i = 0; i < 4; i++) {
    buffer[i] = temp[i];
  }
  return buffer + 4;
}

unsigned char *serialize_type(unsigned char *buffer, char value) {
  buffer[0] = value;
  return buffer + 1;
}

unsigned char *serialize_data_struct(unsigned char *buffer, data_struct &item) {
  buffer = serialize_type(buffer, item.type);
  if (item.type == 'i') {
    buffer = serialize_int(buffer, item.value_i);
  } else if (item.type == 'f') {
    buffer = serialize_float(buffer, item.value_f);
  }
  buffer = serialize_char(buffer, item.name);
  return buffer;
}
