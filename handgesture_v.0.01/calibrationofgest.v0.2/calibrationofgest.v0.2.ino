#include <Wire.h>
#include <MPU6050.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
MPU6050 mpu(0x68);

const int buzzerPin = 8;

// Baseline offsets (from your rest values)
const int baselineAX = -250;
const int baselineAY = -4200;
const int baselineAZ = -15600;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Subtract baseline
  ax -= baselineAX;
  ay -= baselineAY;
  az -= baselineAZ;

  Serial.print("AX="); Serial.print(ax);
  Serial.print(" | AY="); Serial.print(ay);
  Serial.print(" | AZ="); Serial.println(az);

  lcd.clear();

  if (ay > 3000) {              // Forward tilt
    lcd.print("Hi!");
    Serial.println("Hi!");
    tone(buzzerPin, 880, 200);
  }
  else if (az > 3000) {         // Upward tilt
    lcd.print("Food");
    Serial.println("Food");
    tone(buzzerPin, 1000, 200);
  }
  else if (ax > 3000) {         // Right tilt
    lcd.print("Water");
    Serial.println("Water");
    tone(buzzerPin, 1200, 200);
  }
  else if (ax < -3000) {        // Left tilt
    lcd.print("Bye");
    Serial.println("Bye");
    tone(buzzerPin, 1400, 200);
  }
  else {
    lcd.print("No gesture");
    Serial.println("No gesture");
    tone(buzzerPin, 400, 200);
  }

  delay(1000);
  noTone(buzzerPin);
}