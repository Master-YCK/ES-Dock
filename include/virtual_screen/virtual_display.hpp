// src/virtual_display.hpp
#pragma once
#include <Arduino.h>
#include <lvgl.h>

constexpr uint16_t LCD_WIDTH = 174;
constexpr uint16_t LCD_HEIGHT = 640;
constexpr size_t FRAME_BUFFER_SIZE = LCD_WIDTH * LCD_HEIGHT * sizeof(lv_color16_t); // 222,720 bytes

class VirtualDisplay {
public:
    static bool init();
    static uint8_t* getFramebuffer();
    static void updateRemotePointer(int16_t x, int16_t y, bool pressed);

private:
    static void flushCallback(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);
    static void inputReadCallback(lv_indev_t* indev, lv_indev_data_t* data);

    static uint8_t* s_framebuffer;
    static lv_display_t* s_display;
    static lv_indev_t* s_indev;

    static int16_t s_touchX;
    static int16_t s_touchY;
    static bool s_isPressed;
};