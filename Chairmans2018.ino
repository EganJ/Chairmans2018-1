#include <FastLED.h>
#include <Wire.h>
#include "Adafruit_Trellis.h"

//IF THE LEDS LOOP FOREVER... SEE LINE 154

// Comment out for prod to save memory
//#define DEBUG 1

// LEDs
const int LED_PIN = 6;
int isFire = false;
const int LED_START = 32;
const int NUM_LEDS = 231;

// Hex LEDs
const int HEX_LED_START = 211;  // TODO: figure this out
const int HEX_LED_R = 255;
const int HEX_LED_G = 255;
const int HEX_LED_B = 255;

// Thermometer
const int THERMO_START = 0; // First thermo led
const int THERMO_LEVELS[] = {3, 10, 17, 24, 31};
const int THERMO_DELAY = 100; // Delay for slide effect
const int THERMO_COLOR_R = 255;
const int THERMO_COLOR_G = 0;
const int THERMO_COLOR_B = 0;
int thermoLevel = 0;

// Trellis button pad
const int TRELLIS_INT_PIN = A3;
const int TRELLIS_NUM_KEYS = 16;
int buttonPresses = 0;
bool nextButtonClicked = false;
bool lastButtonClicked = false;
bool resetButtonClicked = false;

// Stepper
const int STEPPER_ENABLE_PIN = 3;
const int STEPPER_PIN = 5;
const int STEPPER_DIR_PIN = 4;
const bool STEPPER_FORWARD = 1;
const bool STEPPER_BACKWARDS = 0;
int currentStepper = 0;
boolean resetButton = false;

CRGB leds[NUM_LEDS];
Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);


void setup() {
  Serial.begin(9600);
  #ifdef DEBUG
    Serial.println("2018 Chairmans starting");
    Serial.println("Start LED");
  #endif

  // LEDs
  FastLED.addLeds<NEOPIXEL,LED_PIN>(leds, NUM_LEDS);
  setAll(0, 0, 0);
  delay(100);
  setHexLEDs(true);

  #ifdef DEBUG
    Serial.println("LED setup success");
    Serial.println("Start trellis");
  #endif

  // Trellis
  pinMode(TRELLIS_INT_PIN, INPUT);
  digitalWrite(TRELLIS_INT_PIN, HIGH);
  trellis.begin(0x70);
  delay(100);
  trellisBootLEDs();

  // Stepper
  #ifdef DEBUG
    Serial.println("Start trellis success");
    Serial.println("Start stepper");
  #endif
  pinMode(STEPPER_ENABLE_PIN,OUTPUT); // Enable
  pinMode(STEPPER_PIN,OUTPUT); // Step
  pinMode(STEPPER_DIR_PIN,OUTPUT); // Dir
  #ifdef DEBUG
    Serial.println("Stepper setup success");
    Serial.println("Startup success\n");
  #endif
}
void loop() {
  delay(30);
  if (trellis.readSwitches()) {
    #ifdef DEBUG
      Serial.println();
      Serial.print("Button press: ");
    #endif
    // go through every button
    for (uint8_t i=0; i<TRELLIS_NUM_KEYS; i++) {
      // if it was pressed, turn it on
      if (trellis.justPressed(i)) {
        #ifdef DEBUG
          Serial.println(i);
        #endif
        if (trellis.isLED(i)) {
          trellis.clrLED(i);
        }
        else {
          trellis.setLED(i);
        }
        trellis.writeDisplay();

        //Keep reset LED on
        trellis.setLED(15);

      }
     if (trellis.justReleased(i)) {
        trellis.clrLED(i);
      }
      /* if(resetButton){
                stepperBack(5);
            }*/
    }
    //NEW BUTTON CODE
    if(trellis.justPressed(0) && trellis.justPressed(3) && trellis.justPressed(15) && trellis.justPressed(12)) {
       //reset
      buttonPresses=0;
      setAll(0,0,0);
      setup();
      //setStepper(6,false);
      }
    else if(trellis.justPressed(15)){ //go to last step
      if(buttonPresses >= 0&&buttonPresses<6){toPhase(buttonPresses, false);}
      else if(buttonPresses >= 6){Serial.println("You shall not pass (the 5th step)");  }
      else{Serial.println("You broke it, Congratulations");}
      lastButtonClicked = false;}
    else { //go to next step
      if(buttonPresses>=0&&buttonPresses<6){toPhase(buttonPresses, true);}
      else if(buttonPresses >= 6){Serial.println("You shall not pass (the 5th step)");  }
      else{Serial.println("You broke it, Congratulations");}
      nextButtonClicked = false;}
    /*This line might be causing the stepper to rotate twice
    for (uint8_t i=0; i<TRELLIS_NUM_KEYS - 1; i++) {
      if (trellis.justPressed(i)) {nextButtonClicked = true; lastButtonClicked = false;}
    }*/
  }/*
  if(isFire){//remove this whole section before actual presentation this is just for showcase
    quenchLEDs(true);
    fireLEDs(true);  
  }*/
}    
    //OLD BUTTON CODE
          /*
        switch(i) {
          // First row
          case 0: // Turn to Ignitions
            setStepper(1);
            setThermo(1);
          break;
          case 1: // Thermo level 1
            setThermo(2);
            setStepper(1);
          break;
          case 2: // Turns to Creativity && Thermo level 2 && Fan dial 1
            setThermo(3);
            setStepper(1);
          break;
          case 3: // Turn to Oppertunity && Thermo level 3 && Fan dial 2
            setThermo(4);
            setStepper(1);
            fireLEDs(false);
            isFire = true; 
          break;
          // Second row
          case 4: // Turn to Interaction && Thermo level 4 && Fan dial 3
            // Set case lights to fire
            setThermo(5);
            setStepper(1);
          break;
          case 5: // Turn to Spread && pull bolt
            setStepper(1);
          break;
      
          // Reset button
          case 15:
          resetButton = !resetButton;
          Serial.println("Visual Reset");
            #ifdef DEBUG
              Serial.print("current stepper: ");
              Serial.println(currentStepper);
            #endif
            setAll(0,0,0);
            setHexLEDs(true);
          //break switch-case
          break;
        }      
        */ 

