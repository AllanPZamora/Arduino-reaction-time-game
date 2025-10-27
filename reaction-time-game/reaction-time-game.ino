#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (16x2 LCD at I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
int x_pin = A0;
int y_pin = A1;
int button_pin = 2;
int led_up = 3;
int led_down = 4;
int led_left = 5;
int led_right = 6;

// Joystick calibration
const int JOYSTICK_MIN = 100;    // Threshold for detecting movement
const int JOYSTICK_MAX = 650;
const int JOYSTICK_CENTER_MIN = 400;  // Neutral zone
const int JOYSTICK_CENTER_MAX = 600;

// Game states
enum GameState {
  MENU,
  READY,
  PLAYING,
  SHOWING_RESULT,
  GAME_OVER,
  RESTART_PROMPT
};

GameState currentState = MENU;

// Game variables
int score = 0;
long reactionTime = 0;
long startTime = 0;
int chosenLED = 0;
unsigned long stateTimer = 0;
unsigned long lastButtonCheck = 0;
bool lastButtonState = HIGH;

// Difficulty scaling
int getReadyDelay() {
  // Decrease delay as score increases (min 500ms)
  return max(500, 2000 - (score * 50));
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(led_up, OUTPUT);
  pinMode(led_down, OUTPUT);
  pinMode(led_left, OUTPUT);
  pinMode(led_right, OUTPUT);

  randomSeed(analogRead(A2));  // Better random seed
  showStartMenu();
}

void loop() {
  checkButton();  // Non-blocking button check
  
  switch (currentState) {
    case MENU:
      // Waiting for button press (handled in checkButton)
      break;
      
    case READY:
      if (millis() - stateTimer >= getReadyDelay()) {
        startRound();
      }
      break;
      
    case PLAYING:
      checkJoystick();
      break;
      
    case SHOWING_RESULT:
      if (millis() - stateTimer >= 2000) {
        currentState = READY;
        stateTimer = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Get Ready!");
      }
      break;
      
    case GAME_OVER:
      if (millis() - stateTimer >= 4000) {
        currentState = RESTART_PROMPT;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Start Over?");
        lcd.setCursor(0, 1);
        lcd.print("Press Button");
      }
      break;
      
    case RESTART_PROMPT:
      // Waiting for button press (handled in checkButton)
      break;
  }
}

void showStartMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reaction Game!");
  lcd.setCursor(0, 1);
  lcd.print("Press to Start");
  turnOffAllLEDs();
}

void checkButton() {
  // Debounced button reading
  if (millis() - lastButtonCheck < 50) return;  // Debounce delay
  lastButtonCheck = millis();
  
  bool buttonState = digitalRead(button_pin);
  
  // Detect button press (transition from HIGH to LOW)
  if (lastButtonState == HIGH && buttonState == LOW) {
    if (currentState == MENU) {
      startGame();
    } else if (currentState == RESTART_PROMPT) {
      restartGame();
    }
  }
  
  lastButtonState = buttonState;
}

void startGame() {
  score = 0;
  currentState = READY;
  stateTimer = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready!");
}

void startRound() {
  turnOffAllLEDs();
  
  // Randomly choose a direction
  chosenLED = random(1, 5);  // 1: up, 2: down, 3: left, 4: right
  
  // Light up the corresponding LED
  switch (chosenLED) {
    case 1: digitalWrite(led_up, HIGH); break;
    case 2: digitalWrite(led_down, HIGH); break;
    case 3: digitalWrite(led_left, HIGH); break;
    case 4: digitalWrite(led_right, HIGH); break;
  }
  
  startTime = millis();
  currentState = PLAYING;
}

void checkJoystick() {
  int x_data = analogRead(x_pin);
  int y_data = analogRead(y_pin);
  
  // Check if joystick is in neutral position (ignore small movements)
  bool inNeutral = (x_data >= JOYSTICK_CENTER_MIN && x_data <= JOYSTICK_CENTER_MAX &&
                    y_data >= JOYSTICK_CENTER_MIN && y_data <= JOYSTICK_CENTER_MAX);
  
  if (inNeutral) return;  // No input detected
  
  int detectedDirection = 0;
  
  // Determine which direction is being pushed (with priority for stronger movement)
  if (y_data < JOYSTICK_MIN) {
    detectedDirection = 1;  // Up
  } else if (y_data > JOYSTICK_MAX) {
    detectedDirection = 2;  // Down
  } else if (x_data < JOYSTICK_MIN) {
    detectedDirection = 3;  // Left
  } else if (x_data > JOYSTICK_MAX) {
    detectedDirection = 4;  // Right
  }
  
  if (detectedDirection == 0) return;  // No clear direction
  
  // Check if correct direction
  if (detectedDirection == chosenLED) {
    recordReactionTime();
  } else {
    triggerGameOver();
  }
}

void recordReactionTime() {
  reactionTime = millis() - startTime;
  
  // Score based on reaction time
  if (reactionTime <= 100) {
    score += 10;
  } else if (reactionTime <= 250) {
    score += 5;
  } else if (reactionTime <= 500) {
    score += 3;
  } else if (reactionTime <= 1000) {
    score += 2;
  } else {
    score += 1;
  }
  
  turnOffAllLEDs();
  
  // Display result
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(reactionTime);
  lcd.print("ms");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  currentState = SHOWING_RESULT;
  stateTimer = millis();
}

void triggerGameOver() {
  turnOffAllLEDs();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  currentState = GAME_OVER;
  stateTimer = millis();
}

void restartGame() {
  currentState = MENU;
  score = 0;
  showStartMenu();
}

void turnOffAllLEDs() {
  digitalWrite(led_up, LOW);
  digitalWrite(led_down, LOW);
  digitalWrite(led_left, LOW);
  digitalWrite(led_right, LOW);
}