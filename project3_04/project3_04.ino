/*
 * Name: Gusti Scarlett Halima
 * Date: November 1st 2017
 * Class: ENGR 102 intro to electronics (Fall 2017)
 * Software Version: Arduino 1.8.2
 * What it does: this project is a morse code decoder,
 * it accepts input from pressing the morse button and prints
 * a string to the computer.
 */

/*
 * LCD implementation failed
 */
/*#include <LiquidCrystal.h>*/
/*LiquidCrystal lcd(7, 6, 5, 4, 3, 2);*/

const int onButton = 12; //this pin is connected to the on / pause button for the decoder
const int onLED = 13; //this pin signals an LED to high if onButton is LOW
int onState = 0; //this variable stores the onButton value: high = 0, low = 1 (A button has an active low logic or simply input/output state if the opposite of and LED) 
int onCount = 0; //this variable will be tell the decoder whether what the onState is.

const int button = 10; //this pin is connected to the morse code button
int morseButton; //this variable stores the button state
const int buzzerPin = 9; //this pin is connected to the piezzo buzzer

const int erase = 11; //this pin is connected to the erase button
int eraseState; //this variable stores the erase button state which also has an active low logic


/*
 * This project controls the timing of the decoder using global variables.
 * Each loop either increments a value or resets it. For example if the morse code button is not pressed
 * the countDelay variable increments. If it is pressed, the countDelay gets resets to 0. 
 * This decoder reads the morse from the morseArray[] with max reading of four values. The 'count' variable
 * sets the index for the array and adds either a series of long or short (dashes and dots) values in it. It
 * increments value after a certain amount of delay.
 */
//const int TIMING = 50; //this controls how fast the letters between the morse code is inputted, later it will be used as what value for 'countDelay' will be less than
 
int morseArray[4];
int count = 0;
int stringIndex = 0;

int letterInput = 1;
int printSpace = 0;

int countDelay = 0;
int buttonCount = 0; //This variable stores how long the button was pressed

String ourString = ""; //Initially starts with an empty string

void setup() {
  
  Serial.begin(9600);

  //pausing morse decoder/turning on morse decoder
  pinMode(onButton, INPUT);
  pinMode(onLED, OUTPUT);
  
  pinMode(buzzerPin, OUTPUT); //Piezzo buzzer

  pinMode(button, INPUT);
  pinMode(erase, INPUT);

}

void loop() {
  
  eraseState = digitalRead(erase);
  morseButton = digitalRead(button);
  onState = digitalRead(onButton);

  /*when onState is 0 or onButton is pressed, onCount is set to 0 which indicates that the decoder is on*/
  if (onState == LOW) {
    if (onCount == 0) {
      digitalWrite(onLED, HIGH);
      onCount++;
      delay(200);
    }
    else {
      digitalWrite(onLED, LOW);
      onCount--;
      delay(200);
    }
     /*Note that for both the if-else condition, a delay is added to prevent the button being too sensitive
     when pressed. If it were left out, it would almost be impossible to only increment a single value since
     we're accidentally pressing the button for the next loop.*/
  }

  //Erase functionality if enabled only if LED is off / decoder is in paused state
  if (onCount == 0) {
    if (eraseState == LOW) {
      if(stringIndex != 0) { /*Erase button has another condition where stringIndex cannot be 0. This prevents messing up the string index count*/
        tone(buzzerPin, 440, 100);
        ourString.remove(stringIndex - 1, stringIndex); /*Remember that we only want to erase a single letter*/
        stringIndex--;
        delay(300); /*Same reason as previous, delay is added to prevent overpressing the erase button*/
      }
      else {
        tone(buzzerPin, 300, 100);
        Serial.println("[string is empty]");
        delay(300);
      }
    }
  }

  //LED is on
  if (onCount == 1) {
    initializeDecoder(); /**/
  }
  else {
    printSpace = 0; /*I created a printSpace variable to indicate a starting point for when the 
    decoder just turned on. I personally felt this made the timing more easy by having a cue when I should start.
    It basically gives a single low beep at the beginning.*/
    Serial.print("current string: ");
    Serial.println(ourString);
  }

}

/*METHODS*/
 
