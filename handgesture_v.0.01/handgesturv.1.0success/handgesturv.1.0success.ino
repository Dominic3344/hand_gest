#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change to 0x3F if needed
MPU6050 mpu(0x68); // Explicit I2C address

const int buttonHelpPin = 2;
const int buttonGesturePin = 3;
const int buttonnamePin = 4;
const int buzzerPin = 8;

String morseInput = "";
String decodedText = "";
unsigned long pressStart = 0;
bool buttonPressed = false;
unsigned long lastInputTime = 0;

// For long press detection on Help button
bool helpPressed = false;
unsigned long helpPressStart = 0;

// For long press detection on Gesture button
bool gesturePressed = false;
unsigned long gesturePressStart = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  Wire.begin();
  mpu.initialize();
  Serial.begin(9600);

  pinMode(buttonHelpPin, INPUT_PULLUP);
  pinMode(buttonGesturePin, INPUT_PULLUP);
  pinMode(buttonnamePin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // --- Help button with long press ---
  if (digitalRead(buttonHelpPin) == LOW && !helpPressed) {
    helpPressed = true;
    helpPressStart = millis();
  }

  if (digitalRead(buttonHelpPin) == HIGH && helpPressed) {
    unsigned long heldTime = millis() - helpPressStart;
    helpPressed = false;

    if (heldTime > 2000) { // long press >2s
      lcd.clear();
      lcd.print("Hi my name is");
      lcd.setCursor(0, 1);
      lcd.print("Dominic");
      Serial.println("Hi my name is Dominic");

      // special tone sequence
      tone(buzzerPin, 1000, 200); delay(250);
      tone(buzzerPin, 1200, 200); delay(250);
      tone(buzzerPin, 1400, 200); delay(250);
      noTone(buzzerPin);

      delay(1500);
      lcd.clear();
    } else {
      // short press = normal help
      lcd.clear();
      lcd.print("Help Requested!");
      Serial.println("Help Requested!");
      tone(buzzerPin, 1000, 200);
      delay(500);
      lcd.clear();
    }
  }

  // --- Gesture button with long press ---
  if (digitalRead(buttonGesturePin) == LOW && !gesturePressed) {
    gesturePressed = true;
    gesturePressStart = millis();
  }

  if (digitalRead(buttonGesturePin) == HIGH && gesturePressed) {
    unsigned long heldTime = millis() - gesturePressStart;
    gesturePressed = false;

    if (heldTime > 2000) { // long press >2s
      lcd.clear();
      lcd.print("Thank You!");
      Serial.println("Thank You");

      // melody for thank you
      tone(buzzerPin, 800, 200); delay(250);
      tone(buzzerPin, 1000, 200); delay(250);
      tone(buzzerPin, 1200, 200); delay(250);
      noTone(buzzerPin);

      delay(1500);
      lcd.clear();
    } else {
      // short press = normal gesture detection
      lcd.clear();
      lcd.print("Detecting gesture...");
      delay(500);

      int16_t ax, ay, az;
      mpu.getAcceleration(&ax, &ay, &az);

      lcd.clear();
      if (az > -2000) {
        lcd.print(" Food");
        Serial.println("Food");
        tone(buzzerPin, 1000, 200);
      }
      else if (ax > 4000 && abs(ay) < 3000) {
        lcd.print(" Water");
        Serial.println("water");
        tone(buzzerPin, 1200, 200);
      }
      else if (ax < -4000 && abs(ay) < 3000) {
        lcd.print("bye");
        Serial.println("bye");
        tone(buzzerPin, 1400, 200);
      }
      else if ((abs(ax) > 6000 || abs(ay) > 6000) && abs(az + 16000) < 2000) {
        lcd.print(" Hi!");
        Serial.println("Hi!");
        tone(buzzerPin, 880, 200);
      }
      else {
        lcd.print("No gesture");
        tone(buzzerPin, 400, 200);
      }

      delay(1000);
      noTone(buzzerPin);
      lcd.clear();
    }
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
      morseInput = "";
      decodedText = "";
      lcd.clear();
      lcd.print("Reset done!");
      Serial.println("Reset done!");
      tone(buzzerPin, 500, 200); delay(250);
      tone(buzzerPin, 300, 200); delay(250);
      noTone(buzzerPin);
      delay(500);
      lcd.clear();
    } else if (pressDuration < 300) {
      morseInput += ".";
      tone(buzzerPin, 1000, 100); delay(150);
      noTone(buzzerPin);
    } else {
      morseInput += "-";
      tone(buzzerPin, 600, 300); delay(350);
      noTone(buzzerPin);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse: " + morseInput);
    Serial.println(morseInput);
  }

  // Decode after 1.5 sec of no input
  if (millis() - lastInputTime > 1500 && morseInput.length() > 0) {
    char decoded = decodeMorse(morseInput);
    decodedText += decoded;
    lcd.setCursor(0, 1);
    lcd.print(decodedText);
    Serial.println(decodedText);
    morseInput = "";

    playLetterTone(decoded);
  }

  // Add space after 3 sec of no input
  if (millis() - lastInputTime > 3000 && morseInput.length() == 0 && decodedText.length() > 0) {
    decodedText += ' ';
    lcd.setCursor(0, 1);
    lcd.print(decodedText);
    Serial.println(decodedText);
    lastInputTime = millis();

    tone(buzzerPin, 800, 100); delay(150);
    tone(buzzerPin, 1000, 100); delay(150);
    noTone(buzzerPin);
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

// Tone for decoded letter
void playLetterTone(char c) {
  int freq = 0;
  switch (c) {
    case 'A': freq = 440; break;
    case 'B': freq = 494; break;
    case 'C': freq = 523; break;
    case 'D': freq = 587; break;
    case 'E': freq = 659; break;
    case 'F': freq = 698; break;
    case 'G': freq = 784; break;
    case 'H': freq = 880; break;
    case 'I': freq = 988; break;
    case 'J': freq = 1047; break;
    case 'K': freq = 1109; break;
    case 'L': freq = 1175; break;
    case 'M': freq = 1245; break;
    case 'N': freq = 1319; break;
    case 'O': freq = 1397; break;
    case 'P': freq = 1480; break;
    case 'Q': freq = 1568; break;
    case 'R': freq = 1661; break;
    case 'S': freq = 1760; break;
    case 'T': freq = 1865; break;
    case 'U': freq = 1976; break;
    case 'V': freq = 2093; break;
    case 'W': freq = 2217; break;
    case 'X': freq = 2349; break;
    case 'Y': freq = 2489; break;
    case 'Z': freq = 2637; break;
    case '0': freq = 3000; break;
    case '1': freq = 3100; break;
    case '2': freq = 3200; break;
    case '3': freq = 3300; break;
    case '4': freq = 3400; break;
    case '5': freq = 3500; break;
    case '6': freq = 3600; break;
    case '7': freq = 3700; break;
    case '8': freq = 3800; break;
    case '9': freq = 3900; break;
  }

  if (freq > 0) {
    tone(buzzerPin, freq, 150);
    delay(200);
    noTone(buzzerPin);
  }
}