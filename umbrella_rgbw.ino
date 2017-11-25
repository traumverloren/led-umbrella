#include <ESP8266WiFi.h> // Include the ESP8266 Library
#include <Adafruit_NeoPixel.h> // Include the adafruit Neopixel Library
#include <WebSocketsClient.h> // Include Socket.IO client library to communicate with Server!

const char* ssid     = "ssid";
const char* password = "pw";

const int length = 17;

enum mode {modeColorWipe, modeRainbowRain, modeRain, modeRainbow, modeSocketConnect};
mode currentMode = modeSocketConnect;
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip_1 = Adafruit_NeoPixel(length, 4, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_2 = Adafruit_NeoPixel(length, 5, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_3 = Adafruit_NeoPixel(length, 2, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_4 = Adafruit_NeoPixel(length, 0, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_5 = Adafruit_NeoPixel(length, 15, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_6 = Adafruit_NeoPixel(length, 12, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_7 = Adafruit_NeoPixel(length, 13, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_8 = Adafruit_NeoPixel(length, 14, NEO_GRBW + NEO_KHZ800);

Adafruit_NeoPixel pixelStrips[8] = {strip_1, strip_2, strip_3, strip_4, strip_5, strip_6, strip_7, strip_8};
uint32_t strips[8][17];
int stripCount = 8;
int ledCount = 17;

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
            currentMode = modeSocketConnect;
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);
                isConnected = true;
                currentMode = modeColorWipe;
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
    currentMode = modeRainbow;
//    shirtColors(payload, length);
  } else if (strcmp(event, "colorWipe") == 0){
     Serial.printf("[WSc] trigger event %s\n", event);
     currentMode = modeColorWipe;
  } else if (strcmp(event, "rainbowRain") == 0){
     Serial.printf("[WSc] trigger event %s\n", event);
     currentMode = modeRainbowRain;
  } else if (strcmp(event, "rain") == 0){
     Serial.printf("[WSc] trigger event %s\n", event);
     currentMode = modeRain;
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

int brightness = 100;
void setup() {  
  strip_1.begin();
  strip_2.begin();
  strip_3.begin();
  strip_4.begin();
  strip_5.begin();
  strip_6.begin();
  strip_7.begin();
  strip_8.begin();  
  strip_1.setBrightness(brightness); //adjust brightness here
  strip_2.setBrightness(brightness); //adjust brightness here
  strip_3.setBrightness(brightness); //adjust brightness here
  strip_4.setBrightness(brightness); //adjust brightness here
  strip_5.setBrightness(brightness); //adjust brightness here
  strip_6.setBrightness(brightness); //adjust brightness here
  strip_7.setBrightness(brightness); //adjust brightness here
  strip_8.setBrightness(brightness); //adjust brightness here
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
  isConnected = true;   
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  
  switch(currentMode)
  {
    case modeColorWipe:
      Serial.print("colorWipe\n");
      colorWipe(strip_1.Color(0, 0, 0, 255), 50); // White
      break;
    case modeRainbow:
      Serial.print("rainbow\n");
      colorWipe(strip_1.Color(255, 0, 0, 0), 50); // Red
      break;
    case modeRain:
      Serial.print("rain\n");
      rain();
      break;
    case modeRainbowRain:
      Serial.print("rainbow rain\n");
      rainbowRain();
      break;
    default:
      break;
  }

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
      currentMode = modeSocketConnect;
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
      showStrips();
      delay(wait);
  }
}

void showStrips() {
  strip_1.show();
  strip_2.show();
  strip_3.show();
  strip_4.show();
  strip_5.show();
  strip_6.show();
  strip_7.show();
  strip_8.show();
}

// Rain Program
void rain() {
  uint32_t color = strip_1.Color(0, 0, 255);
  delay(15);
  // first move any ON lights down one on each strip
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>0; y--) {
      if (strips[x][y-1] != 0) {
          strips[x][y] = DimColor2(strips[x][y-1], .75);//strips[x][y-1];
          
      }
      else {
          strips[x][y] = 0;  
      }
    }
    // each row: special case turn off all first lights
    strips[x][0] = 0;
  }
  // turn on light at first position of random strip
  strips[random(stripCount)][0] = color;

  // draw lights according to strips[][] array
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>=0; y--) {
      pixelStrips[x].setPixelColor(y, strips[x][y]);
 

      
//      pixelStrips[x].setPixelColor(y-1, strips[x][y-1]);
//      strips[x][y-1]=DimColor(strips[x][y]);
//      pixelStrips[x].setPixelColor(y-1, strips[x][y-1]);
//      
//      strips[x][y-2]=DimColor(strips[x][y-1]);
//      pixelStrips[x].setPixelColor(y-2, strips[x][y-2]);
//
//      strips[x][y-3]=DimColor(strips[x][y-2]);
//      pixelStrips[x].setPixelColor(y-3, strips[x][y-3]);
//      
//      strips[x][y-4]=DimColor(strips[x][y-3]);
//      pixelStrips[x].setPixelColor(y-4, strips[x][y-4]);
      
      pixelStrips[x].show();
    }
  }
  
}

// Rainbow Rain Program
void rainbowRain() {
  uint32_t color = strip_1.Color(random(200), random(200), random(200));
  delay(25);
  // first move any ON lights down one on each strip
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>0; y--) {
      if (strips[x][y-1] != 0) {
          strips[x][y] = strips[x][y-1];
      }
      else {
          strips[x][y] = 0;  
      }
    }
    // each row: special case turn off all first lights
    strips[x][0] = 0;
  }
  // turn on light at first position of random strip
  strips[random(stripCount)][0] = color;

  // draw lights according to strips[][] array
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>=0; y--) {
      pixelStrips[x].setPixelColor(y, strips[x][y]);
      pixelStrips[x].show();
    }
  }
  
}

// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color)
{
    return color & 0xFF;
}

// Return color, dimmed by 75% (used by scanner)
uint32_t DimColor(uint32_t color)
{
    uint32_t dimColor = strip_1.Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
    return dimColor;
}

// Return color, dimmed by 75% (used by scanner)
uint32_t DimColor2(uint32_t color, float dimPercent)
{
  int red = Red(color) * dimPercent;
  int blue = Blue(color) * dimPercent;
  int green = Green(color) * dimPercent;
  uint32_t dimColor = strip_1.Color(red, green, blue);
  return dimColor;
}


//Rainbow Program
//void rainbow(uint8_t wait) {
//  uint16_t i, j;
//  
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//strip.setPixelColor(i, Wheel((i+j) & 255));
//}
//strip.show();
//    delay(wait);
//  }
//}
//  
//// Rainbow Cycle Program - Equally distributed
//void rainbowCycle(uint8_t wait) {
//uint16_t i, j;
//  
//for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
//for(i=0; i< strip.numPixels(); i++) {
//strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
//}
//strip.show();
//delay(wait);
//}
//}
//// Input a value 0 to 255 to get a color value.
//// The colours are a transition r - g - b - back to r.
//uint32_t Wheel(byte WheelPos) {
//if(WheelPos < 85) {
//return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//} else if(WheelPos < 170) {
//WheelPos -= 85;
//return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//} else {
//WheelPos -= 170;
//return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//}
//}
