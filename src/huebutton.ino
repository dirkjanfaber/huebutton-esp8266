#include <Credentials.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <NTPtimeESP.h>

const char* ssid = WIFI;
const char* password = WIFIPW;

#define buttonPin 0

// Variables will change:
int hueState;                // the state of the hue light
int buttonState;             // the current reading from the input pin
int lastButtonState;         // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

NTPtime NTPnl("nl.pool.ntp.org");
strDateTime dateTime;

void setup () {
  Serial.begin(115200);
  Serial.println("HUE button startup");

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

  // Our button is input
  pinMode(buttonPin,INPUT_PULLUP);
  delay(100);

  buttonState = digitalRead(buttonPin);
  lastButtonState = buttonState;

}

void loop () {
  // Serial.print("loop");
  // delay(1000);

  int reading = digitalRead(buttonPin);

   // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      Serial.println("button changed");

      WiFiClient client;

      // We need to check the hueState, as other buttons may have switched
      // the light too. We check this by asking the bridge for the state of
      // the light. We need to do a get first:
      if (client.connect(HUE_BRIDGE, 80)) {
        // Make a HTTP request:
        client.println(String("GET /api/")+API_KEY+"/lights/"+LIGHT+" HTTP/1.1");
        client.println(String("Host: ")+HUE_BRIDGE);
        client.println("Connection: close");
        client.println();
      } else {
        // if you didn't get a connection to the server:
        Serial.println("connection failed");
      }

      char data[1024];
      int i=0;
      while (client.connected()) {
        while(client.available()) {
          data[i++] = client.read();
        }
      }

      data[i] = '\0';
      // The interesting part is in the posted data. That contains the first { of
      // the whole package, so search for the '{'.
      char *json;
      json = strchr(data, '{');
      Serial.println(json);

      StaticJsonBuffer<350> jsonBuffer;

      JsonObject& root = jsonBuffer.parseObject(json);
      int reachable = root["state"]["reachable"];

      hueState = root["state"]["on"];

      String body;
      // only toggle the LED if the new button state is HIGH
      if (reachable && buttonState == HIGH) {

          StaticJsonBuffer<200> jsonBuffer;
          JsonObject& root = jsonBuffer.createObject();

          if ( hueState == LOW ) {
            root["on"] = true;


            // Depending on the time of date, we need to send a different color
            // So first ask ntp for the current time
            dateTime = NTPnl.getNTPtime(1.0, 1);
            NTPnl.printDateTime(dateTime);
            root["transitiontime"] = 30;

            if (dateTime.hour >= 18 || dateTime.hour <= 7) {
              root["hue"] = HUE;
              root["bri"] = BRI;
              root["sat"] = SAT;

            } else {
              root["hue"] = 10000;
              root["bri"] = 200;
              root["sat"] = 50;
            }

           } else {
            root["on"] = false;
          }

          root.printTo(body);
          Serial.println(body);

          if (client.connect(HUE_BRIDGE, 80)) {
            client.print(String("PUT /api/")+API_KEY+"/lights/"+LIGHT+"/state HTTP/1.1\r\n" +
                         "Connection: close\r\n" +
                         "Host: " + HUE_BRIDGE + "\r\n" +
                         "Content-length: " + body.length() + "\r\n"
                         "Content-Type: application/json\r\n" +
                         "Accept: */*\r\n\r\n"+
                         body + "\r\n");
        }
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}
