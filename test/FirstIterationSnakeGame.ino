#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SELECT 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SNAKE_MAX_LENGTH 12
int snake_x[SNAKE_MAX_LENGTH];
int snake_y[SNAKE_MAX_LENGTH];
int snake_length = 2;
int score = 0;
int snake_dir = 0; // 0 = right, 1 = up, 2 = left, 3 = down
int food_x;
int food_y;

unsigned long lastMove = 0;
const unsigned long moveInterval = 200;

void setup() {
  pinMode(SELECT, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  randomSeed(analogRead(0));
  food_x = random(0, SCREEN_WIDTH / 8);
  food_y = random(0, SCREEN_HEIGHT / 8);

  snake_x[0] = 0;
  snake_y[0] = 0;
  snake_x[1] = 1;
  snake_y[1] = 0;
}

void loop() {
  if (digitalRead(SELECT) == LOW) {
    // Turn left
    snake_dir = (snake_dir + 1) % 4;
    delay(200); // Simple debounce
  }

  if (millis() - lastMove > moveInterval) {
    lastMove = millis();

    // Move body
    for (int i = snake_length - 1; i > 0; i--) {
      snake_x[i] = snake_x[i - 1];
      snake_y[i] = snake_y[i - 1];
    }

    // Move head
    switch (snake_dir) {
      case 0: snake_x[0]++; break; // Right
      case 1: snake_y[0]--; break; // Up
      case 2: snake_x[0]--; break; // Left
      case 3: snake_y[0]++; break; // Down
    }

    // Wrap around
    if (snake_x[0] < 0) snake_x[0] = SCREEN_WIDTH / 8 - 1;
    if (snake_x[0] >= SCREEN_WIDTH / 8) snake_x[0] = 0;
    if (snake_y[0] < 0) snake_y[0] = SCREEN_HEIGHT / 8 - 1;
    if (snake_y[0] >= SCREEN_HEIGHT / 8) snake_y[0] = 0;

    // Check food collision
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
      if (snake_length < SNAKE_MAX_LENGTH) snake_length++;
      score++;

      food_x = random(0, SCREEN_WIDTH / 8);
      food_y = random(0, SCREEN_HEIGHT / 8);
    }

    // Check self collision
    for (int i = 1; i < snake_length; i++) {
      if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
        gameOver();
        resetGame();
        break;
      }
    }

    // Draw everything
    display.clearDisplay();
    drawSnake();
    drawFood();
    display.display();
  }
}

void drawSnake() {
  for (int i = 0; i < snake_length; i++) {
    display.fillRect(snake_x[i] * 8, snake_y[i] * 8, 8, 8, WHITE);
  }
}

void drawFood() {
  display.fillRect(food_x * 8, food_y * 8, 8, 8, WHITE);
}

void gameOver() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("Game Over");
  display.setTextSize(1);
  display.setCursor(30, 45);
  display.print("Score: ");
  display.println(score);
  display.display();
  delay(3000);
}

void resetGame() {
  score = 0;
  snake_length = 2;
  snake_dir = 0;
  snake_x[0] = 0;
  snake_y[0] = 0;
  snake_x[1] = 1;
  snake_y[1] = 0;
  food_x = random(0, SCREEN_WIDTH / 8);
  food_y = random(0, SCREEN_HEIGHT / 8);
}
