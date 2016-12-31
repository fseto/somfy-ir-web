// http://www.google.ca/patents/US8477010
// https://forum.arduino.cc/index.php?topic=394107.0


/**
 * A 1 KHz preamble signal is required to wake the Somfy IR receiver circuit into a higher powered "receive mode". 
 * Once in this receive mode, it will accept a normal 38 KHz infrared signal payload.
 * Because the wake signal is sent at 1 KHz, it cannot be detected using a 38 KHz infrared receiver.
 */

int LED = 5;

float PAYLOAD_DELAY_LONG_US = 7690;
float PAYLOAD_DELAY_SHORT_US = 4887;

boolean done = true;
int count = 0;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() { 
  delay(5000);
  Serial.println("sending signal ...");
  doTheMove();
  delay(1000);
}

void doTheMove() {

  if (done) {
    done = false;

    // send the wake signal
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
    //sendPayload("00010111101");
      sendPayload("11101000010");

    done = true;
  }
}

void sendPreambleSignal() {
  
}

void sendDataSignal() {
  
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

void sendPayload(char code[]) {
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
