#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
MPU6050 mpu;

const int buttonHelpPin = 2;
const int buttonGesturePin = 3;
const int buttonnamePin = 4;

String morseInput = "";
String decodedText = "";
unsigned long pressStart = 0;
bool buttonPressed = false;
unsigned long lastInputTime = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.backlight();
  Wire.begin();
  mpu.initialize();

  pinMode(buttonHelpPin, INPUT_PULLUP);
  pinMode(buttonGesturePin, INPUT_PULLUP);
  pinMode(buttonnamePin, INPUT_PULLUP);
}

void loop() {
  // Help button
  if (digitalRead(buttonHelpPin) == LOW) {
    lcd.clear();
    lcd.print("Help Requested!");
    delay(200);
  }

  // Gesture button
  if (digitalRead(buttonGesturePin) == LOW) {
  lcd.clear();
  lcd.print("MPU6050 ON");
  delay(500);

  int16_t ax, ay, az;
  bool waveDetected = false;

  // Sample for 2 seconds
  for (int i = 0; i < 40; i++) {
    mpu.getAcceleration(&ax, &ay, &az);

    // Detect strong movement on X or Y axis
    if (abs(ax) > 15000 || abs(ay) > 15000) {
      waveDetected = true;
      break;
    }
    delay(50);
  }

  lcd.clear();
  if (waveDetected) {
    lcd.print("Hi!");
  } else {
    lcd.print("No wave");
  }
  delay(1000);
  lcd.clear();
}

  // Morse + Reset + Space button
  int state = digitalRead(buttonnamePin);

  if (state == LOW && !buttonPressed) {
    pressStart = millis();
    buttonPressed = true;
  }

  if (state == HIGH && buttonPressed) {
    unsigned long pressDuration = millis() - pressStart;
    buttonPressed = false;
    lastInputTime = millis();

    if (pressDuration > 2500) {
      // Long press = reset
      morseInput = "";
      decodedText = "";
      lcd.clear();
      lcd.print("Reset done!");
      delay(500);
      lcd.clear();
    } else if (pressDuration < 300) {
      morseInput += ".";
    } else {
      morseInput += "-";
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse: " + morseInput);
  }

  // Decode after 1.5 sec of no input
  if (millis() - lastInputTime > 1500 && morseInput.length() > 0) {
    char decoded = decodeMorse(morseInput);
    decodedText += decoded;
    lcd.setCursor(0, 1);
    lcd.print(decodedText);
    morseInput = "";
  }

  // Add space after 3 sec of no input
  if (millis() - lastInputTime > 3000 && morseInput.length() == 0 && decodedText.length() > 0) {
    decodedText += ' ';
    lcd.setCursor(0, 1);
    lcd.print(decodedText);
    lastInputTime = millis(); // reset timer to avoid repeated spaces
  }
}

// Morse decoder
char decodeMorse(String code) {
  if (code == ".-") return 'A';
  if (code == "-...") return 'B';
  if (code == "-.-.") return 'C';
  if (code == "-..") return 'D';
  if (code == ".") return 'E';
  if (code == "..-.") return 'F';
  if (code == "--.") return 'G';
  if (code == "....") return 'H';
  if (code == "..") return 'I';
  if (code == ".---") return 'J';
  if (code == "-.-") return 'K';
  if (code == ".-..") return 'L';
  if (code == "--") return 'M';
  if (code == "-.") return 'N';
  if (code == "---") return 'O';
  if (code == ".--.") return 'P';
  if (code == "--.-") return 'Q';
  if (code == ".-.") return 'R';
  if (code == "...") return 'S';
  if (code == "-") return 'T';
  if (code == "..-") return 'U';
  if (code == "...-") return 'V';
  if (code == ".--") return 'W';
  if (code == "-..-") return 'X';
  if (code == "-.--") return 'Y';
  if (code == "--..") return 'Z';
  if (code == "-----") return '0';
  if (code == ".----") return '1';
  if (code == "..---") return '2';
  if (code == "...--") return '3';
  if (code == "....-") return '4';
  if (code == ".....") return '5';
  if (code == "-....") return '6';
  if (code == "--...") return '7';
  if (code == "---..") return '8';
  if (code == "----.") return '9';
  return '?';
}