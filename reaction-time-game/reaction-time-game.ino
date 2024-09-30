#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (16x2 LCD at I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

int x_pin = A0;
int y_pin = A1;
int sw_pin = 2;

int led_up = 3;
int led_down = 4;
int led_left = 5;
int led_right = 6;

int score = 0;
long reactionTime = 0;
bool waitingForReaction = false;
long startTime = 0;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Set joystick pins as inputs
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);
  pinMode(sw_pin, INPUT_PULLUP);  // Use INPUT_PULLUP for the switch

  // Set LED pins as outputs
  pinMode(led_up, OUTPUT);
  pinMode(led_down, OUTPUT);
  pinMode(led_left, OUTPUT);
  pinMode(led_right, OUTPUT);

  // Welcome message on LCD
  lcd.setCursor(0, 0);
  lcd.print("Reaction Time!");
  delay(2000);
}

void loop() {
  // Reset LEDs
  digitalWrite(led_up, LOW);
  digitalWrite(led_down, LOW);
  digitalWrite(led_left, LOW);
  digitalWrite(led_right, LOW);

  // Randomly choose a direction
  int chosenLED = random(1, 5);  // 1: up, 2: down, 3: left, 4: right

  // Light up the corresponding LED
  if (chosenLED == 1) {
    digitalWrite(led_up, HIGH);
  } else if (chosenLED == 2) {
    digitalWrite(led_down, HIGH);
  } else if (chosenLED == 3) {
    digitalWrite(led_left, HIGH);
  } else if (chosenLED == 4) {
    digitalWrite(led_right, HIGH);
  }

  // Record the start time
  startTime = millis();
  waitingForReaction = true;

  // Wait for the correct joystick movement
  while (waitingForReaction) {
    int x_data = analogRead(x_pin);
    int y_data = analogRead(y_pin);

    // Check if the correct joystick movement is made
    if (chosenLED == 1 && y_data <= 100) {  // Up direction
      recordReactionTime();
    } else if (chosenLED == 2 && y_data >= 650) {  // Down direction
      recordReactionTime();
    } else if (chosenLED == 3 && x_data <= 100) {  // Left direction
      recordReactionTime();
    } else if (chosenLED == 4 && x_data >= 650) {  // Right direction
      recordReactionTime();
    }
  }

  // Wait for 1 second before next round
  delay(1000);
}

void recordReactionTime() {
  // Calculate reaction time
  reactionTime = millis() - startTime;

  // Update score based on reaction time (lower time gives more points)
  if (reactionTime < 500) {
    score += 10;
  } else if (reactionTime < 1000) {
    score += 5;
  } else {
    score += 2;
  }

  // Display reaction time and score on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(reactionTime);
  lcd.print(" ms");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);

  // End the reaction phase
  waitingForReaction = false;
}
