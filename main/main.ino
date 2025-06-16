#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#include <EEPROM.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <CuteBuzzerSounds.h>

#include "Scrolling_menu.h"
#include "Petuino.h"
#include "Sprites.h"


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BUZZER_PIN 9
#define SWITCH_SCREEN 7
#define LEFT 4
#define SELECT 8
#define RIGHT 2
#define SERVO 10

// Create display instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
Servo myservo;

void setup() {

  Serial.begin(9600);
  Serial.println("BOOT");

  // Try to initialize the display at address 0x3C
  if (!display.begin( SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("I2C ERROR");
    while (true);
  }

  // Clear the buffer
  display.clearDisplay();
  display.display();

  delay(100);
  myservo.attach(SERVO);
  pinMode(SWITCH_SCREEN, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);

  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = 1<<WDP0 | 1<<WDP3;
  WDTCSR |= _BV(WDIE);

  cute.init(BUZZER_PIN);

  Serial.println("START");
  delay(1000);

  if (digitalRead(SWITCH_SCREEN) == LOW){
    displayAll();
    displayMenuFunc();
    displayStats();
    display.display();
    cute.play(S_CONNECTION);
  }
}

// TAMAGOTCHI STATES

int state = 0;

// 0 = Home
// 1 = Sleep
// 2 = Stats

char menuSections[][20] = {"Eat", "Stats", "Play", "Sleep"};

const unsigned char* menuIcons[] = {
  eat_icon,
  stats_icon,
  play_icon,
  sleep_icon
};

Scrolling_menu menu(58, 32, 4);
// Scrolling_menu menuDress(58, 32, 5);
char activeState[20] = "Home";

bool sleeping = EEPROM.read(0);
  
int sleep = EEPROM.read(1);
int happiness = EEPROM.read(2);
int hunger = EEPROM.read(3);

int var_type = 0;
int age = EEPROM.get(10, var_type);
int expression = EEPROM.read(5);

Petuino petuino(sleeping, sleep, happiness, hunger, age, expression);

int sec = 0;
bool pressed = false;
int frameCount = 0;
int spriteOffset = -1;


int sensorValue = 0;

int minVal = 1023;
int maxVal = 0;
int normalizedValue = 0;
void loop() {
  sensorValue = analogRead(A0);

  if (sensorValue < minVal) {
    minVal = sensorValue;
  }
  if (sensorValue > maxVal) {
    maxVal = sensorValue;
  }

  normalizedValue = map(sensorValue, minVal, maxVal, 0, 100);
  if (digitalRead(SWITCH_SCREEN) == HIGH){
    turnOff();
  }
  else{
    stateSelection();
    delay(10);
  }
  
}

void wiggleTail() {
  int pos;
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(2);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(2);
  }
}
 
void stateSelection() {
  if(state == 0){
    homeFunc();
  }else if (state == 1){
    sleepFunc();
  }else if (state == 2){
    statsFunc();    
  }
}

// STATES MANAGEMENT

void changeState(char section[20]){

  display.invertDisplay(true);
  delay(50);
  display.invertDisplay(false);
  
  Serial.println(section);
  if(strcmp(section, "Sleep") == 0){
    petuino.updateSleeping();
    petuino.updateHappiness(2);
    petuino.updateSleep(1);
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    state = 1;
  }else if(strcmp(section, "Play") == 0){
    petuino.updateHappiness(4);
    petuino.updateSleep(-2);
    petuino.updateHunger(-2);
    displayPlay();    
  }else if(strcmp(section, "Stats") == 0){
    displayAge();
    state = 2;
  }else if(strcmp(section, "Eat") == 0){
    petuino.updateHunger(5);
    petuino.updateHappiness(1);
    displayEat();
  }else if(strcmp(section, "Home") == 0){
    frameCount = 50;
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    displayMenuFunc();
    displayAll();
    state = 0;
  }
  for (int i = 0; i < 20; i++){
   activeState[i] = section[i]; 
  }
  displayStats();
}

// STATES FUNCTIONS

// HOME

void homeFunc() {
    
  if(pressed == false){
        
    if(digitalRead(SELECT) == HIGH){

      cute.play(S_CONNECTION);
      pressed = true;
      changeState(menuSections[menu.activeSection]);
      
    }else if (digitalRead(LEFT) == HIGH){

      cute.play(S_CONNECTION);
      menu.moveLeft();
      displayMenuFunc();
      pressed = true;

    }else if (digitalRead(RIGHT) == HIGH){

      cute.play(S_CONNECTION);
      menu.moveRight();
      displayMenuFunc();
      pressed = true;

    }
  }else if(digitalRead(SELECT) == LOW && digitalRead(LEFT) == LOW && digitalRead(RIGHT) == LOW){
    pressed = false;
  }
  displayAll(); 
  
}

