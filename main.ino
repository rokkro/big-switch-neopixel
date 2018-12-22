// Libraries
#include <Adafruit_NeoPixel.h>
#include <Keyboard.h>

// Pins used on Pro Micro
#define LED_PIN 5
#define BUTTON_PIN 2

// Special keyboard keys: https://www.sparkfun.com/tutorials/337
#define KEY_LEFT_CTRL  0x80
#define KEY_LEFT_SHIFT 0x81
#define KEY_LEFT_ALT   0x82
#define KEY_LEFT_GUI   0x83
#define KEY_RIGHT_CTRL 0x84
#define KEY_RIGHT_SHIFT    0x85
#define KEY_RIGHT_ALT  0x86
#define KEY_RIGHT_GUI  0x87
#define KEY_UP_ARROW   0xDA
#define KEY_DOWN_ARROW 0xD9
#define KEY_LEFT_ARROW 0xD8
#define KEY_RIGHT_ARROW    0xD7
#define KEY_BACKSPACE  0xB2
#define KEY_TAB        0xB3
#define KEY_RETURN 0xB0
#define KEY_ESC        0xB1
#define KEY_INSERT 0xD1
#define KEY_DELETE 0xD4
#define KEY_PAGE_UP    0xD3
#define KEY_PAGE_DOWN  0xD6
#define KEY_HOME   0xD2
#define KEY_END        0xD5
#define KEY_CAPS_LOCK  0xC1
#define KEY_F1     0xC2
#define KEY_F2     0xC3
#define KEY_F3     0xC4
#define KEY_F4     0xC5
#define KEY_F5     0xC6
#define KEY_F6     0xC7
#define KEY_F7     0xC8
#define KEY_F8     0xC9
#define KEY_F9     0xCA
#define KEY_F10    0xCB
#define KEY_F11    0xCC
#define KEY_F12    0xCD

Adafruit_NeoPixel strip = Adafruit_NeoPixel(160, LED_PIN, NEO_GRB + NEO_KHZ800);

#define PRESS_THRESHOLD_MIN 100 // Approx min # of cycles button held to trigger action
#define PRESS_HOLD_THRESHOLD 10000 // Approx # of cycles button held down before considered a button hold action

// For finding RGB values: https://arduino.stackexchange.com/a/35769
const uint8_t lights[360] = {
  0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 15, 17, 18, 20, 22, 24, 26, 28, 30, 32, 35, 37, 39,
  42, 44, 47, 49, 52, 55, 58, 60, 63, 66, 69, 72, 75, 78, 81, 85, 88, 91, 94, 97, 101, 104, 107, 111, 114, 117, 121, 124, 127, 131, 134, 137,
  141, 144, 147, 150, 154, 157, 160, 163, 167, 170, 173, 176, 179, 182, 185, 188, 191, 194, 197, 200, 202, 205, 208, 210, 213, 215, 217, 220, 222, 224, 226, 229,
  231, 232, 234, 236, 238, 239, 241, 242, 244, 245, 246, 248, 249, 250, 251, 251, 252, 253, 253, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 253,
  252, 251, 251, 250, 249, 248, 246, 245, 244, 242, 241, 239, 238, 236, 234, 232, 231, 229, 226, 224, 222, 220, 217, 215, 213, 210, 208, 205, 202, 200, 197, 194,
  191, 188, 185, 182, 179, 176, 173, 170, 167, 163, 160, 157, 154, 150, 147, 144, 141, 137, 134, 131, 127, 124, 121, 117, 114, 111, 107, 104, 101, 97, 94, 91,
  88, 85, 81, 78, 75, 72, 69, 66, 63, 60, 58, 55, 52, 49, 47, 44, 42, 39, 37, 35, 32, 30, 28, 26, 24, 22, 20, 18, 17, 15, 13, 12,
  11, 9, 8, 7, 6, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int buttonState = 0; // HIGH, LOW
float downCount = 0; // Increases as button is pushed down and held
bool wasPressed = false; // Changed when button was pushed
int rgb_stepper = 0; // Incremented when rgb values are to change
int r = 0;
int g = 0;
int b = 0;

// Initialization...
void setup() {
  Serial.begin(9600); // Set baud to 9600
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin(); // Initialize NeoPixel ring
  strip.setBrightness(100);
}

/* 
  The buttonState is HIGH when:
    1. The button is not being pressed
    2. During the action of the button being pressed
    3. During the action of releasing a button press
   The buttonState is LOW when:
    1. The button is pressed down.
   During a press & hold action, it generally appears to do HIGHs --> LOW --> HIGH --> LOWs
    - The PRESS_THRESHOLD_MIN prevents the keypress actions during a hold when the second HIGH occurs.
    During a short press and release action, it appears to go HIGHs --> some LOWs and/or HIGHs --> LOWs --> HIGHs
    - It should do enough LOWs to hit the PRESS_THRESHOLD_MIN and trigger the keyboard/LED actions.
*/
void loop() {
  buttonState = digitalRead(2); // Get button state
  Serial.print(buttonState);
  
  if (buttonState == HIGH && wasPressed == true) {
    // If button is pushed for a shorter duration of time
    if (downCount <= PRESS_HOLD_THRESHOLD && downCount >= PRESS_THRESHOLD_MIN) {
      setAllColor(strip.Color(r,g,b));
      Keyboard.write(KEY_LEFT_GUI); // Send Windows/Super/Meta keystroke back to PC
      delay(250); // Short delay to keep LEDs on
      fadeToOff(); // Shut off LEDs
    }
    wasPressed = false;
    downCount = 0;
  }
  else if (buttonState == LOW) { 
    downCount += 1; // Increments every cycle button is down
    // If button is held down long enough
    if (downCount >= PRESS_HOLD_THRESHOLD) {
      delay(10); // Slow down rate at which fade through colors
      rgb_stepper += 1;
      r = lights[(rgb_stepper + 120) % 360];
      g = lights[rgb_stepper];
      b = lights[(rgb_stepper + 240) % 360];
      setAllColor(strip.Color(r,g,b));
      if (rgb_stepper == 360) {
        rgb_stepper = 0;
      }
    }
    wasPressed = true;
  }
}

void fadeToOff(){
  // Temporary copies of stored rgb vals so we don't lose custom color
  int t_r = r;
  int t_g = g;
  int t_b = b;
  for(int i = 0; i < 255; i++){
    if(t_r > 0)
      t_r -=1;
    if(t_g > 0)
      t_g -=1;
    if(t_b > 0)
      t_b -=1;
    setAllColor(strip.Color(t_r, t_g, t_b));
    // Exit method if it's reached 0,0,0 early
    if(t_r == 0 && t_g == 0 && t_b == 0)
      return;
  }
}

// Sets color for all LEDs on NeoPixel ring.
void setAllColor(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}
