#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// LCD setup (16x2 LCD at I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// EEPROM addresses for high score
#define EEPROM_MAGIC_BYTE 0      // Address for initialization check
#define EEPROM_HIGH_SCORE 1      // Address for high score (2 bytes)
#define MAGIC_NUMBER 42          // Value to check if EEPROM is initialized

// Pin definitions
int x_pin = A0;
int y_pin = A1;
int button_pin = 2;
int led_up = 3;
int led_down = 4;
int led_left = 5;
int led_right = 6;

// Joystick calibration
const int JOYSTICK_MIN = 100;
const int JOYSTICK_MAX = 650;
const int JOYSTICK_CENTER_MIN = 400;
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
int highScore = 0;
bool isNewHighScore = false;
long reactionTime = 0;
long startTime = 0;
int chosenLED = 0;
unsigned long stateTimer = 0;
unsigned long lastButtonCheck = 0;
bool lastButtonState = HIGH;
unsigned long ledBlinkTimer = 0;
bool ledBlinkState = true;
bool ledTimedOut = false;

// Score-based difficulty thresholds
int getCurrentReactionLimit() {
  if (score < 20) return 0;           // Easy: No time limit
  else if (score < 50) return 3000;   // Medium: 3 seconds
  else if (score < 100) return 2000;  // Getting harder: 2 seconds
  else return 1500;                   // Hard: 1.5 seconds
}

int getCurrentLedOnTime() {
  if (score < 20) return 0;           // Easy: LED stays on
  else if (score < 50) return 3000;   // Medium: 3 seconds
  else if (score < 100) return 1000;  // Getting harder: 1 second
  else return 500;                    // Hard: 0.5 seconds
}

int getCurrentLedBlinkInterval() {
  if (score < 100) return 0;          // No blinking until hard mode
  else return 250;                    // Hard: Blink every 250ms
}

String getDifficultyName() {
  if (score < 20) return "Easy";
  else if (score < 50) return "Medium";
  else if (score < 100) return "Hard";
  else return "EXTREME";
}

int getReadyDelay() {
  // Decrease delay as score increases (min 300ms)
  return max(300, 1500 - (score * 10));
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

  randomSeed(analogRead(A2));
  initializeEEPROM();
  loadHighScore();
  showStartMenu();
}

void loop() {
  checkButton();
  
  switch (currentState) {
    case MENU:
      // Waiting for button press
      break;
      
    case READY:
      if (millis() - stateTimer >= getReadyDelay()) {
        startRound();
      }
      break;
      
    case PLAYING:
      handlePlayingState();
      break;
      
    case SHOWING_RESULT:
      if (millis() - stateTimer >= 1500) {
        currentState = READY;
        stateTimer = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Get Ready!");
        lcd.setCursor(0, 1);
        lcd.print(getDifficultyName());
        lcd.print(" Mode");
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
      // Waiting for button press
      break;
  }
}

void showStartMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reaction Game!");
  lcd.setCursor(0, 1);
  lcd.print("High: ");
  lcd.print(highScore);
  lcd.print(" - Start");
  turnOffAllLEDs();
}

void checkButton() {
  if (millis() - lastButtonCheck < 50) return;
  lastButtonCheck = millis();
  
  bool buttonState = digitalRead(button_pin);
  
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
  isNewHighScore = false;
  currentState = READY;
  stateTimer = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready!");
  lcd.setCursor(0, 1);
  lcd.print(getDifficultyName());
  lcd.print(" Mode");
}

void startRound() {
  turnOffAllLEDs();
  ledTimedOut = false;
  
  chosenLED = random(1, 5);
  
  switch (chosenLED) {
    case 1: digitalWrite(led_up, HIGH); break;
    case 2: digitalWrite(led_down, HIGH); break;
    case 3: digitalWrite(led_left, HIGH); break;
    case 4: digitalWrite(led_right, HIGH); break;
  }
  
  startTime = millis();
  ledBlinkTimer = millis();
  ledBlinkState = true;
  currentState = PLAYING;
}

