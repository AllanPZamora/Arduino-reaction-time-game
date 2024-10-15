#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (16x2 LCD at I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

int x_pin = A0;              // Joystick X-axis pin
int y_pin = A1;              // Joystick Y-axis pin
int button_pin = 2;          // Pin for the start button (external button)

int led_up = 3;              // Pin for up direction LED
int led_down = 4;            // Pin for down direction LED
int led_left = 5;            // Pin for left direction LED
int led_right = 6;           // Pin for right direction LED

int score = 0;
long reactionTime = 0;
bool waitingForReaction = false;
long startTime = 0;
bool gameStarted = false;
bool gameOver = false;
bool buttonPressed = false;  // Track button state for debouncing

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);
  pinMode(button_pin, INPUT_PULLUP);  // Button with internal pull-up
  pinMode(led_up, OUTPUT);
  pinMode(led_down, OUTPUT);
  pinMode(led_left, OUTPUT);
  pinMode(led_right, OUTPUT);

  showStartMenu();
}

void loop() {
  if (!gameStarted && !gameOver) {
    // Wait for the player to press the button to start the game
    handleButtonPress();  // Check for button press
  } else if (gameOver) {
    handleGameOver();  // Handle game over state
  } else {
    playRound();  // Play the game round
  }
}

void showStartMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press Start");
  lcd.setCursor(0, 1);
  lcd.print("To Play");
}

void startGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready!");
  delay(2000);  // Delay before starting the first round
  score = 0;  // Reset the score
}

void playRound() {
  // Reset LEDs
  turnOffAllLEDs();

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

  // Wait for the correct joystick movement or incorrect guess
  while (waitingForReaction) {
    int x_data = analogRead(x_pin);
    int y_data = analogRead(y_pin);

    // Correct direction guess
    if (chosenLED == 1 && y_data <= 100) {  // Up direction
      recordReactionTime();
    } else if (chosenLED == 2 && y_data >= 650) {  // Down direction
      recordReactionTime();
    } else if (chosenLED == 3 && x_data <= 100) {  // Left direction
      recordReactionTime();
    } else if (chosenLED == 4 && x_data >= 650) {  // Right direction
      recordReactionTime();
    }
    
    // Wrong guess triggers a game over
    else if ((chosenLED == 1 && (y_data >= 650 || x_data <= 100 || x_data >= 650)) ||  // Up LED
             (chosenLED == 2 && (y_data <= 100 || x_data <= 100 || x_data >= 650)) ||  // Down LED
             (chosenLED == 3 && (y_data <= 100 || y_data >= 650 || x_data >= 650)) ||  // Left LED
             (chosenLED == 4 && (y_data <= 100 || y_data >= 650 || x_data <= 100))) {  // Right LED
      gameOver = true;  // Set gameOver flag
      waitingForReaction = false;  // Stop waiting for reaction
    }
  }

  //all LEDs are off after the player guesses correctly
  turnOffAllLEDs();

  // Show "Get Ready!"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready!");
  delay(100);  // Delay to show "Get Ready!" message
}

void recordReactionTime() {
  // Calculate reaction time
  reactionTime = millis() - startTime;

  // Update score based on reaction time (adjusted as per new system)
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

  // Ensure LED is turned off immediately after a correct guess
  turnOffAllLEDs();
  
  // Delay to allow the player to see the score and reaction time
  delay(2000);
}


void handleGameOver() {
  // Turn off all LEDs
  turnOffAllLEDs();

  // Display "Game Over" and the score
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  delay(4000);  // Wait for 3 seconds to show the message

  // Display "Start Over?" message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start Over?");

  // Wait for the button press to restart the game
  while (true) {
    handleButtonPress();  // Check for button press
    if (buttonPressed) {
      buttonPressed = false;  // Reset button state
      restartGame();  // Restart the game
      break;  // Break the loop after restarting
    }
  }
}

void restartGame() {
  // Reset game flags and variables
  gameStarted = false;
  gameOver = false;
  score = 0;

  // Show start menu again
  showStartMenu();
}

void handleButtonPress() {
  // Check for button press
  if (digitalRead(button_pin) == LOW && !buttonPressed) {
    delay(200);  // Debounce delay
    if (digitalRead(button_pin) == LOW) {  // Confirm press
      buttonPressed = true;  // Set button pressed state
      if (!gameStarted && !gameOver) {
        gameStarted = true;  // Start the game
        startGame();
      }
    }
  }
}

void turnOffAllLEDs() {
  // Turn off all LEDs
  digitalWrite(led_up, LOW);
  digitalWrite(led_down, LOW);
  digitalWrite(led_left, LOW);
  digitalWrite(led_right, LOW);
}