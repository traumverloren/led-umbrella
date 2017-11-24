#include <ESP8266WiFi.h> // Include the ESP8266 Library
#include <Adafruit_NeoPixel.h> // Include the adafruit Neopixel Library
#include <WebSocketsClient.h> // Include Socket.IO client library to communicate with Server!

const char* ssid     = "SSID";
const char* password = "PASSWORD";

String currentProgram = "colorWipe";

#define rowCount = rowCount;
enum mode {colorWipe, modeSunrise, modeSunset, modeDay, modeNight, modeDayStorm, modeNightStorm, rainbow, rain};
mode currentMode = colorWipe;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip_1 = Adafruit_NeoPixel(rowCount, 4, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_2 = Adafruit_NeoPixel(rowCount, 5, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_3 = Adafruit_NeoPixel(rowCount, 2, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_4 = Adafruit_NeoPixel(rowCount, 0, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_5 = Adafruit_NeoPixel(rowCount, 15, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_6 = Adafruit_NeoPixel(rowCount, 12, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_7 = Adafruit_NeoPixel(rowCount, 13, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_8 = Adafruit_NeoPixel(rowCount, 14, NEO_GRBW + NEO_KHZ800);

WiFiClient client;
WebSocketsClient webSocket;

#define MESSAGE_INTERVAL 10000
#define HEARTBEAT_INTERVAL 5000


uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    String msg;
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            isConnected = false;
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);
                isConnected = true;

                // send message to server when Connected
                // socket.io upgrade confirmation message (required)
                webSocket.sendTXT("5");
            }
            break;
        case WStype_TEXT:
            msg = String((char*)payload);
            if(msg.startsWith("42")) {
              trigger(getEventName(msg).c_str(), getEventPayload(msg).c_str(), length);
            }
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }
}

const String getEventName(const String msg) {
  return msg.substring(4, msg.indexOf("\"",4));
}

const String getEventPayload(const String msg) {
  String result = msg.substring(msg.indexOf("\"",4)+2,msg.length()-1);
  if(result.startsWith("\"")) {
    result.remove(0,1);
  }
  if(result.endsWith("\"")) {
    result.remove(result.length()-1);
  }
  return result;
}

void trigger(const char* event, const char * payload, size_t length) {
  Serial.printf("[WSc] trigger event %s\n", event);
  
  if(strcmp(event, "rainbow") == 0) {
    Serial.printf("[WSc] trigger event %s\n", event);
    currentProgram = "rainbow";
//    shirtColors(payload, length);
  } else if (strcmp(event, "colorWipe") == 0){
     Serial.printf("[WSc] trigger event %s\n", event);
     currentProgram = "colorWipe";
  } else if (strcmp(event, "rain") == 0){
     Serial.printf("[WSc] trigger event %s\n", event);
     currentProgram = "rain";
  }
}

void shirtColors(const char * colors, size_t length) {
  char * pEnd;
  int number = (int) strtol(colors, &pEnd, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;

  int number2 = (int) strtol(pEnd, &pEnd, 16);
  int r2 = number2 >> 16;
  int g2 = number2 >> 8 & 0xFF;
  int b2 = number2 & 0xFF;
}

void setup() {  
  strip_1.begin();
  strip_2.begin();
  strip_3.begin();
  strip_4.begin();
  strip_5.begin();
  strip_6.begin();
  strip_7.begin();
  strip_8.begin();  
  strip_1.setBrightness(30); //adjust brightness here
  strip_2.setBrightness(30); //adjust brightness here
  strip_3.setBrightness(30); //adjust brightness here
  strip_4.setBrightness(30); //adjust brightness here
  strip_5.setBrightness(30); //adjust brightness here
  strip_6.setBrightness(30); //adjust brightness here
  strip_7.setBrightness(30); //adjust brightness here
  strip_8.setBrightness(30); //adjust brightness here
  strip_1.show(); // Initialize all pixels to 'off'
  strip_2.show(); // Initialize all pixels to 'off'}
  strip_3.show(); // Initialize all pixels to 'off'}
  strip_4.show(); // Initialize all pixels to 'off'}
  strip_5.show(); // Initialize all pixels to 'off'}
  strip_6.show(); // Initialize all pixels to 'off'}
  strip_7.show(); // Initialize all pixels to 'off'}
  strip_8.show(); // Initialize all pixels to 'off'}
  
  Serial.begin(115200); // Get ready for serial communications and display the connection status
  Serial.print("Connecting to WiFi network -  ");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!!!!!!!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(10);
  webSocket.beginSocketIO("led-umbrella.herokuapp.com", 80);
  webSocket.onEvent(webSocketEvent);
  delay(10);
}

void loop() {
  webSocket.loop();

  if (currentProgram == "colorWipe") {
      
//      colorWipe(strip_1.Color(0, 0, 0, 255), 50); // White
//      colorWipe(strip_1.Color(255, 0, 0, 0), 50); // Red
//      colorWipe(strip_1.Color(0, 255, 0, 0), 50); // Green
//      colorWipe(strip_1.Color(0, 0, 255, 0), 50); // Blue
  } else if (currentProgram == "rainbow") {
      Serial.print("rainbow");
  } else if (currentProgram == "rain" ) {
      Serial.print("rain");
  } else {
    Serial.print("ELSE UH OH");
  }

  delay(10);

  if(isConnected) {

      uint64_t now = millis();

      if(now - messageTimestamp > MESSAGE_INTERVAL) {
        messageTimestamp = now;
        // example socket.io message with type "messageType" and JSON payload
        webSocket.sendTXT("42[\"messageType\",{\"greeting\":\"hello\"}]");
        delay(10);
      }
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
        heartbeatTimestamp = now;
        // socket.io heartbeat message
        webSocket.sendTXT("2");
        delay(10);
      }
   } else {
      Serial.printf("[WSc] Reconnected!\n");
      WiFi.begin(ssid, password);
      delay(10);
      webSocket.beginSocketIO("led-umbrella.herokuapp.com", 80);
      isConnected = true;
    }    
}
 
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip_1.numPixels(); i++) {
      strip_1.setPixelColor(i, c);
      strip_2.setPixelColor(i, c);
      strip_3.setPixelColor(i, c);
      strip_4.setPixelColor(i, c);
      strip_5.setPixelColor(i, c);
      strip_6.setPixelColor(i, c);
      strip_7.setPixelColor(i, c);
      strip_8.setPixelColor(i, c);
      strip_1.show();
      strip_2.show();
      strip_3.show();
      strip_4.show();
      strip_5.show();
      strip_6.show();
      strip_7.show();
      strip_8.show();
      delay(wait);
  }
}
// Rain Program
 
void rain() {
  // Create an array of 20 raindrops
  const int count = 20;
  int pos[count];
  // Set each rain drop at the starting gate.
  // Signify by a position of -1
  for( int i=0; i < count; i++) {
    pos[i]=-1;
  }
  // Main loop. Keep looping until we've done
  // enough "frames."
  boolean done=false;
  int counter = 0;
  while(!done) {
    // Start by turning all LEDs off:
    for(int i=0; i<strip.numPixels(); i++)
    strip.setPixelColor(i, 0);
   
    // Loop for each rain drop
    for( int i=0; i < count; i++) {
    // If the drop is out of the starting gate,
    // turn on the LED for it.
      if( pos[i] >= 0 ) {
        strip.setPixelColor(pos[i], strip.Color(0,0,127));
        // Move the drop down one row
        pos[i] -= 7;
      }
      // If we've fallen off the strip, but us back at the starting gate.
      if( pos[i] < 0 )
        pos[i]=-1;
      }
      // If this drop is at the starting gate, randomly
      // see if we should start it falling.
      if ( pos[i] == -1 && random(40) == 0 && counter < 380 ) {
        // Pick one of the 6 starting spots to begin falling
        pos[i] = 143-random(6);
      }
      strip.show();
      delay(2);
    }
  }
}
//Rainbow Program
void rainbow(uint8_t wait) {
  uint16_t i, j;
  
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
strip.setPixelColor(i, Wheel((i+j) & 255));
}
strip.show();
    delay(wait);
  }
}
  
// Rainbow Cycle Program - Equally distributed
void rainbowCycle(uint8_t wait) {
uint16_t i, j;
  
for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
for(i=0; i< strip.numPixels(); i++) {
strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
}
strip.show();
delay(wait);
}
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
if(WheelPos < 85) {
return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
} else if(WheelPos < 170) {
WheelPos -= 85;
return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
} else {
WheelPos -= 170;
return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
}
}
