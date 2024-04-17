//Sebastian Bruce
//Date: December 16 2023
//Description: Whack a mole game with multiple modes


#include <LiquidCrystal.h>
#include <Wire.h>

//variables used later
int count;
int score = 0;
char input;
int lightTime = 500;
int waitTime = 250;

int nums[] = {0, 0, 0, 0, 0, 0};
int buttons[] = {2, 3, 7, 4, 5, 6};  // Array of button pins
int LEDs[] = {8, 9, 10, 11, 12, 13}; // Array of LED pins

int pattern[20]; // Maximum pattern length of 20
int patternLength = 1; // Initial pattern length
int userPattern[20];

void setup() {
  //for loop runs through each button/LED in the array and sets them to input/output
  for (int i = 0; i < 6; i++) {
    pinMode(LEDs[i], OUTPUT);
    pinMode(buttons[i], INPUT);
  }
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  //seed the randomness
  randomSeed(analogRead(0));
}

void loop() {
  //print message and options
  Serial.println("Welcome to whack a mole!");
  Serial.println("Please pick a mode.");
  Serial.println("1. Standard, you must click the button that lights up.");
  Serial.println("2. Memory, a series of lights will play, you must input them in the same order.");
  
  while(Serial.available()==0){} //wait for user input
  input = Serial.read(); //store user input
  
  while(true) {
    if(input == '1') { //if user enters 1
      Serial.println("Standard mode selected, game will start shortly.");
      delay(2000);
      while(true) {
        whackMole();
      }
    }else if(input == '2') { //if user enters 2
      Serial.println("Memory mode selected, game will start shortly.");
      delay(2000);
      while(true) {
        memoryMode();
      }
    }else { //if user enters anything else
      Serial.println("Invalid input");
      while(Serial.available()==0){} //wait for user input
      input = Serial.read(); //store user input
      continue;
    }
  }
}

void randomLight() { //selects a random number and turns on/off the associated light
  count = random(0, 7);
  digitalWrite(LEDs[count], HIGH);
  delay(500);
  digitalWrite(LEDs[count], LOW);
}

void whackMole() { //function for standard mode
  randomLight();

  //wait for any button press
  while(digitalRead(buttons[0]) == LOW && digitalRead(buttons[1]) == LOW && digitalRead(buttons[2]) == LOW &&
        digitalRead(buttons[3]) == LOW && digitalRead(buttons[4]) == LOW && digitalRead(buttons[5]) == LOW) {}

  //check if the pressed button is the correct one
  if(digitalRead(buttons[count]) == HIGH) {
    Serial.println("YES");
    score++; //add 1 to score
  }else {
    //check if any button other than button 1 is pressed
    for(int i = 0; i < 6; i++) {
      if(i != count && digitalRead(buttons[i]) == HIGH) {
        Serial.println("NO");
        score--; //subtract 1 from score
        break;
      }
    }
  }
  sendScore(); //sends score to other arduino
}

void memoryMode() { //function for memory mode
  generatePattern();
  displayPattern();
  getUserInput();
  if(checkUserInput()) { //if user inputted pattern matches generated pattern
    Serial.println("Correct! Adding another light to the pattern.");
    patternLength += 1; //increase pattern length by 1
    score++; //add 1 to score
  }else {
    Serial.println("Wrong! Game Over.");
    Serial.println("Restarting from pattern length 1 shortly.");
    patternLength = 1; //reset pattern length to 1
    score = 0; //reset score
    delay(1000);
  }
  sendScore(); //sends score to other arduino
  delay(1000); //delays 1 second before next pattern
}

void generatePattern() { //generates and stores a pattern
  for(int i = 0; i < patternLength; i++) {
    pattern[i] = random(6);
  }
}

void displayPattern() { //uses the generated pattern and stores it
  for(int i = 0; i < patternLength; i++) { //runs through the array and displays each light
    digitalWrite(LEDs[pattern[i]], HIGH);
    delay(lightTime); //delays for however long (gets shorter over time)
    digitalWrite(LEDs[pattern[i]], LOW);
    delay(waitTime); //delays for however long (gets shorter over time)
  }
  if(lightTime >= 200) { //each time a new pattern is displayed, the next gets faster
    lightTime = lightTime - 30;
    waitTime = waitTime - 15;
  }
}

void getUserInput() { //gets input from user and stores it in array
  for(int i = 0; i < patternLength; i++) {
    //wait for the user to press a button
    while(digitalRead(buttons[0]) == LOW && digitalRead(buttons[1]) == LOW && digitalRead(buttons[2]) == LOW &&
           digitalRead(buttons[3]) == LOW && digitalRead(buttons[4]) == LOW && digitalRead(buttons[5]) == LOW) {}

    //determine which button the user pressed
    for(int j = 0; j < 6; j++) {
      if(digitalRead(buttons[j]) == HIGH) {
        userPattern[i] = j;
        break;
      }
    }
    //wait until the button is released
    while(digitalRead(buttons[userPattern[i]]) == HIGH) {}
  }
}

bool checkUserInput() { //validates user input against the generated pattern
  for(int i = 0; i < patternLength; i++) {
    if(userPattern[i] != pattern[i]) {
      return false; //user input doesn't match the pattern
    }
  }
  return true; //user input matches the pattern
}

void sendScore() { //sends score to other arduino
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(score);             // sends one byte as an integer
  Wire.endTransmission();    // stop transmitting
}