//Sebastian Bruce
//Date: December 16 2023
//Description: Display the score on lcd

#include <LiquidCrystal.h>
#include <Wire.h>
int score;
LiquidCrystal lcd(12,11,5,4,3,2); //These pin numbers are hard coded in on the serial backpack board.

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  lcd.begin (16,2); //Initialize the LCD.

  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop()
{
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  if(Wire.available() >= 1) {
    score = Wire.read();    // receive byte as an integer
    Serial.println(score);  // print the received integer
    lcd.clear();
    lcd.setCursor(0,0); //goto first column and first line (0,0)
    lcd.print("Score: ");
    lcd.print(score);
  }
}