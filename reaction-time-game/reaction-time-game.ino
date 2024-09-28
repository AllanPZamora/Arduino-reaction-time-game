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

  // Determine joystick direction based on your reference values
  if (y_data <= 100) {  // Up direction (Y = 0)
    digitalWrite(led_up, HIGH);
  } else {
    digitalWrite(led_up, LOW);
  }

  if (y_data >= 650) {  // Down direction (Y = 670)
    digitalWrite(led_down, HIGH);
  } else {
    digitalWrite(led_down, LOW);
  }

  if (x_data <= 100) {  // Left direction (X = 0)
    digitalWrite(led_left, HIGH);
  } else {
    digitalWrite(led_left, LOW);
  }

  if (x_data >= 650) {  // Right direction (X = 670)
    digitalWrite(led_right, HIGH);
  } else {
    digitalWrite(led_right, LOW);
  }

  // Print joystick data to the Serial Monitor for debugging
  Serial.print("X: ");
  Serial.print(x_data);
  Serial.print("\tY: ");
  Serial.println(y_data);

  delay(100);
}