// SLEEP

void sleepFunc() {

  if(!petuino.sleeping){
    changeState("Home");
  }
    
  if(pressed == false){
        
    if(digitalRead(SELECT) == HIGH){

      pressed = true;

      petuino.updateSleeping();
      petuino.updateHappiness(-3);
      changeState("Home");
      cute.play(S_FART1);       
      
    }
  }else if(digitalRead(SELECT) == LOW && digitalRead(LEFT) == LOW && digitalRead(RIGHT) == LOW){
    pressed = false;
  }
  displaySleep();

}

void statsFunc(){
  if(pressed == false){
        
    if(digitalRead(SELECT) == HIGH){

      pressed = true;
      changeState("Home");
      cute.play(S_CONNECTION);
      
    }
  }else if(digitalRead(SELECT) == LOW && digitalRead(LEFT) == LOW && digitalRead(RIGHT) == LOW){
    pressed = false;
  }  
}

// MAIN MENU

void displayMenuFunc(){
  int y = 17;
  int height = 16;

  display.fillRect(51, 17, 77, 47, SSD1306_BLACK);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  
  display.setTextSize(1);

  // PREV SECTION
  display.setCursor(menu.x + 19, y + 4);
  display.println(menuSections[menu.prevSection]);
  display.drawBitmap(menu.x, y + 3, menuIcons[menu.prevSection], 10, 10, 1);

  // ACTIVE SECTION
  y = y + height;
  display.drawRoundRect(menu.x - 5, y, 73, 16, 3, SSD1306_WHITE);
  display.setCursor(menu.x + 19, y + 4);
  display.println(menuSections[menu.activeSection]);
  display.drawBitmap(menu.x, y + 3, menuIcons[menu.activeSection], 10, 10, 1);

  // NEXT SECTION
  y = y + height;
  display.setCursor(menu.x + 19, y + 4);
  display.println(menuSections[menu.nextSection]);
  display.drawBitmap(menu.x, y + 3, menuIcons[menu.nextSection], 10, 10, 1);

  display.display();
}

// ALL SPRITES

void displayAll(){

  if(frameCount >= 50){
    display.drawRoundRect(0, 16, 50, 48, 5, SSD1306_WHITE);
    
    display.fillRect(10, 27, 32, 46, SSD1306_BLACK);
    
    display.drawBitmap(10, 28 + spriteOffset, myBitmapbody_01, 32, 46, SSD1306_WHITE);
  
    // display.drawBitmap(10, 18, feet_bg, 32, 46, SSD1306_BLACK);
    // display.drawBitmap(10, 18, feet, 32, 46, SSD1306_WHITE);

    if (normalizedValue > 50) {
      display.drawBitmap(10, 28 + spriteOffset, head, 32, 46, SSD1306_WHITE);
    } else {
      display.drawBitmap(10, 28 + spriteOffset, head_night, 32, 46, SSD1306_WHITE);
    }
      
    display.display();
    if(spriteOffset == -1){
      spriteOffset = -2;
    }else{   
      spriteOffset = -1;
    }
    frameCount = 0;
  }
  frameCount++;
}

// SLEEPING SPRITES

void displaySleep(){

  if(frameCount >= 50){
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(64 - spriteOffset, 33 - spriteOffset);
    display.println("Z");
    display.setCursor(59 - !spriteOffset, 46 - !spriteOffset);
    display.println("Z");
    display.setTextSize(2);
    display.setCursor(47 - spriteOffset, 26 - !spriteOffset);
    display.println("Z");
      
    display.display();
    spriteOffset = !spriteOffset;
    frameCount = 0;
  }
  frameCount++;
}

// EAT ANIMATION

void selectEatFrame(int x, int y, int n){
  if(n == 1){
    display.drawBitmap(x, y, eat_anim_head_n_1, 32, 46, SSD1306_WHITE);
  }else{
    display.drawBitmap(x, y, eat_anim_head_n_2, 32, 46, SSD1306_WHITE);
  }    
}