void handlePlayingState() {
  int reactionLimit = getCurrentReactionLimit();
  int ledOnTime = getCurrentLedOnTime();
  int ledBlinkInterval = getCurrentLedBlinkInterval();
  unsigned long elapsedTime = millis() - startTime;
  
  // Handle LED blinking for EXTREME mode
  if (ledBlinkInterval > 0 && !ledTimedOut) {
    if (millis() - ledBlinkTimer >= ledBlinkInterval) {
      ledBlinkState = !ledBlinkState;
      ledBlinkTimer = millis();
      
      if (ledBlinkState) {
        switch (chosenLED) {
          case 1: digitalWrite(led_up, HIGH); break;
          case 2: digitalWrite(led_down, HIGH); break;
          case 3: digitalWrite(led_left, HIGH); break;
          case 4: digitalWrite(led_right, HIGH); break;
        }
      } else {
        turnOffAllLEDs();
      }
    }
  }
  
  // Check if LED should turn off
  if (ledOnTime > 0 && elapsedTime >= ledOnTime && !ledTimedOut) {
    turnOffAllLEDs();
    ledTimedOut = true;
  }
  
  // Check if time limit exceeded
  if (reactionLimit > 0 && elapsedTime >= reactionLimit) {
    triggerGameOver();
    return;
  }
  
  checkJoystick();
}

void checkJoystick() {
  int x_data = analogRead(x_pin);
  int y_data = analogRead(y_pin);
  
  bool inNeutral = (x_data >= JOYSTICK_CENTER_MIN && x_data <= JOYSTICK_CENTER_MAX &&
                    y_data >= JOYSTICK_CENTER_MIN && y_data <= JOYSTICK_CENTER_MAX);
  
  if (inNeutral) return;
  
  int detectedDirection = 0;
  
  if (y_data < JOYSTICK_MIN) {
    detectedDirection = 1;  // Up
  } else if (y_data > JOYSTICK_MAX) {
    detectedDirection = 2;  // Down
  } else if (x_data < JOYSTICK_MIN) {
    detectedDirection = 3;  // Left
  } else if (x_data > JOYSTICK_MAX) {
    detectedDirection = 4;  // Right
  }
  
  if (detectedDirection == 0) return;
  
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
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(reactionTime);
  lcd.print("ms");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  // Show high score indicator if beating it
  if (score > highScore) {
    lcd.print(" NEW!");
  }
  
  currentState = SHOWING_RESULT;
  stateTimer = millis();
}

void triggerGameOver() {
  turnOffAllLEDs();
  
  // Check if new high score
  if (score > highScore) {
    highScore = score;
    isNewHighScore = true;
    saveHighScore();
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (isNewHighScore) {
    lcd.print("NEW HIGH SCORE!");
  } else {
    lcd.print("Game Over!");
  }
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

// EEPROM Functions
void initializeEEPROM() {
  // Check if EEPROM has been initialized
  if (EEPROM.read(EEPROM_MAGIC_BYTE) != MAGIC_NUMBER) {
    // First time setup - initialize high score to 0
    EEPROM.write(EEPROM_MAGIC_BYTE, MAGIC_NUMBER);
    writeIntToEEPROM(EEPROM_HIGH_SCORE, 0);
  }
}

void loadHighScore() {
  highScore = readIntFromEEPROM(EEPROM_HIGH_SCORE);
}

void saveHighScore() {
  writeIntToEEPROM(EEPROM_HIGH_SCORE, highScore);
}

void writeIntToEEPROM(int address, int value) {
  // Write a 2-byte integer to EEPROM
  EEPROM.write(address, value >> 8);      // High byte
  EEPROM.write(address + 1, value & 0xFF); // Low byte
}

int readIntFromEEPROM(int address) {
  // Read a 2-byte integer from EEPROM
  int highByte = EEPROM.read(address);
  int lowByte = EEPROM.read(address + 1);
  return (highByte << 8) | lowByte;
}