void initializeDecoder() {
    if (count > 4) { /*This condition makes sure that the morseArray only has 4 */
      Serial.println("Exceeds morse limit");
      count = 0; /*The array resets*/
      countDelay = 0;
    }

    /*This is the main timing value. When the decoder is on and it is left untouched for 50ms,
    it will either signal the first beep to input the morse, count it as a letter, print a space, */
    if (countDelay > 50) { 

        /*This first condition prevents the last morse signal not being read (there was a previous unread value)*/
        if (buttonCount != 0) {
          letterCount();
        }

        else {
          if (count != 0) { /*If user inputs a morse, count will never be zero and therefore the input is read*/
            tone(buzzerPin, 990, 50);
            readArray(); /*This method is printed only to show what values are passed for the morse code*/
            ourString += addLetter(); /*Reads a letter and adds it to the string*/
            stringIndex++;
            
            Serial.print("Current string: ");
            Serial.println(ourString);
        
            countDelay = 0;
            count = 0;
          }

          else if (printSpace == 0){ /*This signals first beep*/
            tone(buzzerPin, 500, 50);
            printSpace++;
            countDelay = 0;
          }
          
          else{ /*printing space*/
            tone(buzzerPin, 770, 50);
            readArray();
            ourString += "_"; /*Currently an underscore to see the amount of space printed*/
            stringIndex++;
            
            Serial.print("Current string: ");
            Serial.println(ourString);
        
            countDelay = 0;
            count = 0;
          }
        }
        
  }

  /*This condition increments the countDelay value when the morseButton isn't pressed*/
  if (morseButton == HIGH) {
    countDelay++;
  }

  /*This condition determines how long the morseButton was pressed*/
  //Long && short button
  if (morseButton == LOW) {
      buttonCount++;
      countDelay = 0;
      tone(buzzerPin, 880, 50);
   }
   
  /*letterCount method is initialized after a 1 ms delay. This makes the morse code input sensitive
  enough to store the long & short values in a single index of the morseArray*/
  if (countDelay > 0) {
    letterCount();
  }



  /*The following Serial print can be commented out to see the current values for delay value and button count*/
//  Serial.print("Delay: ");
//  Serial.println(countDelay);
//  Serial.print("button: ");
//  Serial.println(buttonCount);

}

void letterCount() {
  /*The conditions are set to read the values of the button. Values larger than 0 and smaller than equal 10 sets the 
  reading as short pressed (dot) while values above 10 sees it as long pressed (dash)*/
  if (buttonCount > 0 && buttonCount <= 10) {
      morseArray[count] = 1; //short button is symbolized as 1
      count++;
      Serial.println("short"); 
      countDelay = 0;
      buttonCount = 0;
    }
    else if (buttonCount > 10) {
      morseArray[count] = 2; //long button is symbolized as 2
      count++;
      Serial.println("long");
      countDelay = 0;
      buttonCount = 0;
    }
    else {
      
    }
}

/*This method is only to show what the values are in an index*/
void readArray() {
  for (int i = 0; i < count; i++) {
    if (morseArray[i] == 2) {
      Serial.print("long, ");
    }
    else if (morseArray[i] == 1) {
      Serial.print("short, ");
    }
  }
  Serial.println();
}

/*This methods determines what the array is trying to decode*/
String addLetter() {
  String currentArray = ""; //We first create a string based on the array which will then be compared to the letter that matches the value
  for (int i = 0; i < count; i++) {
    if (morseArray[i] == 2) {
      currentArray += "-"; //2 is a value for dash '-'
    }
    else {
      currentArray += "."; //1 is a value for dot '.'
    }
  }
  
  if (currentArray.equals(".-")) { 
    return "a";
  }
  else if (currentArray.equals("-...")) { 
    return "b";
  }
  else if (currentArray.equals("-.-.")) { 
    return "c";
  }
  else if (currentArray.equals("-..")) { 
    return "d";
  }
  else if (currentArray.equals(".")) { 
    return "e";
  }
  else if (currentArray.equals("..-.")) { 
    return "f";
  }
  else if (currentArray.equals("--.")) { 
    return "g";
  }
  else if (currentArray.equals("....")) { 
    return "h";
  }
  else if (currentArray.equals("..")) { 
    return "i";
  }
  else if (currentArray.equals(".---")) { 
    return "j";
  }
  else if (currentArray.equals("-.-")) {
    return "k";
  }
  else if (currentArray.equals(".-..")) {
    return "l";
  }
  else if (currentArray.equals("--")) {
    return "m";
  }
  else if (currentArray.equals("-.")) {
    return "n";
  }
  else if (currentArray.equals("---")) {
    return "o";
  }
  else if (currentArray.equals(".--.")) {
    return "p";
  }
  else if (currentArray.equals("--.-")) {
    return "q";
  }
  else if (currentArray.equals(".-.")) {
    return "r";
  }
  else if (currentArray.equals("...")) {
    return "s";
  }
  else if (currentArray.equals("-")) {
    return "t";
  }
  else if (currentArray.equals("..-")) {
    return "u";
  }
  else if (currentArray.equals("...-")) {
    return "v";
  }
  else if (currentArray.equals(".--")) {
    return "w";
  }
  else if (currentArray.equals("-..-")) {
    return "x";
  }
  else if (currentArray.equals("-.--")) {
    return "y";
  }
  else if (currentArray.equals("--..")) {
    return "z";
  }
  /*The else statement is never actually used but I decided to add it in case there's a possibility 
  of having a non alphabet combination. I chose a space instead of an empty string so I would still
  increment the stringIndex*/
  else {
    return " ";
  }
}
