#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// http://www.google.ca/patents/US8477010
// https://forum.arduino.cc/index.php?topic=394107.0

/**
   A 1 KHz preamble signal is required to wake the Somfy IR receiver circuit into a higher powered "receive mode".
   Once in this receive mode, it will accept a normal 38 KHz infrared command payload.
   Note that because the wake signal is sent at 1 KHz, it cannot be detected using a 38 KHz infrared receiver.
*/

ESP8266WebServer server(80);

int LED = 5;

float PAYLOAD_DELAY_LONG_US = 7690;
float PAYLOAD_DELAY_SHORT_US = 4887;

const char* ssid = "........";
const char* password = "........";

const char* SOMFY_BUTTON_1 = "10101010000";
const char* SOMFY_BUTTON_2 = "10101010001";
const char* SOMFY_BUTTON_3 = "11101010010";
const char* SOMFY_BUTTON_4 = "10101010011";
const char* SOMFY_BUTTON_5 = "11101010100";
const char* SOMFY_BUTTON_6 = "10101010101";
const char* SOMFY_BUTTON_7 = "11101010110";
const char* SOMFY_BUTTON_8 = "10101010111";
const char* SOMFY_BUTTON_UP = "10101000001";
const char* SOMFY_BUTTON_PAUSE = "11101000011";
const char* SOMFY_BUTTON_DOWN = "11101000010";
const char* SOMFY_BUTTON_1_TO_8 = "11101100000";
const char* SOMFY_BUTTON_1_TO_4 = "10101100001";
const char* SOMFY_BUTTON_5_TO_8 = "11101100010";

boolean done = true;
int count = 0;

void setup() {
  // set up our IR LED
  pinMode(LED, OUTPUT);
  Serial.begin(9600);

  // web server stuff
  //pinMode(led, OUTPUT);
  //digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/command", handleCommand);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {

  server.handleClient();
  
}

void handleCommand() {
  String command = "" + server.arg("command");
    
  if (strcmp(command.c_str(), "up") == 0) {
    sendPreambleAndCommand(SOMFY_BUTTON_UP);
    server.send(200, "text/plain", "Command UP Sent\n");
  } else if (strcmp(command.c_str(), "down") == 0) {
    sendPreambleAndCommand(SOMFY_BUTTON_DOWN);
    server.send(200, "text/plain", "Command DOWN Sent\n");
  } else if (strcmp(command.c_str(), "pause") == 0) {
    sendPreambleAndCommand(SOMFY_BUTTON_PAUSE);
    server.send(200, "text/plain", "Command PAUSE Sent\n");
  } else {
    server.send(404, "text/plain", "Invalid Command\n");
  }
}

void handleRoot() {
  //digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  //digitalWrite(led, 0);
}

void handleNotFound() {
  //  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);
}

void testCommand() {
  if (done) {
    done = false;

    // send the premable signal to wake the motor
    for (int i = 0; i < 11; i++) {

      for (int j = 0; j < 20; j++) {

        // PIN HIGH
        digitalWrite(LED, HIGH);
        delayMicroseconds(5.5);
        // PIN LOW
        digitalWrite(LED, LOW);

        // if on the last, don't delay after pulling pin low
        delayMicroseconds(15);
      }
      // if on last, don't delay
      delayMicroseconds(461);
    }

    // delay 18 ms before sending the payload
    //delay(18);
    delayMicroseconds(17500);

    // roll shades up
    //sendCommand("00010111101");
    sendCommand("11101000010");

    done = true;
  }
}

void sendPreambleAndCommand(const char* code) {
  sendPreamble();
  delayMicroseconds(17500);
  sendCommand(code);
}

// Sends the premable signal to wake the motor
void sendPreamble() {
  for (int i = 0; i < 11; i++) {
    for (int j = 0; j < 20; j++) {
      // PIN HIGH
      digitalWrite(LED, HIGH);
      delayMicroseconds(5.5);
      // PIN LOW
      digitalWrite(LED, LOW);
      // if last, don't delay after pulling pin low
      delayMicroseconds(15);
    }
    // if on last, don't delay
    delayMicroseconds(461);
  }
}

void payloadHigh() {
  for (int i = 0; i < 6; i++) {
    // pull pin HIGH
    digitalWrite(LED, HIGH);
    delayMicroseconds(9.16);
    // pull pin LOW
    digitalWrite(LED, LOW);
    // if on last, don't delay
    delayMicroseconds(17.5);
  }
}

void sendCommand(const char code[]) {
  payloadHigh();
  for (int i = 0; i < strlen(code); i++) {
    if (code[i] == '1') {
      delayMicroseconds(PAYLOAD_DELAY_LONG_US);
    } else {
      delayMicroseconds(PAYLOAD_DELAY_SHORT_US);
    }
    payloadHigh();
  }
}