/**
 *Change the phase 
 */
void toPhase(int phase, bool forwards){
  //go to next step
      if(phase < 0) {Serial.println("You can't go to a step behind 0");}

       if(!forwards){phase--;buttonPresses--;}
  if(forwards){     
      setStepper(1);
       setThermo(phase++);  
      
 if(phase == 3){
        fireLEDs(false);
        isFire = true; 
      }
      else if(phase >= 6){
        Serial.println("You shall not pass (the 5th step)");  

      }
      else{
        Serial.println("You broke it, Congratulations");
      }
      Serial.print("ButtonPresses: ");
      Serial.println(phase);
  }

  else  if(!forwards){       
      if(phase == 0){
        stepperBack(32);
        setThermo(1);
      }
      else if(phase == 1){
        setThermo(2);
        stepperBack(32);
      }
      else if(phase == 2){
        setThermo(3);
        stepperBack(32);
      }
      else if(phase == 3){
        setThermo(4);
        stepperBack(32);
        fireLEDs(false);
        isFire = true; 
      }
      else if(phase == 4){
        // Set case lights to fire
        setThermo(5);
        stepperBack(32);
      }
      else if(phase == 5){
        stepperBack(32);
      }
      else if(phase >= 6){
        Serial.println("You shall not pass (the 5th step)");  
      }
      else{
        Serial.println("You broke it, Congratulations");
      }
  }

      if(forwards){
        buttonPresses++;
      }
      else{
        buttonPresses--;
      }
      
      nextButtonClicked = false;
}

/**
 * Set the background LEDs to fire
 */
