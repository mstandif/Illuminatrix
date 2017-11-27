//**************************************************************//
//  Created by: michaelzs111@gmail.com
//  Based on Illuminatrix by doc@dawning.ca
//**************************************************************//
using namespace std;
#include <FastLED.h>


//##################################################################
//## Main Class ####################################################
//##################################################################

#define NUM_LEDS 30
#define DATA_PIN 4
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
#define DEFAULT_CYCLES_PER_STEP				100

//Globals////
char inputString[20];
int count = 0;
boolean upDate;

//colorCycle Components
int cycleMode = 0;
int cycle = 0;
boolean cycleAscend = false;
int oldRand1 = 0;
int oldRand2 = 0;

//HypnoOrb Components
boolean hypnoOrb;
boolean hypnoOrbAscending;
int cyclesPerStep;
int cyclesSinceLastStep;
int stepsSinceChange;
int stepsPerHypnoOrbChange;


//##################################################################
//## Init functions ################################################
//##################################################################

//Arduino's firmware start of execution
void setup() {
  delay(3000); // power-up safety delay
  
  //WS2811,WS2812,WS2812b
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  //APA102
  //FastLED.addLeds<APA102>(leds, NUM_LEDS);  //Comment line 15 and this line if using hardware clock and data
  //FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN>(leds, NUM_LEDS); //Uncomment line 16 and this line for software clock 
  //FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_MHZ(12)>(leds, NUM_LEDS); //Uncomment for slower 12MHz option

  FastLED.setBrightness(BRIGHTNESS);
	Serial.begin(BAUD_RATE);
	//hypnoOrbAscending = true;
	//hypnoOrbDeltaSubject = &LEDS[1];
	//cyclesSinceLastStep = 0;
	//stepsSinceChange = 0;
	//stepsPerHypnoOrbChange = 96;
  upDate = true;

  currentPalette;
  currentBlending = LINEARBLEND;

	Serial.println("Illuminum online.");
} //end setup


//##################################################################
//## Functions #####################################################
//##################################################################

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
  fill_solid(leds, NUM_LEDS, CRGB(red, green, blue));

	Serial.print("Changed LED: ");
	Serial.print(red);
	Serial.print(", ");
	Serial.print(green);
	Serial.print(", ");
  Serial.print(blue);
  Serial.print("\n");
}

void cycleOn() {
	hypnoOrb=true;
}

void interpretInput() {
	if (!strcmp(inputString, "ON")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Blue); 
	  upDate = true; 
	  Serial.println("LED Changed: BLUE");
	}
	else if (!strcmp(inputString, "OFF")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Black); 
	  upDate = true; 
	  Serial.println("LED Changed: BLACK");
	}
	else if (!strcmp(inputString, "WHITE")) {
	  fill_solid(leds, NUM_LEDS, CRGB::White); 
	  upDate = true; 
	  Serial.println("LED Changed: WHITE");
	}
	else if (!strcmp(inputString, "RED")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Red); 
	  upDate = true; 
	  Serial.println("LED Changed: RED");
	}
	else if (!strcmp(inputString, "GREEN")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Green); 
	  upDate = true; 
	  Serial.println("LED Changed: GREEN");
	}
	else if (!strcmp(inputString, "BLUE")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Blue); 
	  upDate = true; 
	  Serial.println("LED Changed: BLUE");
	}
	else if (!strcmp(inputString, "LIGHTBLUE")) {
	  fill_solid(leds, NUM_LEDS, CRGB::LightBlue); 
	  upDate = true; 
	  Serial.println("LED Changed: LIGHTBLUE");
	}
	else if (!strcmp(inputString, "YELLOW")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Yellow); 
	  upDate = true; 
	  Serial.println("LED Changed: YELLOW");
	}
	else if (!strcmp(inputString, "PURPLE")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Purple); 
	  upDate = true; 
	  Serial.println("LED Changed: PURPLE");
	}
	else if (!strcmp(inputString, "STANDBY")) {
	  fill_solid(leds, NUM_LEDS, CRGB::Maroon); 
	  upDate = true; 
	  Serial.println("LED Changed: MAROON");
	}
	else if (!strcmp(inputString, "CYCLEON")) cycleOn();
	else if (!strcmp(inputString, "CYCLEOFF")) hypnoOrb=false;
	else if (!strcmp(inputString, "CYCLEWHITE")) setForWhiteCycle();
	else if (!strcmp(inputString, "CYCLERED")) setForSingleColorCycle();
	else if (!strcmp(inputString, "CYCLEGREEN")) setForSingleColorCycle();
	else if (!strcmp(inputString, "CYCLEBLUE")) setForSingleColorCycle();
  else if ((inputString[0] = "S") && (inputString[1] = "E") && (inputString[2] = "T")) {
    setLED(inputString); 
    upDate = true; 
    Serial.println("LED Changed: CUSTOM");
  }
  else {
    Serial.println("Not Found");
  }
  //Serial.println(inputString);
}

void setForWhiteCycle() {
	hypnoOrb = true;
}

void setForSingleColorCycle() {
	hypnoOrb = true;
}

void serviceInputIfNecessary() {
	char ch = ' ';

	if (Serial.available() > 0) {
		char ch = (char)Serial.read();
		//inputString += ch;
		Serial.print(ch);
		if (ch == '\r' || ch == ';') {
      inputString[count] = '\0';
			Serial.print('\n');
			interpretInput();
      Serial.println("Interpreted");
			//inputString = "";
      count = 0;
		}
    else if (ch == '\n') {
      //nothing left-over char from terminal input
    }
    else {
      inputString[count] = ch;
      count++;
    }
	}
}

void SetupTotallyRandomPalette() {
  int newRand1 = random8();
  int newRand2 = random8();
  
  for (int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV(oldRand1 + (i*((newRand1 - oldRand1) / 16)), 255, oldRand2 + (i*((newRand2 - oldRand2) / 16)));
  }
  oldRand1 = newRand1;
  oldRand2 = newRand2;
}

void serviceHypnoOrbIfNecessary() {
	if (!hypnoOrb) return;

	if (stepsSinceChange > stepsPerHypnoOrbChange) {
		stepsSinceChange = 0;
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
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;
  
  if (upDate) {
    FastLED.show();
    upDate = false;
    Serial.println("Updated Strand");
  }
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
  }
  
	serviceInputIfNecessary();
}



