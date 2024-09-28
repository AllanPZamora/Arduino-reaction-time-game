#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2-line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

int x_pin = A0;
int y_pin = A1;
int sw_pin = 2;

int led_up = 3;
int led_down = 4;
int led_left = 5;
int led_right = 6;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();  // Turn on the LCD backlight
  
  // Set joystick pins as inputs
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);  
  pinMode(sw_pin, INPUT);

  // Set LED pins as outputs
  pinMode(led_up, OUTPUT);
  pinMode(led_down, OUTPUT);
  pinMode(led_left, OUTPUT);
  pinMode(led_right, OUTPUT);
}

void loop() {
  // Read the joystick values
  int x_data = analogRead(x_pin);
  int y_data = analogRead(y_pin);

  // Clear LCD for new direction display
  lcd.clear();

  // Control LED states and display direction based on joystick movement
  if (y_data <= 100) {  // Up direction (Y = 0)
    digitalWrite(led_up, HIGH);  // Turn on LED up
    lcd.setCursor(0, 0);  // Set cursor to the top line
    lcd.print("Up direction");  // Display direction on LCD
  } else {
    digitalWrite(led_up, LOW);  // Turn off LED
  }

  if (y_data >= 650) {  // Down direction (Y = 670)
    digitalWrite(led_down, HIGH);  // Turn on LED down
    lcd.setCursor(0, 0);
    lcd.print("Down direction");
  } else {
    digitalWrite(led_down, LOW);  // Turn off LED
  }

  if (x_data <= 100) {  // Left direction (X = 0)
    digitalWrite(led_left, HIGH);  // Turn on LED left
    lcd.setCursor(0, 0);
    lcd.print("Left direction");
  } else {
    digitalWrite(led_left, LOW);  // Turn off LED
  }

  if (x_data >= 650) {  // Right direction (X = 670)
    digitalWrite(led_right, HIGH);  // Turn on LED right
    lcd.setCursor(0, 0);
    lcd.print("Right direction");
  } else {
    digitalWrite(led_right, LOW);  // Turn off LED
  }

  // Print joystick data to the Serial Monitor for debugging
  Serial.print("X: ");
  Serial.print(x_data);
  Serial.print("\tY: ");
  Serial.println(y_data);

  delay(100);
}
