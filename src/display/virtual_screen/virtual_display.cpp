// src/display/virtual_screen/virtual_display.cpp
#include "virtual_screen/virtual_display.hpp"
#include <esp_heap_caps.h>
#include <Wire.h>

uint8_t *VirtualDisplay::s_framebuffer = nullptr;
lv_display_t *VirtualDisplay::s_display = nullptr;
lv_indev_t *VirtualDisplay::s_indev = nullptr;
static Arduino_GFX *s_hardwareDisplay = nullptr;
static TwoWire *s_touchBus = nullptr;
static uint8_t s_touchAddress = 0;

int16_t VirtualDisplay::s_touchX = 0;
int16_t VirtualDisplay::s_touchY = 0;
bool VirtualDisplay::s_isPressed = false;

bool VirtualDisplay::init()
{
    lv_init();

    // Allocate the full-screen buffer in PSRAM when available; otherwise use internal heap memory.
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

    // Create a 174x640 virtual display.
    s_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_buffers(s_display, s_framebuffer, nullptr, FRAME_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(s_display, flushCallback);

    // Create a virtual pointer input device.
    s_indev = lv_indev_create();
    lv_indev_set_type(s_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(s_indev, inputReadCallback);

    Serial.println("[Display] Virtual display and pointer initialized successfully");
    return true;
}

void VirtualDisplay::setHardwareDisplay(Arduino_GFX *display)
{
    s_hardwareDisplay = display;
}

void VirtualDisplay::setTouchBus(TwoWire *bus, uint8_t address)
{
    s_touchBus = bus;
    s_touchAddress = address;
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
    if (s_hardwareDisplay)
    {
        const int16_t width = area->x2 - area->x1 + 1;
        const int16_t height = area->y2 - area->y1 + 1;
        s_hardwareDisplay->draw16bitRGBBitmap(
            area->x1, area->y1, reinterpret_cast<uint16_t *>(px_map), width, height);
    }

    lv_display_flush_ready(disp);
}

void VirtualDisplay::inputReadCallback(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (s_touchBus && s_touchAddress != 0)
    {
        static const uint8_t request[] = {
            0xB5, 0xAB, 0xA5, 0x5A, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00};

        s_touchBus->beginTransmission(s_touchAddress);
        s_touchBus->write(request, sizeof(request));
        const uint8_t writeError = s_touchBus->endTransmission();

        if (writeError == 0 && s_touchBus->requestFrom(s_touchAddress, (uint8_t)8) == 8)
        {
            uint8_t response[8];
            for (uint8_t i = 0; i < sizeof(response); ++i)
                response[i] = s_touchBus->read();

            if (response[1] == 1)
            {
                s_touchX = ((response[2] & 0x0F) << 8) | response[3];
                s_touchY = ((response[4] & 0x0F) << 8) | response[5];
                s_touchX = constrain(s_touchX, 0, LCD_WIDTH - 1);
                s_touchY = constrain(s_touchY, 0, LCD_HEIGHT - 1);
                s_isPressed = true;
            }
            else if (response[1] == 0)
            {
                s_isPressed = false;
            }
        }
    }

    data->point.x = s_touchX;
    data->point.y = s_touchY;
    data->state = s_isPressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}