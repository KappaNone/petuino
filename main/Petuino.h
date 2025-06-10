#ifndef Petuino_h
#define Petuino_h

#include <Arduino.h>

class Petuino
{
  public:
  Petuino(bool _sleeping, int _sleep, int _happiness, int _hunger, int _age, int _expression);

  bool sleeping; // address: 0
  
  int sleep; // address: 1
  int happiness; // address: 2
  int hunger; // address: 3

  int age; // address: 4
  int expression; // address: 5


  void updateSleeping();

  void updateSleep(int sleepVal);
  void updateHappiness(int happinessVal);
  void updateHunger(int hungerVal);

  void updateAge();
  
  void updateExpression(int newExpression);

  int mapValue(int val, int barLength);
};

#endif
