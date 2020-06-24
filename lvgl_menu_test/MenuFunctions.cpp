#include "MenuFunctions.h"

lv_obj_t * slider_label;
lv_obj_t * ta1;
lv_obj_t * ta2;

Menu* current_menu;

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

MenuFunctions::MenuFunctions() {
  
}

void MenuFunctions::lv_tick_handler()
{
  lv_tick_inc(LVGL_TICK_PERIOD);
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  //extern TFT_eSPI tft;
  
  uint16_t c;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1));
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
  //extern TFT_eSPI tft;
  
  uint16_t touchX, touchY;

  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if(!touched)
  {
    return false;
  }

  if(touchX>SCREEN_WIDTH || touchY > SCREEN_HEIGHT)
  {
    Serial.println("Y or y outside of expected parameters..");
    Serial.print("y:");
    Serial.print(touchX);
    Serial.print(" x:");
    Serial.print(touchY);
  }
  else
  {

    data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 

    //if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);
   
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print("Data x");
    Serial.println(touchX);
    
    Serial.print("Data y");
    Serial.println(touchY);

  }

  return false;
}

void MenuFunctions::RunSetup() {
  // Need to declare new
  display_buffer = new LinkedList<String>();

  lv_init();
  
  tft.init();
  tft.setRotation(0); // Portrait

  tft.setCursor(0, 0);
  //tft.setFreeFont(&FreeMonoBold9pt7b);

  // Calibration data
  //uint16_t calData[5] = { 390, 3516, 253, 3520, 7 }; tft.setRotation(1); // Portrait

  // Make sure you define which one you are using
  #ifdef TFT_SHIELD
    uint16_t calData[5] = { 275, 3494, 361, 3528, 4 }; // tft.setRotation(0); // Portrait with TFT Shield
    Serial.println("Using TFT Shield");
  #else if defined(TFT_DIY)
    uint16_t calData[5] = { 339, 3470, 237, 3438, 2 }; // tft.setRotation(0); // Portrait with DIY TFT
    Serial.println("Using TFT DIY");
  #endif
  tft.setTouch(calData);

  //tft.fillScreen(TFT_BLACK);
  //clearScreen();

  Serial.println("SPI_FREQUENCY: " + (String)SPI_FREQUENCY);
  Serial.println("SPI_READ_FREQUENCY: " + (String)SPI_READ_FREQUENCY);
  Serial.println("SPI_TOUCH_FREQUENCY: " + (String)SPI_TOUCH_FREQUENCY);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
  indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
  lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  // root menu stuff
  mainMenu.list = new LinkedList<MenuNode>(); // Get list in first menu ready

  // Main menu stuff
  wifiMenu.list = new LinkedList<MenuNode>(); // Get list in second menu ready
  bluetoothMenu.list = new LinkedList<MenuNode>(); // Get list in third menu ready
  generalMenu.list = new LinkedList<MenuNode>();
  deviceMenu.list = new LinkedList<MenuNode>();

  // WiFi menu stuff
  wifiSnifferMenu.list = new LinkedList<MenuNode>();

  mainMenu.name = " ESP32 Marauder ";
  wifiMenu.name = "WiFi";
  bluetoothMenu.name = "Bluetooth";

  wifiSnifferMenu.name = "WiFi Sniffers";

  // Main Menu
  mainMenu.parentMenu = NULL;
  addNodes(&mainMenu, "WiFi", TFT_GREEN, NULL, 0, [this](){changeMenu(&wifiMenu);});
  addNodes(&mainMenu, "Bluetooth", TFT_CYAN, NULL, 0, [this](){changeMenu(&bluetoothMenu);});
  addNodes(&mainMenu, "Chicken", TFT_CYAN, NULL, 0, [this](){event_handler1;});
  addNodes(&mainMenu, "Sloths", TFT_CYAN, NULL, 0, [this](){event_handler1;});
  addNodes(&mainMenu, "Anime", TFT_CYAN, NULL, 0, [this](){event_handler1;});
  addNodes(&mainMenu, "Nani?", TFT_CYAN, NULL, 0, [this](){event_handler1;});
  addNodes(&mainMenu, "Just some good stuff", TFT_CYAN, NULL, 0, [this](){event_handler1;});
  addNodes(&mainMenu, "Legitness", TFT_CYAN, NULL, 0, [this](){event_handler1;});
  addNodes(&mainMenu, "Boiii", TFT_CYAN, NULL, 0, [this](){event_handler1;});

  // Build WiFi Menu
  wifiMenu.parentMenu = &mainMenu; // Main Menu is second menu parent
  addNodes(&wifiMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(wifiMenu.parentMenu);});
  addNodes(&wifiMenu, "Sniffers", TFT_BLACK, NULL, 1, [this](){changeMenu(&wifiSnifferMenu);});
  
  // Build Bluetooth Menu
  bluetoothMenu.parentMenu = &mainMenu; // Second Menu is third menu parent
  addNodes(&bluetoothMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(bluetoothMenu.parentMenu);});

  // WiFi Sniffer menu
  wifiSnifferMenu.parentMenu = &wifiMenu;
  addNodes(&wifiSnifferMenu, "Back", TFT_LIGHTGREY, NULL, 0, [this](){changeMenu(wifiSnifferMenu.parentMenu);});

  changeMenu(&mainMenu);

}