void displayEat(){

  int startX = 30;
  int startY = 20;

  display.fillRect(0, 16, 128, 48, SSD1306_BLACK); // Clean screen

  // Frame 1
  selectEatFrame(startX,startY,1);
  display.drawBitmap(startX + 50, startY + 15, eat_anim_food_1, 15, 15, SSD1306_WHITE);
  display.display();

  delay(500);

  // Frame 2
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,2); 
  display.display();

  delay(500);

  // Frame 3
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  display.fillRect(45 + startX, 16, 55 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,1);
  display.drawBitmap(startX + 50, startY + 15, eat_anim_food_2, 15, 15, SSD1306_WHITE);
  display.display();

  delay(500);

  // Frame 4
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,2);
  display.display();

  delay(500);

  // Frame 5
  display.fillRect(0, 17, 128, 48, SSD1306_BLACK);
  display.fillRect(45 + startX, 16, 55 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,1);
  display.drawBitmap(startX + 50, startY + 15, eat_anim_food_3, 15, 15, SSD1306_WHITE);
  display.display();

  delay(500);

  // Frame 6
  display.fillRect(0, 16, 45 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,2);
  display.display();

  delay(500);

  // Frame 7
  display.fillRect(0, 17, 128, 48, SSD1306_BLACK);
  display.fillRect(45 + startX, 16, 55 + startX, 48, SSD1306_BLACK);
  selectEatFrame(startX,startY,1);
  display.display();

  delay(500);
  cute.play(S_JUMP);
  delay(500);

  changeState("Home");
  
}

// PLAY ANIMATION

void displayPlay(){
  int startX = 50;
  int startY = 17;
  int loop = 2;
  
  for (int i = 0; i <= loop; i++) {
    // Frame 1
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK); // Clear screen 
    display.drawBitmap(startX, startY, play_anim_1, 27, 48, SSD1306_WHITE);
    display.display(); 
    delay(200);

    // Frame 2
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_2, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);

    // Frame 3
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_3, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);

    // Frame 4
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_4, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);

    // Frame 5
    display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
    display.drawBitmap(startX, startY, play_anim_5, 27, 48, SSD1306_WHITE);
    display.display();
    delay(200);
  }
  
  delay(500);
  cute.play(S_JUMP);
  delay(500);

  changeState("Home");
  wiggleTail();
}

// DISPLAY AGE

void displayAge(){
  display.fillRect(0, 16, 128, 48, SSD1306_BLACK);

  display.setTextSize(1);
  
  display.setCursor(10,20);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("Name");
  display.setTextColor(SSD1306_WHITE);
  display.println(" kittyk");
  
  display.setCursor(10,32);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("Age");
  display.setTextColor(SSD1306_WHITE);
  display.print(" ");
  display.print(petuino.age/24);
  display.println(" days");

  display.setCursor(10,44);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.print("Weight");
  display.setTextColor(SSD1306_WHITE);
  display.println(" 4 kg");
}


// STATS DISPLAY

void displayStats(){

  display.fillRect(1, 1, 127, 15, SSD1306_BLACK);
  display.setTextSize(1);

  // HAPPINESS STATS
  display.setCursor(2,5);
  display.println("H");
  display.drawRoundRect(9, 6, 27, 6, 1, SSD1306_WHITE);
  int H = petuino.mapValue(petuino.happiness, 27);
  display.fillRect(10, 7, H, 4, SSD1306_WHITE);

  // SLEEP STATS
  display.setCursor(43,5);
  display.println("S");
  display.drawRoundRect(50, 6, 27, 6, 1, SSD1306_WHITE);
  int S = petuino.mapValue(petuino.sleep, 27);
  display.fillRect(51, 7, S, 4, SSD1306_WHITE);

  // HUNGER/FOOD STATS
  display.setCursor(85,5);
  display.println("F");
  display.drawRoundRect(92, 6, 27, 6, 1, SSD1306_WHITE);
  int F = petuino.mapValue(petuino.hunger, 27);
  display.fillRect(93, 7, F, 4, SSD1306_WHITE);
  
  display.display();
}

// SLEEP FUNCTIONS
int hour = 450;
ISR (WDT_vect) {
  
    sec ++; // sec = 450 --> an hour has passed
    Serial.print("sec: ");
    Serial.println(sec);
    
    if(sec > hour){
      sec = 0;
      
      petuino.updateAge();
      Serial.println(petuino.age);
      
      petuino.updateHappiness(-1);
      petuino.updateHunger(-1);
      if(petuino.sleeping){
        petuino.updateSleep(3);
      }else{
        petuino.updateSleep(-1);
      }
      
    }
}

void turnOn() {
  Serial.println("TURN ON");
  delay(2000);
}

void turnOff() {
  display.clearDisplay();
  display.display();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(2), turnOn, LOW);
  sleep_mode();

  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));

  // Every hour if state is low it rings to get attention
  // Only if the screen switch is off
  if((petuino.sleep == 0 || petuino.hunger == 0 || petuino.happiness == 0) && sec >= hour && digitalRead(SWITCH_SCREEN) == HIGH){
    cute.play(S_CONFUSED);
  }

  
  if (digitalRead(SWITCH_SCREEN) == LOW){
    if(state == 0){
      frameCount = 50;
      displayAll();
      displayMenuFunc();
    }
    
    displayStats();
    display.display();
    cute.play(S_CONNECTION);
  }
}
