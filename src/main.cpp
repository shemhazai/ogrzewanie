#include <Arduino.h>

void setUp();
void doSomething();

int main() {
  setUp();

  while (true)
    doSomething();

  return 0;
}

void setUp() {
  pinMode(1, OUTPUT);
}

void doSomething() {

}
