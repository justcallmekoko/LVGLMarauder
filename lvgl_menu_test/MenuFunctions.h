#ifndef MenuFunctions_h
#define MenuFunctions_h

#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>
#include <LinkedList.h>
#include <functional>

#define TFT_SHIELD
//#define TFT_DIY

//Ticker tick; /* timer for interrupt handler */


// Keypad start position, key sizes and spacing
#define KEY_X 120 // Centre of key
#define KEY_Y 50
#define KEY_W 240 // Width and height
#define KEY_H 22
#define KEY_SPACING_X 0 // X and Y gap
#define KEY_SPACING_Y 1
#define KEY_TEXTSIZE 1   // Font size multiplier
#define ICON_W 22
#define ICON_H 22
#define BUTTON_PADDING 22
//#define BUTTON_ARRAY_LEN 5

#define LVGL_TICK_PERIOD 6

// Display defines
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

PROGMEM void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
PROGMEM bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);

PROGMEM static lv_disp_buf_t disp_buf;
PROGMEM static lv_color_t buf[LV_HOR_RES_MAX * 10];

PROGMEM static void ta_event_cb(lv_obj_t * ta, lv_event_t event);
PROGMEM static void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event);

// lvgl stuff
PROGMEM static lv_obj_t *kb;

struct Menu;

// Individual Nodes of a menu

struct MenuNode {
    String name;
    uint16_t color;
    int icon;
    //TFT_eSPI_Button* button;
    lv_obj_t * button;
    std::function<void()> callable;
};

// Full Menus
struct Menu {
    String name;
    LinkedList<MenuNode>* list;
    Menu                * parentMenu;
    //uint8_t               selected;
};

class MenuFunctions
{
  private:
    //Menu* current_menu;

    // Main menu stuff
    Menu mainMenu;

    Menu wifiMenu;
    Menu bluetoothMenu;
    Menu generalMenu;
    Menu deviceMenu;

    // WiFi menu stuff
    Menu wifiSnifferMenu;

    //lv_obj_t * list = lv_list_create(lv_scr_act(), NULL);

    void addNodes(Menu* menu, String name, uint16_t color, Menu* child, int place, std::function<void()> callable);
    static void event_handler1(lv_obj_t * obj, lv_event_t event);
    static void the_thang(lv_obj_t *obj, lv_event_t event);
    static void lv_tick_handler();
    
  public:
    MenuFunctions();

    Ticker tick;
    //TFT_eSPI tft = TFT_eSPI();

    LinkedList<String>* display_buffer;
    
    void buildButtons(Menu* menu);
    void changeMenu(Menu* menu);
    void displayCurrentMenu();
    void main(uint32_t currentTime);
    void RunSetup();
};


#endif
