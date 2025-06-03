#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUTTON_PIN 4
#define BUZZER_PIN 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
}
  
void loop() {
  int button_state = digitalRead(BUTTON_PIN);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("...");
  if (button_state == HIGH) {
    display.clearDisplay();
    display.println("BEEP!");
    tone(BUZZER_PIN, 440);
  } else {
    noTone(BUZZER_PIN);
  }
  display.display();
}