// Function to add MenuNodes to a menu
void MenuFunctions::addNodes(Menu* menu, String name, uint16_t color, Menu* child, int place, std::function<void()> callable)
{
  lv_obj_t new_button;
  menu->list->add(MenuNode{name, color, place, &new_button, callable});
  //strcpy(menu->list->get(-1).icon, bluetooth_icon);
}

void MenuFunctions::changeMenu(Menu* menu) {
  current_menu = menu;

  buildButtons(menu);
}

// Function to build a scrollable list object and populate it with buttons
void MenuFunctions::buildButtons(Menu* menu) {
  if (menu->list != NULL) {

    // Create a scroll
    // Not quite sure how to use the layout function
    // LV_LAYOUT_PRETTY was not found
    //lv_page_set_scrl_layout(lv_scr_act(), LV_LAYOUT_PRETTY);

    // Create a list
    lv_obj_t * list = lv_list_create(lv_scr_act(), NULL);

    // Set size of list
    lv_obj_set_size(list, lv_obj_get_width(lv_scr_act())-20, lv_obj_get_height(lv_scr_act())-20);

    // Align list
    lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);

    // Clean list before adding any buttons
    // This is not needed because I thought I was going to make the list 
    // a variable of the MenuFunctions class...but it wouldn't work
    lv_list_clean(list);

    // Add buttons to list
    for (int i = 0; i < menu->list->size(); i++) {
      lv_obj_t * label;
      lv_obj_t * new_button;
      
      // Of course convert a string to char array
      char buf[menu->list->get(i).name.length() + 1] = {};
      menu->list->get(i).name.toCharArray(buf, menu->list->get(i).name.length() + 1);

      // Add button to list
      lv_obj_t * btn = lv_list_add_btn(list, NULL, buf);

      // Set the callback
      //lv_obj_set_event_cb(btn, the_thang(menu->list->get(i).name));
      lv_obj_set_event_cb(btn, the_thang);
      //lv_obj_set_event_cb(btn, event_handler1);
      
      //new_button = lv_btn_create(lv_scr_act(), NULL);
      //lv_obj_set_event_cb(new_button, event_handler1);
      //lv_obj_align(new_button, NULL, LV_ALIGN_IN_TOP_MID, 0, (40 * i) + (i * 5));

      //label = lv_label_create(new_button, NULL);
      //lv_label_set_text(label, buf);
    }
  }
}

// Not a very descriptive name, but I wrote this at 2300 on a work night
void MenuFunctions::the_thang(lv_obj_t *obj, lv_event_t event){
  //Serial.println("Button clickened: " + throat);  
  String obj_name = lv_list_get_btn_text(obj);
  if (event == LV_EVENT_CLICKED) {
    Serial.print("\n" + obj_name);

    // Iterate through all of the menu nodes in the current menu
    for (int i = 0; i < current_menu->list->size(); i++) {
      
      // This is where the magic of menu traversal using the lvgl library happens
      // Run the callable function of the node with the name of the list button that was clicked
      if (obj_name == current_menu->list->get(i).name) {
        Serial.print(" -> " + current_menu->list->get(i).name + "\n");
        current_menu->list->get(i).callable();
      }
    }
  }
}

// Really this is just a placeholder function for now
// All list button clicks will either run changeMenu to traverse menus
// or they will run an actual function like a wifi or bluetooth application
void MenuFunctions::event_handler1(lv_obj_t * obj, lv_event_t event) {
  Serial.println(lv_btn_get_state(obj));
  if(event == LV_EVENT_CLICKED) {
    printf("Clicked1\n");
  }
  else if(event == LV_EVENT_VALUE_CHANGED) {
    printf("Toggled1\n");
  }
}
