#include <Arduino.h>
#include <lidar.hpp>

void setup() {
  //Serial.begin(115200);
  lidar_t *L = new lidar_t(0);
}

void loop() 
{
  // Serial.printf("AAA");
  delay(1000);
}