void fireLEDs(bool showUpdate) {
  for (int i = LED_START; i < HEX_LED_START; i++) {
    
      leds[i] = getRandomFireColor();
        FastLED.show();
     
  }
}
/**
 * sets the background leds to off
 */
 void quenchLEDs(bool showUpdate) {
  for (int i = LED_START; i < HEX_LED_START; i++) {
    leds[i].setRGB(0,0,0);
    FastLED.show();
    }
   FastLED.show();
  }
/**
 * Returns a CRGB color for fire
 */
CRGB getRandomFireColor() {
  int brightness=random(0,20);
  int r = random(150, 200);
  int g = random(0, 20);
  int b = random(0, 3);
  return CRGB(r+brightness*2, g+brightness, b+brightness);
}

/**
 * Set the hex LEDs
 * bool on
 *  true = LEDs on
 *  false = LEDs off
 */
void setHexLEDs(bool on) {
  #ifdef DEBUG
    Serial.print("set hex ");
    Serial.println(on);
  #endif
  if (on) {
    // Turn on
    setPortion(HEX_LED_R, HEX_LED_G, HEX_LED_B, HEX_LED_START, NUM_LEDS);
  } else {
    // Turn off
    setPortion(0, 0, 0, HEX_LED_START, NUM_LEDS);
  }
}

/**
 * Set the stepper motor to rotate
 * int side - side to display
 *  0-5 step for the six sided display
 */
void setStepper(int side) {
  digitalWrite(STEPPER_DIR_PIN, STEPPER_FORWARD);
  #ifdef DEBUG
    Serial.print("(f) Stepper dir: ");
    Serial.print(STEPPER_FORWARD);
    Serial.print(" side: ");
    Serial.println(side); 
  #endif
  // 200 steps = 1 rev
  // One step = about 33 steps. Less to acount for sliding
  currentStepper++;
  currentStepper = currentStepper % 6;
  int steps = 37 * side;
  enableStepper(true);
  for(int x = 0; x < steps * .75; x++) {
    digitalWrite(STEPPER_PIN,HIGH);
    delay(10);
    digitalWrite(STEPPER_PIN,LOW);
  }
  for(int x = 0; x < steps* .25; x++){
    digitalWrite(STEPPER_PIN,HIGH);
    delay(10);
    digitalWrite(STEPPER_PIN,LOW);
    delay (pow(x, 2) / 4);
  }
  enableStepper(false);
  stepperBack(2);
}

/**
 * Set the stepper motor to rotate
 * int side - side to display
 *  0-5 step for the six sided display
 * bool dir - direction
 */
void setStepper(int side, bool dir) {
  #ifdef DEBUG
    Serial.print("Stepper dir: ");
    Serial.print(dir);
    Serial.print(" side: ");
    Serial.println(side);    
  #endif
  digitalWrite(STEPPER_DIR_PIN, dir);
  // 200 steps = 1 rev
  // One step = about 33 steps
  currentStepper++;
  currentStepper = currentStepper % 6;
  int steps = 11 * side;
  enableStepper(true);
  for(int x = 0; x < steps; x++) {
    digitalWrite(STEPPER_PIN,HIGH);
    delay(10);
    digitalWrite(STEPPER_PIN,LOW);
  }
  enableStepper(false);
}

/**
 * Set the stepper motor to rotate
 * int side - side to display
 *  0-5 step for the six sided display
 */
void setStepperBack(int side) {
  digitalWrite(STEPPER_DIR_PIN, STEPPER_BACKWARDS);
  #ifdef DEBUG
    Serial.print("(f) Stepper dir: ");
    Serial.print(STEPPER_FORWARD);
    Serial.print(" side: ");
    Serial.println(side); 
  #endif
  // 200 steps = 1 rev
  // One step = about 33 steps. Less to acount for sliding
  currentStepper++;
  currentStepper = currentStepper % 6;
  int steps = 32 * side;
  enableStepper(true);
  for(int x = 0; x < steps * .75; x++) {
    digitalWrite(STEPPER_PIN,HIGH);
    delay(10);
    digitalWrite(STEPPER_PIN,LOW);
  }
  for(int x = 0; x < steps* .25; x++){
    digitalWrite(STEPPER_PIN,HIGH);
    delay(10);
    digitalWrite(STEPPER_PIN,LOW);
    delay (pow(x, 2) / 4);
  }
  enableStepper(false);
  stepperBack(2);
}


