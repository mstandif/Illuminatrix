//**************************************************************//
//  Author: doc@dawning.ca
//  Modified by: michaelzs111@gmail.com
//**************************************************************//
using namespace std;
#include "LED.cpp"
#include "Color.cpp"
#include <FastLED.h>


//##################################################################
//## Main Class ####################################################
//##################################################################

#define NUM_LEDS 60
#define DATA_PIN 6
//#define CLOCK_PIN 5
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

#define BAUD_RATE					9600
#define NUMBER_OF_SPACES_BEFORE_RED_IN_SET_CMD    1
#define NUMBER_OF_SPACES_BEFORE_GREEN_IN_SET_CMD  2
#define NUMBER_OF_SPACES_BEFORE_BLUE_IN_SET_CMD	  3
//#define DEFAULT_MIN_BRIGHTNESS				500
//#define DEFAULT_MIN_BRIGHTNESS_FOR_SINGLE_COLOR_CYCLE	96
#define DEFAULT_CYCLES_PER_STEP				100

//Globals////
CRGB leds[NUM_LEDS];

String inputString;

//HypnoOrb Components
boolean hypnoOrb;
boolean hypnoOrbAscending;
LED* hypnoOrbDeltaSubject;
int cyclesPerStep;
int cyclesSinceLastStep;
int minBrightness;
int stepsSinceChange;
int stepsPerHypnoOrbChange;


//##################################################################
//## Init functions ################################################
//##################################################################

//Arduino's firmware start of execution
void setup() {
  //WS2811,WS2812,WS2812b
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  //APA102
  //FastLED.addLeds<APA102>(leds, NUM_LEDS);  //Comment line 15 and this line if using hardware clock and data
  //FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN>(leds, NUM_LEDS); //Uncomment line 16 and this line for software clock 
  //FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_MHZ(12)>(leds, NUM_LEDS); //Uncomment for slower 12MHz option

	Serial.begin(BAUD_RATE);
	resetColorParameters();
	hypnoOrbAscending = true;
	hypnoOrbDeltaSubject = &LEDS[1];
	cyclesSinceLastStep = 0;
	stepsSinceChange = 0;
	stepsPerHypnoOrbChange = 96;

	Serial.println("Illuminatrix greets you.");
	setForSingleColorCycle(2);
} //end setup


//##################################################################
//## Functions #####################################################
//##################################################################

void resetColorParameters() {
	cyclesPerStep = DEFAULT_CYCLES_PER_STEP;
	minBrightness = DEFAULT_MIN_BRIGHTNESS;
	hypnoOrb = false;
	for (int i=0;i<3;i++) {
		LEDS[i].minPWM = 0;
		LEDS[i].maxPWM = 255;
		LEDS[i].enable();
	}
}

void printLEDs() {
	Serial.print("LED # ");
	Serial.print(i);
	Serial.print(", value: ");
	Serial.print(LEDS[i].getValue());
	Serial.print(", state: ");
	Serial.println(LEDS[i].activated);
}

char* findSpaceDelimitedSubstring(String input, int numberOfLeadingSpaces) {
	int start = 0;
	int end = 0;
	int spacesObserved = 0;

	//find the start of the substring
	for (int i=0;i<input.length();i++) {
		start++;
		if (input.charAt(i) == ' ') {
			spacesObserved++;
			if (spacesObserved >= numberOfLeadingSpaces) i = input.length();
		}
	}

	//find the end of the substring
	end = start;
	for (int i=start;i<input.length();i++) {
		if (input.charAt(i) == ' ') {
			i=input.length();
		} else {
			start++;
		}
	}
	char charBuf[input.length()];
	input.substring(start,end).toCharArray(charBuf, input.length());
	return charBuf;
}

void setLED(String input) {
	//SET    RED     GRN     BLU
	//"SET {0-255} {0-255} {0-255}"
	int red = atoi(findSpaceDelimitedSubstring(input, NUMBER_OF_SPACES_BEFORE_RED_IN_SET_CMD));
	int green = atoi(findSpaceDelimitedSubstring(input, NUMBER_OF_SPACES_BEFORE_GREEN_IN_SET_CMD));
	int blue = atoi(findSpaceDelimitedSubstring(input, NUMBER_OF_SPACES_BEFORE_BLUE_IN_SET_CMD));
  fill_solid( &(leds[i]), NUM_LEDS, CRGB(red, green, blue));

	Serial.print("Changed LED: ");
	Serial.print(red);
	Serial.print(", ");
	Serial.print(green);
	Serial.print(", ");
  Serial.print(blue);
}

