#include <Adafruit_NeoPixel.h>

#define LED_PIN 5 
#define BUTTON_PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(160, LED_PIN, NEO_GRB + NEO_KHZ800);

#define RED strip.Color(255, 0, 0)
#define ORANGE strip.Color(255, 165, 0)
#define YELLOW strip.Color(255, 255, 0)
#define GREEN strip.Color(173, 255, 47)
#define BLUE strip.Color(0, 191, 255)
#define PURPLE strip.Color(148, 0, 211)
#define MAGENTA strip.Color(255, 0, 255)
#define OFF strip.Color(0, 0, 0)

int ledStateIndex = 0;
int buttonState = 0;
int lastButtonState = 0;
int ledStateIndexMax = 7;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(100); //adjust brightness here
    
}

void loop() { 
  buttonState = digitalRead(2);
  Serial.print(buttonState);
  Serial.print(ledStateIndex);
  if(buttonState == HIGH){
    switch(ledStateIndex){
      case 0:
        setAllColor(OFF);
        break;
      case 1:
        setAllColor(RED);
        break;
      case 2:
        setAllColor(ORANGE);
        break;
      case 3:
        setAllColor(YELLOW);
        break;
      case 4:
        setAllColor(GREEN);
        break;
      case 5:
        setAllColor(BLUE);
        break;
      case 6:
        setAllColor(PURPLE);
        break;
      case 7:
        setAllColor(MAGENTA);
        break;
     }
     if(ledStateIndex >= ledStateIndexMax)
      ledStateIndex = 0; 
     lastButtonState = buttonState; 
    }
    else{
      if(lastButtonState != buttonState){
        ledStateIndex++;
        delay(500);
      }
      lastButtonState = buttonState;
    }
  }

void setAllColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
   strip.show();
}