/**
 * Backwards boy
 */
void stepperBack(int steps) {
  #ifdef DEBUG
    Serial.print("Stepper dir: ");
    Serial.print(dir);
    Serial.print(" side: ");
    Serial.println(side);    
  #endif
  digitalWrite(STEPPER_DIR_PIN, false);
  // 200 steps = 1 rev
  // One step = about 33 steps
  currentStepper++;
  currentStepper = currentStepper % 6;
  enableStepper(true);
  for(int x = 0; x < steps; x++) {
    digitalWrite(STEPPER_PIN,HIGH);
    delay(10);
    digitalWrite(STEPPER_PIN,LOW);
  }
  enableStepper(false);
}

/**
 * Enable or disable the stepper
 * bool isEnable - if the stepper should be enabled
 *  true - enable
 *  false - disable
 */
void enableStepper(bool isEnable) {
  if (!isEnable) {
    // Set enable pin to low (disabled)
    digitalWrite(STEPPER_ENABLE_PIN, HIGH);
  } else {
    // Set enable pin to high (enable)
    digitalWrite(STEPPER_ENABLE_PIN, LOW);
  }
}

/**
 * Boot LEDs on trelis
 */
void trellisBootLEDs() {
  for (uint8_t i=0; i<TRELLIS_NUM_KEYS; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();    
    delay(50);
  }
  for (uint8_t i=0; i<TRELLIS_NUM_KEYS; i++) {
    trellis.clrLED(i);
    trellis.writeDisplay();    
    delay(50);
  }
}

/**
 * Set all LEDs to one color
 * int r, g, b - color
 */
void setAll(int r, int g, int b) {
  setPortion(0, 0, 0, 0, NUM_LEDS);
}

/**
 * Set a prtion of LEDs to a color
 * int r, g, b - color
 * int start - first LED
 * int finish - last led
 */
void setPortion(int r, int g, int b, int start, int finish) {
  #ifdef DEBUG
    Serial.print("setPortion: ");
    Serial.print(start);
    Serial.print(" | ");
    Serial.println(finish);
  #endif
  for (int i = start; i < finish; i++) {
    leds[i].setRGB( r, g, b);
  }
  FastLED.show();
}

/**
 * Set a portion of LEDs with a delay between each LED
 * creates a slide effect
 * int r, g, b - color
 * int start - first LED
 * int finish - las led
 */
void setPortion(int r, int g, int b, int start, int finish, int wait) {
  #ifdef DEBUG
    Serial.print("setPortion");
    Serial.print(start);
    Serial.print(" | ");
    Serial.println(finish);
  #endif
  for (int i = start; i < finish; i++) {
    leds[i].setRGB( r, g, b);
    FastLED.show();
    delay(wait);
  }
}

/**
 * Set the thermometer LEDs
 * int level - level to set
 *  0 to 5
 *  0 = off
 *  1-5 = thermo levels
 */
void setThermo(int level) {
  thermoLevel = level % 6;
  #ifdef DEBUG
    Serial.print("Set thermo level: ");
    Serial.println(level);
  #endif
  if (thermoLevel == 0) {
    // Set all thermo LEDs to off
    setPortion(0, 0, 0, 0, LED_START);
  } else {
    // Set thermo leds
    int lowBound = THERMO_START;
    if (level > 1) {
      lowBound = THERMO_LEVELS[level-2];
    }
    setPortion(THERMO_COLOR_R, THERMO_COLOR_G, THERMO_COLOR_B, lowBound, THERMO_LEVELS[level-1], THERMO_DELAY);
  } 
}
