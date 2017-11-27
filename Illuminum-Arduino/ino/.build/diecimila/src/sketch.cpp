#include <Arduino.h>
#include "LED.cpp"
#include "Color.cpp"
void refreshLEDs();
void setup();
void resetColorParameters();
void printLEDs();
void setLEDs(boolean state);
char* findSpaceDelimitedSubstring(String input, int numberOfLeadingSpaces);
void setLED(String input);
void setColor(Color color);
void cycleOn();
void interpretInput(String input);
void setForWhiteCycle();
void setForSingleColorCycle(int ledNumber);
void serviceInputIfNecessary();
bool isNotOkayToDescendFurther(bool preference);
int getRandomLED();
void serviceHypnoOrbIfNecessary();
void loop();
#line 1 "src/sketch.ino"
//**************************************************************//
//  Author: doc@dawning.ca
//**************************************************************//
using namespace std;
//#include "LED.cpp"
//#include "Color.cpp"

//##################################################################
//## Main Class ####################################################
//##################################################################

#define l_R    3    //PWM pin for RED
#define l_G    5    //PWM pin for GREEN
#define l_B    6    //PWM pin for BLUE

#define BAUD_RATE					9600
#define NUMBER_OF_SPACES_BEFORE_PWM_IN_SET_CMD		2
#define NUMBER_OF_SPACES_BEFORE_STATUS_IN_SET_CMD	3
#define NUMBER_OF_SPACES_BEFORE_LED_NUMBER_IN_SET_CMD	1
#define DEFAULT_MIN_BRIGHTNESS				500
#define DEFAULT_MIN_BRIGHTNESS_FOR_SINGLE_COLOR_CYCLE	96
#define DEFAULT_CYCLES_PER_STEP				100

#define NAME_WHITE	"WHITE"
#define	NAME_RED	"RED"
#define NAME_GREEN	"GREEN"
#define NAME_BLUE	"BLUE"
#define	NAME_LIGHTBLUE	"LIGHTBLUE"
#define NAME_YELLOW	"YELLOW"
#define NAME_PURPLE	"PURPLE"
#define NAME_STANDBY	"STANDBY"



//Function Prototypes////
void refreshLEDs();
void refreshLEDState(LED led);

//Globals////
LED LEDS[3];
Color WHITE;
Color RED;
Color GREEN;
Color BLUE;
Color LIGHTBLUE;
Color YELLOW;
Color PURPLE;
Color OFF;
Color STANDBY;
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
void refreshLEDs() {
	for(int i=0;i<3;i++) {
		LEDS[i].refreshLED();
	}
}

//Arduino's firmware start of execution
void setup() {
	//Create color constants
	WHITE.initialize(255, 255, 48);
	RED.initialize(255, 0, 0);
	GREEN.initialize(0, 255, 0);
	BLUE.initialize(0, 0, 255);
	LIGHTBLUE.initialize(0, 166, 255);
	YELLOW.initialize(255, 255, 0);
	PURPLE.initialize(255, 0, 255);
	OFF.initialize(0, 0 ,0);
	STANDBY.initialize(16, 0, 0);


	//Setup for each LED
	LEDS[0].initialize(NAME_RED, l_R, true, 0, 255, 0);
	LEDS[1].initialize(NAME_GREEN, l_G, true, 0, 255, 0);
	LEDS[2].initialize(NAME_BLUE, l_B, true, 0, 255, 180);

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
	for(int i=0;i<3;i++) {
		Serial.print("LED # ");
		Serial.print(i);
		Serial.print(", value: ");
		Serial.print(LEDS[i].getValue());
		Serial.print(", state: ");
		Serial.println(LEDS[i].activated);
	}
}

void setLEDs(boolean state){
	Serial.println("LED state changed");
	for (int i=0;i<3;i++) {
		LEDS[i].activated = state;
	}
	
	if (!state) {
		hypnoOrb = false;
		resetColorParameters();
	}
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
	end=start;
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
	//SET LED# PWM-MOD ENABLE
	//"SET {0-2} {0-9} {0-1}"
	int mode = atoi(findSpaceDelimitedSubstring(input, NUMBER_OF_SPACES_BEFORE_PWM_IN_SET_CMD));
	int ledNumber = atoi(findSpaceDelimitedSubstring(input, NUMBER_OF_SPACES_BEFORE_LED_NUMBER_IN_SET_CMD));
	int status = atoi(findSpaceDelimitedSubstring(input, NUMBER_OF_SPACES_BEFORE_STATUS_IN_SET_CMD));
	LED* led = &LEDS[ledNumber];
	if (status > 0) led->enable();
	else led->disable();
	led->setValue(mode);

	Serial.print("Changed LED #");
	Serial.print(ledNumber);
	Serial.print(", with mode ");
	Serial.print(mode);
	Serial.print(", and status ");
	Serial.println(status);
}

void setColor(Color color) {
	resetColorParameters();	

	for (int i=0;i<3;i++) {
		LED* led = &LEDS[i];
		if (led->name.startsWith(NAME_RED)) led->setTarget(color.red);
		if (led->name.startsWith(NAME_GREEN)) led->setTarget(color.green);
		if (led->name.startsWith(NAME_BLUE)) led->setTarget(color.blue);
		led->enable();
	}
}

void cycleOn() {
	resetColorParameters();
	hypnoOrb=true;
	setLEDs(true);
}

void interpretInput(String input) {
	if (inputString.startsWith("ON")) setColor(BLUE);
	if (inputString.startsWith("OFF")) setColor(OFF);
	if (inputString.startsWith("SET")) setLED(inputString);
	if (inputString.startsWith(NAME_WHITE)) setColor(WHITE);
	if (inputString.startsWith(NAME_RED)) setColor(RED);
	if (inputString.startsWith(NAME_GREEN)) setColor(GREEN);
	if (inputString.startsWith(NAME_BLUE)) setColor(BLUE);
	if (inputString.startsWith(NAME_LIGHTBLUE)) setColor(LIGHTBLUE);
	if (inputString.startsWith(NAME_YELLOW)) setColor(YELLOW);
	if (inputString.startsWith(NAME_PURPLE)) setColor(PURPLE);
	if (inputString.startsWith(NAME_STANDBY)) setColor(STANDBY);
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
	refreshLEDs();
	serviceInputIfNecessary();
	serviceHypnoOrbIfNecessary();
}
