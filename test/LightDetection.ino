#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUTTON_PIN 4
#define BUZZER_PIN 3

#define LDR_PIN A0 // Analog pin connected to the LDR voltage divider

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize Serial (optional for debugging)
  Serial.begin(9600);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // Stop here if display doesn't work
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("LDR Test");
  display.display();
  delay(1000);
}

void loop() {
  int lightLevel = analogRead(LDR_PIN);

  // Optional: print to Serial for debugging
  Serial.print("Light Level: ");
  Serial.println(lightLevel);

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LDR Test");
  display.setCursor(0, 16);
  display.print("Light: ");
  display.println(lightLevel);
  display.display();

  delay(500);
}
