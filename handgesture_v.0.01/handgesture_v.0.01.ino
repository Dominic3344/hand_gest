#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the address as needed
MPU6050 mpu;

const int buttonHelpPin = 2; // Pin for help button
const int buttonGesturePin = 3; // Pin for gesture button
const int buttonnamePin=4;// Pin for name button

void setup() {
  lcd.begin(16, 2); // Specify the number of columns and rows
  lcd.backlight();
  Wire.begin();
  mpu.initialize();
  
  pinMode(buttonHelpPin, INPUT_PULLUP);
  pinMode(buttonGesturePin, INPUT_PULLUP);
  pinMode(buttonnamePin, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(buttonHelpPin) == LOW) {
    lcd.clear();
    lcd.print("Help Requested!");
    delay(200); // Display for 2 seconds
  }
    if (digitalRead(buttonnamePin) == LOW) {
    lcd.clear();
    String message = "My name is Dominic happy to meet you ";
for (int i = 0; i < message.length(); i++) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message.substring(i, i + 16));
  delay(300); // Adjust speed
}

  }

  if (digitalRead(buttonGesturePin) == LOW) {
    lcd.clear();
    lcd.print("Activating MPU6050");
    delay(200); // Display for 2 seconds

    // Here you can add code to read from the MPU6050 and determine gestures
    // For demonstration, we will just print messages
    lcd.clear();
    lcd.print("Hi!");
    delay(1000);
    lcd.clear();
    lcd.print("Food");
    delay(1000);
    lcd.clear();
    lcd.print("Water");
    delay(1000);
  }
}

























