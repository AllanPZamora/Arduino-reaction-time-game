#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (16x2 LCD at I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

int x_pin = A0;              // Joystick X-axis pin
int y_pin = A1;              // Joystick Y-axis pin
int start_button_pin = 2;    // Pin for the start button (external button)

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

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Set joystick pins as inputs
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);

  // Set start button pin as input with pull-up
  pinMode(start_button_pin, INPUT_PULLUP);  // Using INPUT_PULLUP for the external button

  // Set LED pins as outputs
  pinMode(led_up, OUTPUT);
  pinMode(led_down, OUTPUT);
  pinMode(led_left, OUTPUT);
  pinMode(led_right, OUTPUT);

  // Show the start menu
  showStartMenu();
}

void loop() {
  if (!gameStarted && !gameOver) {
    // Wait for the player to press the button to start the game
    if (digitalRead(start_button_pin) == LOW) {  // Button pressed (LOW due to INPUT_PULLUP)
      delay(200);  // Debounce delay
      if (digitalRead(start_button_pin) == LOW) {  // Check again after the delay to confirm the press
        gameStarted = true;
        startGame();
      }
    }
  } else if (gameOver) {
    // Wait for the player to press the button to start over
    if (digitalRead(start_button_pin) == LOW) {
      delay(200);  // Debounce delay
      if (digitalRead(start_button_pin) == LOW) {
        restartGame();
      }
    }
  } else {
    playRound();
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
  // Clear LCD and prepare for the game
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready!");
  delay(2000);  // Delay before starting the first round

  score = 0;  // Reset the score
}

void playRound() {
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

  // Wait for the correct joystick movement or incorrect guess
  while (waitingForReaction) {
    int x_data = analogRead(x_pin);
    int y_data = analogRead(y_pin);

    // Correct direction guess
    if (chosenLED == 1 && y_data <= 100) {  // Up direction
      recordReactionTime(chosenLED);
    } else if (chosenLED == 2 && y_data >= 650) {  // Down direction
      recordReactionTime(chosenLED);
    } else if (chosenLED == 3 && x_data <= 100) {  // Left direction
      recordReactionTime(chosenLED);
    } else if (chosenLED == 4 && x_data >= 650) {  // Right direction
      recordReactionTime(chosenLED);
    }
    
    // Wrong guess triggers a game over
    else if ((chosenLED == 1 && (y_data >= 650 || x_data <= 100 || x_data >= 650)) ||  // Up LED, but joystick moved down, left, or right
             (chosenLED == 2 && (y_data <= 100 || x_data <= 100 || x_data >= 650)) ||  // Down LED, but joystick moved up, left, or right
             (chosenLED == 3 && (y_data <= 100 || y_data >= 650 || x_data >= 650)) ||  // Left LED, but joystick moved up, down, or right
             (chosenLED == 4 && (y_data <= 100 || y_data >= 650 || x_data <= 100))) {  // Right LED, but joystick moved up, down, or left
      gameOverSequence();
    }
  }

  // Ensure all LEDs are off after the player guesses correctly
  turnOffAllLEDs();

  // After a correct guess, display "Get Ready" for the next round
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready!");
  delay(2000);  // Delay to give the player time before the next round
}

void recordReactionTime(int chosenLED) {
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

  // Ensure LED is turned off immediately after a correct guess
  turnOffAllLEDs();

  // Delay to allow the player to see the score and reaction time
  delay(2000);
}

void gameOverSequence() {
  // Set gameOver flag to true
  gameOver = true;

  // Turn off all LEDs
  turnOffAllLEDs();

  // Display "Wrong guess!" and the score
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wrong guess!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);

  delay(3000);  // Wait for 3 seconds to show the message

  // Display "Start Over?" message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start Over?");
  lcd.setCursor(0, 1);
  lcd.print("Press Button");
}

void restartGame() {
  // Reset game flags and variables
  gameStarted = false;
  gameOver = false;
  score = 0;

  // Show start menu again
  showStartMenu();
}

void turnOffAllLEDs() {
  // Turn off all LEDs
  digitalWrite(led_up, LOW);
  digitalWrite(led_down, LOW);
  digitalWrite(led_left, LOW);
  digitalWrite(led_right, LOW);
}