void cycleOn() {
	resetColorParameters();
	hypnoOrb=true;
}

void interpretInput(String input) {
	if (inputString.startsWith("ON")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Blue); 
	if (inputString.startsWith("OFF")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Black);  
	if (inputString.startsWith("SET")) setLED(inputString);
	if (inputString.startsWith("WHITE")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::White); 
	if (inputString.startsWith("RED")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Red); 
	if (inputString.startsWith("GREEN")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Green);  
	if (inputString.startsWith("BLUE")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Blue);  
	if (inputString.startsWith("LIGHTBLUE")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::LightBlue);
	if (inputString.startsWith("YELLOW")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Yellow);  
	if (inputString.startsWith("PURPLE")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Purple);  
	if (inputString.startsWith("STANDBY")) fill_solid( &(leds[i]), NUM_LEDS, CRGB::Maroon); 
	if (inputString.startsWith("CYCLEON")) cycleOn();
	if (inputString.startsWith("CYCLEOFF")) hypnoOrb=false;
	if (inputString.startsWith("CYCLEWHITE")) setForWhiteCycle();
	if (inputString.startsWith("CYCLERED")) setForSingleColorCycle(0);
	if (inputString.startsWith("CYCLEGREEN")) setForSingleColorCycle(1);
	if (inputString.startsWith("CYCLEBLUE")) setForSingleColorCycle(2);
	printLEDs();
}

void setForWhiteCycle() {
	resetColorParameters();
	LEDS[0].minPWM = 128;
	LEDS[1].minPWM = 128;
	LEDS[2].minPWM = 96;
	hypnoOrb = true;
}

void setForSingleColorCycle(int ledNumber) {
	resetColorParameters();
	//cyclesPerStep = 30;
	minBrightness = DEFAULT_MIN_BRIGHTNESS_FOR_SINGLE_COLOR_CYCLE;
	for (int i=0;i<3;i++) {
		if (i == ledNumber) {
			LEDS[i].minPWM = minBrightness;
			LEDS[i].maxPWM = 255;
		} else {
			LEDS[i].disable();
		}
	}
	hypnoOrb = true;
}

void serviceInputIfNecessary() {
	char ch = ' ';

	if (Serial.available() > 0) {
		char ch = (char)Serial.read();
		inputString += ch;
		Serial.print(ch);
		if (ch == '\r' || ch == ';') {
			Serial.print('\n');
			interpretInput(inputString);
			inputString = "";
		}
	}
}

//This function is intended to prevent all colours from ever going below the min threshold
bool isNotOkayToDescendFurther(bool preference) {
	int brightnessSum = 0;
	for (int i=0;i<3;i++) {
		LED* led = &LEDS[i];
		brightnessSum += led->getValue();
	}
	if (brightnessSum <= minBrightness) {
		return true;
	} 
	return preference;
}

//Grabs a suitable random LED, disregards inactive LEDs
int getRandomLED() {
	int led = random(0,3); //gets a random int 0 <= led < 3
	int tryLimit = 10;
	while (!LEDS[led].activated) {
		if (tryLimit < 0) break;
		tryLimit--;
		led = random(0,3);
	}
	return led;
}

void serviceHypnoOrbIfNecessary() {
	if (!hypnoOrb) return;

	if (stepsSinceChange > stepsPerHypnoOrbChange) {
		stepsSinceChange = 0;
		//int newLEDNumber = random(0,3);
		int newLEDNumber = getRandomLED();
		int newDirection = random(0,2);
		hypnoOrbDeltaSubject = &LEDS[newLEDNumber];
		hypnoOrbAscending = isNotOkayToDescendFurther((boolean)(newDirection));
	}

	//Increment cycle counter if necessary
	if (cyclesSinceLastStep < cyclesPerStep) {
		cyclesSinceLastStep++;
		return;
	}

	stepsSinceChange++;

	//alter subject according to direction
	cyclesSinceLastStep = 0;
	int deltaValue = 0;
	if (hypnoOrbAscending) deltaValue = 1;
	else deltaValue = -1;

	hypnoOrbDeltaSubject->setValue(hypnoOrbDeltaSubject->getValue() + deltaValue);
}

//Main
void loop() {
  FastLED.show();
	serviceInputIfNecessary();
	serviceHypnoOrbIfNecessary();
}
