#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define LEFT 4
#define RIGHT 2
#define SELECT 8

#define ROCK 1
#define PAPER 2
#define SCISSOR 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  Serial.begin(115200);

  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(SELECT, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.display();

  Serial.print("Starting game.");
}

void loop()
{
  int gameMode = selectGameMode();

  int p1 = selectMove(1);
  int p2 = (gameMode == 1) ? askAI() : selectMove(2);

  bool winner = checkWin(p1, p2);

  if (winner) {
    delay(5000);
    // restart game after delay
  }
}

void displayOLED(int textSize, int setX, int setY, char str[20]) {
    display.setFont(NULL);
    display.setTextSize(textSize);
    display.setTextColor(WHITE);
    display.setCursor(setX, setY);
    display.print(str);
    display.display();
}

void drawCircles()
{
  for(int16_t i = 5; i < 90; i += 5) {
    display.drawCircle(63,31,i,WHITE);
    display.display();
    delay(10);
    display.clearDisplay();
  }
}

int askAI()
{
  int aiMove = (rand()%3) + 1;
  Serial.print("AI chose ");
  Serial.println(aiMove);
  return aiMove;
}

int selectGameMode()
{
  display.clearDisplay();
  displayOLED(1, 20, 20, "1. Single Player.");
  displayOLED(1, 20, 30, "2. Two Players.");
  delay(1000);
  
  int mode = readButt();
  Serial.print("Selected game mode ");
  Serial.println(mode);
  
  return mode;
}

int readButt()
{
  int val = 0;

  while(1) {
    if (digitalRead(RIGHT)==HIGH) { val = 1; break; }
    if (digitalRead(LEFT)==HIGH) { val = 2; break; }
    if (digitalRead(SELECT)==HIGH) { val = 3; break; }
  }
  Serial.print("returning ");
  Serial.println(val);
  delay(500);
  return val;
}

int selectMove(int player)
{
  display.clearDisplay();
  char str[20];
  snprintf(str, sizeof(str), "Player %d move!", player);
  displayOLED(1, 20, 20, str);
  displayOLED(1, 20, 30, "Left: Rock | Right: Paper | Select: Scissors");

  int move = 0;

  while (move == 0) {
    if (digitalRead(LEFT) == HIGH) {
      move = ROCK;
      Serial.print("Player chose ");
      Serial.println("ROCK");

    } else if (digitalRead(RIGHT) == HIGH) {
      move = PAPER;
      Serial.print("Player chose ");
      Serial.println("PAPER");

    } else if (digitalRead(SELECT) == HIGH) {
      move = SCISSOR;
      Serial.print("Player chose ");
      Serial.println("SCISSOR");

    }
    delay(100);
  }

  delay(500);
  return move;
}

bool checkWin(int p1, int p2)
{
  bool winner = false;

  display.clearDisplay();

  if (p1 == p2) {
    displayOLED(1, 20, 20, "Tie!");
  }
  else if((p1 == ROCK && p2 == SCISSOR) ||
          (p1 == SCISSOR && p2 == PAPER) ||
          (p1 == PAPER && p2 == ROCK)) {
    displayOLED(1, 20, 20, "Player 1 Wins!");
    winner = true;
  } else {
    displayOLED(1, 20, 20, "Player 2 Wins!");
    winner = true;
  }

  Serial.print("Winner ");
  Serial.println(winner ? "TRUE" : "FALSE");

  delay(5000);
  return winner;
}

