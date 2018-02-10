#include <Adafruit_NeoPixel.h> // Include the adafruit Neopixel Library
#include <MQTTClient.h>
#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h" 

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;
bool rainbowOn = false;
int colorR = 0;
int colorG = 0;
int colorB = 255;
uint16_t TotalSteps = 400;  // total number of steps in the pattern
uint16_t Index;  // current step within the pattern
uint32_t Color1, Color2;  // What colors are in use (used by Fade)

void connect();  // <- predefine connect() for setup()

const int stripCount = 8;
const int ledCount = 17;
const int brightness = 150;

enum mode {modeFade, modeRain, modeSnake, modeSparkle};
mode currentMode = modeRain;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip_1 = Adafruit_NeoPixel(ledCount, 5, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_2 = Adafruit_NeoPixel(ledCount, 6, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_3 = Adafruit_NeoPixel(ledCount, 10, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_4 = Adafruit_NeoPixel(ledCount, 11, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_5 = Adafruit_NeoPixel(ledCount, 12, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_6 = Adafruit_NeoPixel(ledCount, A5, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_7 = Adafruit_NeoPixel(ledCount, A4, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_8 = Adafruit_NeoPixel(ledCount, A3, NEO_GRBW + NEO_KHZ800);

Adafruit_NeoPixel pixelStrips[8] = {strip_1, strip_2, strip_3, strip_4, strip_5, strip_6, strip_7, strip_8};
uint32_t strips[stripCount][ledCount];

void setup() {  
  WiFi.setPins(8,7,4,2);
  for(int x=0; x < stripCount; x++) {
      pixelStrips[x].begin();
      pixelStrips[x].setBrightness(brightness);
      pixelStrips[x].show();
  }
  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  client.begin(IP_ADDRESS, PORT, net);
  client.onMessage(messageReceived);

  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("led-umbrella", "", "")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe("lights");
}


void messageReceived(String &topic, String &payload) {
  const char* delimiter = ",";
  String incomingMode = payload.substring(0,payload.indexOf(delimiter));
  String colorValue   = payload.substring(incomingMode.length()+2,payload.length());
  Serial.println("topic: " + topic);
  Serial.println("payload: " + incomingMode);
  
  if(colorValue == "rainbow") {
    rainbowOn = true;
    colorR = 0;
    colorG = 0;
    colorB = 0;
    Serial.println("color: " + colorValue);
  }
  else {
    rainbowOn = false;  
    // rainbow not used, get color values
    String colorStringR = colorValue.substring(0, colorValue.indexOf(delimiter));
    colorValue.remove(0,colorStringR.length()+2);
    String colorStringG = colorValue.substring(0, colorValue.indexOf(delimiter));
    colorValue.remove(0,colorStringG.length()+2);
    String colorStringB = colorValue.substring(0, colorValue.indexOf(delimiter));
    Serial.println("colorR: " + colorStringR);
    Serial.println("colorG: " + colorStringG);
    Serial.println("colorB: " + colorStringB);
    colorR = colorStringR.toInt();
    colorG = colorStringG.toInt();
    colorB = colorStringB.toInt();
  }
  trigger(incomingMode.c_str());
}

void trigger(const char* event) {
  if (strcmp(event, "fade") == 0){
     currentMode = modeFade;
  } else if (strcmp(event, "rain") == 0){
     currentMode = modeRain;
  } else if (strcmp(event, "sparkle") == 0){
     currentMode = modeSparkle;
  } else if (strcmp(event, "snake") == 0){
   currentMode = modeSnake;
  }
}

void loop() {
  switch(currentMode)
  {
    case modeFade:
      Serial.print("fade\n");
      runFade(110);
      break;
     case modeRain:
      Serial.print("rain\n");
      rain(25);
      break;
    case modeSparkle:
      Serial.print("sparkle\n");
      sparkle(50);
      break;
    case modeSnake:
      Serial.print("snake\n");
      snake(30);
      break;
    default:
      break;
  }
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
}


//Rainbow Program
void rainbow() {
  int j, x, y;
  
  for(j=1; j < 256; j++) {
    for(x=0; x < stripCount; x++) {
      for(y=ledCount-1; y>=0; y--) {   
        pixelStrips[x].setPixelColor(y, Wheel(j));
      }
      pixelStrips[x].show();
    }
  }  
}

// Fill the dots one after the other with a color
void runFade(uint8_t wait) {
  if(rainbowOn == true) {
    int j, x, y;
    for(j=1; j < 256; j++) {
      for(x=0; x < stripCount; x++) {
        for(y=ledCount-1; y>=0; y--) {   
          pixelStrips[x].setPixelColor(y, Wheel(j));
        }
        pixelStrips[x].show();
      }
    }  
  }
  else {
    Index = 0;
    Color1 = strip_1.Color(colorR, colorG, colorB);
    Color2 = strip_1.Color(1,1,1);
    while(Index+50 <= TotalSteps) {
      fadeCycle();  // Fading darker
      Index++;
    }
    while(Index > 50) {
      fadeCycle();  // Fading brighter
      Index--;
    }
  }
  delay(wait);
}
void fadeCycle() {
  int x, y;
  uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
  uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
  uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
  for(x=0; x < stripCount; x++) {
    for(y=ledCount-1; y>=0; y--) {   
      pixelStrips[x].setPixelColor(y, strip_1.Color(red, green, blue));
    }
    pixelStrips[x].show();     
  }
}

void showStrips() {
  for(int x=0; x < stripCount; x++) {
      pixelStrips[x].show();
  }
}

// draw lights according to strips[][] array
void updateStrips() {
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>=0; y--) {
      pixelStrips[x].setPixelColor(y, strips[x][y]);
      pixelStrips[x].show();
    }
  }  
}

void snake(uint8_t wait) {
  uint16_t count = 1;
  uint32_t color = strip_1.Color(colorR, colorG, colorB);
  if (rainbowOn == true) {
    for(int x=0; x < stripCount; x++) {
      for(uint16_t i=0; i<ledCount; i++) {
        if (count < 18) {
           strip_8.setPixelColor(16-i, 0); // Erase 'tail'
           strip_1.setPixelColor(i, Wheel(Index)); // Draw 'head' pixel
        } else if (17 < count && count < 35) {
           strip_1.setPixelColor(i, 0); // Erase 'tail'
           strip_2.setPixelColor(16-i, Wheel(Index)); // Draw 'head' pixel
        } else if (34 < count && count < 52) {
           strip_2.setPixelColor(16-i, 0); // Erase 'tail'
           strip_3.setPixelColor(i, Wheel(Index)); // Draw 'head' pixel
        } else if (51 < count && count < 69) {
           strip_3.setPixelColor(i, 0); // Erase 'tail'
           strip_4.setPixelColor(16-i, Wheel(Index)); // Draw 'head' pixel      
        } else if (68 < count && count < 86) {
          strip_4.setPixelColor(16-i, 0); // Erase 'tail'
          strip_5.setPixelColor(i, Wheel(Index)); // Draw 'head' pixel  
        } else if (85 < count && count < 103) {
          strip_5.setPixelColor(i, 0); // Erase 'tail'
          strip_7.setPixelColor(16-i, Wheel(Index)); // Draw 'head' pixel 
        } else if (102 < count && count < 120) {
          strip_7.setPixelColor(16-i, 0); // Erase 'tail'
          strip_6.setPixelColor(i, Wheel(Index)); // Draw 'head' pixel    
        } else if (119 < count && count < 137) {
          strip_6.setPixelColor(i, 0); // Erase 'tail'
          strip_8.setPixelColor(16-i, Wheel(Index)); // Draw 'head' pixel    
        }
        Index++;
        if(Index > 255) {
          Index = 0;
        }
        count++;
        showStrips();
        delay(wait);
      }
    }
  } else {
      for(int x=0; x < stripCount; x++) {
        for(uint16_t i=0; i<ledCount; i++) {
          if (count < 18) {
             strip_8.setPixelColor(16-i, 0); // Erase 'tail'
             strip_1.setPixelColor(i, color); // Draw 'head' pixel
          } else if (17 < count && count < 35) {
             strip_1.setPixelColor(i, 0); // Erase 'tail'
             strip_2.setPixelColor(16-i, color); // Draw 'head' pixel
          } else if (34 < count && count < 52) {
             strip_2.setPixelColor(16-i, 0); // Erase 'tail'
             strip_3.setPixelColor(i, color); // Draw 'head' pixel
          } else if (51 < count && count < 69) {
             strip_3.setPixelColor(i, 0); // Erase 'tail'
             strip_4.setPixelColor(16-i, color); // Draw 'head' pixel      
          } else if (68 < count && count < 86) {
            strip_4.setPixelColor(16-i, 0); // Erase 'tail'
            strip_5.setPixelColor(i, color); // Draw 'head' pixel  
          } else if (85 < count && count < 103) {
            strip_5.setPixelColor(i, 0); // Erase 'tail'
            strip_7.setPixelColor(16-i, color); // Draw 'head' pixel 
          } else if (102 < count && count < 120) {
            strip_7.setPixelColor(16-i, 0); // Erase 'tail'
            strip_6.setPixelColor(i, color); // Draw 'head' pixel    
          } else if (119 < count && count < 137) {
            strip_6.setPixelColor(i, 0); // Erase 'tail'
            strip_8.setPixelColor(16-i, color); // Draw 'head' pixel    
          }
          count++;
          showStrips();
          delay(wait);
        }
     }
  }
}

void sparkle(uint8_t wait) {
  uint32_t color;
  if (rainbowOn == true) {
    createSparkle(strip_1.Color(random(255), random(255), random(255)));
  } else {
    createSparkle(strip_1.Color(colorR, colorG, colorB)); 
  }
  delay(wait);
}

// Draw 2 random leds, dim all leds, repeat
void createSparkle(uint32_t color) {
    
  // dim any leds that are on
  for(int x=0; x < stripCount; x++) {
    for(int y=0; y< ledCount; y++) {
      strips[x][y] = DimColor(strips[x][y], .85);  
    }
  }

  int x = random(stripCount);
  int y = random(ledCount);
  strips[x][y] = color;

  x = random(stripCount);
  y = random(ledCount);
  strips[x][y] = color;

  updateStrips();
}

// Rain Program
void rain(uint8_t wait) {
  delay(wait);
  uint32_t color;
  if (rainbowOn == true) {
    color = strip_1.Color(random(255), random(255), random(255));
  } else {
    color = strip_1.Color(colorR, colorG, colorB);
  }
  createRain(color);
}

// Base Rain Program
void createRain(uint32_t color) {
  // first move any ON lights down one on each strip
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>0; y--) {
      if (strips[x][y-1] != 0) {
          strips[x][y] = strips[x][y-1];
          strips[x][y-1] = DimColor(strips[x][y-1], .50);  
      }
    }
    // each row: special case turn off all first lights
    strips[x][0] = 0;
  }
  // turn on light at first position of random strip
  strips[random(stripCount)][0] = color;

  updateStrips();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(int WheelPos) {
  if(WheelPos < 85) {
    return strip_1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip_1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip_1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
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

// Return color, dimmed by percentage
uint32_t DimColor(uint32_t color, float dimPercent)
{
  int red = Red(color) * dimPercent;
  int blue = Blue(color) * dimPercent;
  int green = Green(color) * dimPercent;
  uint32_t dimColor = strip_1.Color(red, green, blue);
  return dimColor;
}
