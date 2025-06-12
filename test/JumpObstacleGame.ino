#include <Adafruit_SSD1306.h>

#define BUTTON_PIN 8  // SELECT button pin

// Variables
bool isJumping = false;
bool gameOver = false;
int petY = 40;
int velocity = 0;
const int gravity = 2;
const int groundY = 40;
int obstacleX1 = 128;
int obstacleX2 = 180;
int gameSpeed = 3;
unsigned long lastSpeedIncrease = 0;
unsigned long lastFrame = 0;
unsigned long score = 0;

bool buttonPressed = false;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);  

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (1);
    }

    display.clearDisplay();
    display.display();
    randomSeed(analogRead(0));  // Randomize obstacle spawn
}

void loop() {
    unsigned long currentTime = millis();
    if (currentTime - lastFrame < 50) return;
    lastFrame = currentTime;

    int buttonState = digitalRead(BUTTON_PIN);

    // --- GAME OVER & RESTART HANDLING ---
    if (gameOver) {
        if (buttonState == HIGH && !buttonPressed) {
            buttonPressed = true;
        }

        if (buttonState == LOW && buttonPressed) {
            buttonPressed = false;
            resetGame();
        }

        return;
    }

    // --- JUMP HANDLING ---
    if (buttonState == HIGH && !buttonPressed && petY == groundY) {
        buttonPressed = true;
        isJumping = true;
        velocity = -13;
    }
    if (buttonState == LOW) {
        buttonPressed = false;
    }

    // --- PHYSICS ---
    if (isJumping) {
        petY += velocity;
        velocity += gravity;
        if (petY >= groundY) {
            petY = groundY;
            isJumping = false;
        }
    }

    // --- MOVE OBSTACLES ---
    obstacleX1 -= gameSpeed;
    obstacleX2 -= gameSpeed;

    if (obstacleX1 < -10) {
        obstacleX1 = 128 + random(0, 40);
        if (random(0, 10) > 6) obstacleX2 = obstacleX1 + random(15, 30);
    }
    if (obstacleX2 < -10) {
        obstacleX2 = 128 + random(30, 60);
    }

    // --- SPEED UP ---
    if (currentTime - lastSpeedIncrease > 3000) {
        gameSpeed++;
        lastSpeedIncrease = currentTime;
    }

    // --- SCORE ---
    score++;

    // --- DRAW SCENE ---
    display.clearDisplay();
    display.fillRect(10, petY, 10, 10, SSD1306_WHITE);        // Pet
    display.fillRect(obstacleX1, groundY, 10, 15, SSD1306_WHITE); // Obstacle 1
    display.fillRect(obstacleX2, groundY, 10, 15, SSD1306_WHITE); // Obstacle 2
    display.drawLine(0, 58, 128, 58, SSD1306_WHITE);              // Ground
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Score: ");
    display.print(score / 10);
    display.display();

    // --- COLLISION DETECTION ---
    bool hitObstacle1 = (obstacleX1 < 20 && obstacleX1 > 5 && petY == groundY);
    bool hitObstacle2 = (obstacleX2 < 20 && obstacleX2 > 5 && petY == groundY);

    if (hitObstacle1 || hitObstacle2) {
        gameOver = true;
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, 20);
        display.print("Game Over");
        display.setTextSize(1);
        display.setCursor(10, 45);
        display.print("Press to restart");
        display.display();
    }
}

// --- RESTART GAME FUNCTION ---
void resetGame() {
    isJumping = false;
    petY = groundY;
    velocity = 0;
    obstacleX1 = 128;
    obstacleX2 = 180;
    gameSpeed = 3;
    score = 0;
    gameOver = false;
    lastSpeedIncrease = millis();
    lastFrame = millis();
    display.clearDisplay();
    display.display();
}
