#include <Arduino.h>
#include <EEPROM.h>
#include "Petuino.h"

Petuino::Petuino(bool _sleeping, int _sleep, int _happiness, int _hunger, int _age, int _expression){
  sleeping = _sleeping;
  sleep = _sleep;
  happiness = _happiness;
  hunger = _hunger;
  age = _age;
  expression = _expression;
}

// SLEEP/WAKE-UP FUNCTION
void Petuino::updateSleeping(){
  sleeping = !sleeping;
  EEPROM.write(0, sleeping);
}

// UPDATE STATS FUNCTIONS

void Petuino::updateSleep(int sleepVal){
  sleep += sleepVal;
  if(sleep < 0){
    sleep = 0;
  }else if(sleep > 24){
    sleep = 24;
  }
  EEPROM.write(1, sleep);
}

void Petuino::updateHappiness(int happinessVal){
  happiness += happinessVal;
  if(happiness < 0){
    happiness = 0;
  }else if(happiness > 24){
    happiness = 24;
  }
  EEPROM.write(2, happiness);
}

void Petuino::updateHunger(int hungerVal){
  hunger += hungerVal;
  if(hunger < 0){
    hunger = 0;
  }else if(hunger > 24){
    hunger = 24;
  }
  EEPROM.write(3, hunger);
}

// UPDATE APPEARANCE FUNCTIONS

void Petuino::updateAge(){
  age++;
  EEPROM.put(10, age);
}

void Petuino::updateExpression(int newExpression){
  expression = newExpression;
  EEPROM.write(5, expression);
}

// MAP VALUES TO PIXELS

int Petuino::mapValue(int val, int barLength){
  int statLength = (val * barLength-2) / 24;
  return statLength;
}
