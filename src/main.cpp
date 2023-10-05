#include <Arduino.h>
#include <lidar.hpp>

void setup() {
  //Serial.begin(115200);
  lidar_t *L = new lidar_t("Micro_ROS","Mi10","12345678","192.168.235.59",8888,0);
}

void loop() 
{
  // Serial.printf("AAA");
  delay(1000);
}

