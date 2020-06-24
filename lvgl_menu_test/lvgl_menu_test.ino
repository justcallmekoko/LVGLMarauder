#include <WiFi.h>

#include "MenuFunctions.h"

MenuFunctions menu_obj;

void setup() {
  Serial.begin(115200);
  
  menu_obj.RunSetup();
}

void loop() {
  lv_task_handler(); /* let the GUI do its work */
  delay(5);
}
