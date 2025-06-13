#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUTTON_PIN 4
#define BUZZER_PIN 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Game variables
int gameState = 0; // 0=waiting, 1=player chose, 2=show result
int playerChoice = 0; // 0=none, 1=rock, 2=paper, 3=scissors
int petChoice = 0;
unsigned long gameStartTime;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
  
  randomSeed(analogRead(0)); // Seed random number generator
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  
  if (gameState == 0) {
    // Waiting for player to press button
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("Press Button");
    display.setTextSize(1);
    display.setCursor(10, 40);
    display.println("to play RPS!");
    
    if (buttonState == HIGH) {
      gameState = 1;
      playerChoice = 0;
      gameStartTime = millis();
      tone(BUZZER_PIN, 523, 200); // Play a tone
    }
  } 
  else if (gameState == 1) {
    // Player choosing
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 5);
    display.println("Quick! Choose:");
    
    display.setCursor(20, 25);
    display.println("Hold: Rock");
    display.setCursor(20, 35);
    display.println("Release: Paper");
    display.setCursor(20, 45);
    display.println("Wait: Scissors");
    
    if (buttonState == HIGH) {
      playerChoice = 1; // Rock (button held)
    } 
    else if (millis() - gameStartTime < 2000) {
      playerChoice = 2; // Paper (released quickly)
    } 
    else {
      playerChoice = 3; // Scissors (waited too long)
    }
    
    if (playerChoice != 0 && millis() - gameStartTime > 500) {
      petChoice = random(1, 4); // Pet chooses randomly
      gameState = 2;
      gameStartTime = millis();
    }
  } 
  else if (gameState == 2) {
    // Show result
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 5);
    display.print("You: ");
    display.println(getChoiceName(playerChoice));
    display.setCursor(10, 20);
    display.print("Pet: ");
    display.println(getChoiceName(petChoice));
    
    display.setTextSize(2);
    display.setCursor(10, 40);
    
    // Determine winner
    if (playerChoice == petChoice) {
      display.println("Tie!");
      tone(BUZZER_PIN, 392, 300);
    } 
    else if ((playerChoice == 1 && petChoice == 3) || 
             (playerChoice == 2 && petChoice == 1) || 
             (playerChoice == 3 && petChoice == 2)) {
      display.println("You Win!");
      tone(BUZZER_PIN, 659, 500);
    } 
    else {
      display.println("You Lose!");
      tone(BUZZER_PIN, 262, 700);
    }
    
    if (millis() - gameStartTime > 3000) {
      gameState = 0;
    }
  }
  
  display.display();
  delay(50);
}

String getChoiceName(int choice) {
  switch (choice) {
    case 1: return "Rock";
    case 2: return "Paper";
    case 3: return "Scissors";
    default: return "";
  }
}