// src/display/virtual_screen/virtual_display.cpp
#include "virtual_screen/virtual_display.hpp"
#include <esp_heap_caps.h>

uint8_t *VirtualDisplay::s_framebuffer = nullptr;
lv_display_t *VirtualDisplay::s_display = nullptr;
lv_indev_t *VirtualDisplay::s_indev = nullptr;

int16_t VirtualDisplay::s_touchX = 0;
int16_t VirtualDisplay::s_touchY = 0;
bool VirtualDisplay::s_isPressed = false;

bool VirtualDisplay::init()
{
    lv_init();

    // 優先在 PSRAM 分配 217.5 KiB 的全螢幕 buffer；若無 PSRAM 則退回一般內部堆疊
    if (psramFound())
    {
        s_framebuffer = (uint8_t *)heap_caps_malloc(FRAME_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    }
    else
    {
        s_framebuffer = (uint8_t *)malloc(FRAME_BUFFER_SIZE);
    }

    if (!s_framebuffer)
    {
        Serial.println("[Display] Failed to allocate framebuffer memory!");
        return false;
    }
    memset(s_framebuffer, 0, FRAME_BUFFER_SIZE);

    // 建立 174x640 虛擬顯示器
    s_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_buffers(s_display, s_framebuffer, nullptr, FRAME_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(s_display, flushCallback);

    // 建立虛擬觸控裝置
    s_indev = lv_indev_create();
    lv_indev_set_type(s_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(s_indev, inputReadCallback);

    Serial.println("[Display] Virtual display and pointer initialized successfully");
    return true;
}

uint8_t *VirtualDisplay::getFramebuffer()
{
    return s_framebuffer;
}

void VirtualDisplay::updateRemotePointer(int16_t x, int16_t y, bool pressed)
{
    s_touchX = constrain(x, 0, LCD_WIDTH - 1);
    s_touchY = constrain(y, 0, LCD_HEIGHT - 1);
    s_isPressed = pressed;
}

void VirtualDisplay::flushCallback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    // 記憶體中已有最新畫面，此處直接通知 LVGL 渲染管線已就緒
    lv_display_flush_ready(disp);
}

void VirtualDisplay::inputReadCallback(lv_indev_t *indev, lv_indev_data_t *data)
{
    data->point.x = s_touchX;
    data->point.y = s_touchY;
    data->state = s_isPressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}