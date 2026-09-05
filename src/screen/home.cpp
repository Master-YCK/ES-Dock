// src/screen/home.cpp
#include "virtual_screen/home.hpp"
#include <Arduino.h>

static int s_clickCount = 0;
static lv_obj_t *s_btnLabel = nullptr;

static void btnEventHandler(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        s_clickCount++;
        char buf[32];
        snprintf(buf, sizeof(buf), "Tapped: %d", s_clickCount);
        lv_label_set_text(s_btnLabel, buf);
        Serial.printf("[UI] Button clicked, count=%d\n", s_clickCount);
    }
}

void createDemoUI()
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), LV_PART_MAIN);

    // Use a vertical flexbox layout.
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(screen, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_row(screen, 12, LV_PART_MAIN);

    // 1. Title
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "ESP32-S3\n174x640");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // 2. Interactive test button
    lv_obj_t *btn = lv_button_create(screen);
    lv_obj_set_size(btn, 150, 44);
    lv_obj_add_event_cb(btn, btnEventHandler, LV_EVENT_CLICKED, nullptr);
    s_btnLabel = lv_label_create(btn);
    lv_label_set_text(s_btnLabel, "Tap Me");
    lv_obj_center(s_btnLabel);

    // 3. Slider control
    lv_obj_t *slider = lv_slider_create(screen);
    lv_obj_set_size(slider, 140, 12);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);

    // 4. Vertically scrollable list
    lv_obj_t *list = lv_list_create(screen);
    lv_obj_set_size(list, 158, 420);
    lv_obj_set_style_bg_color(list, lv_color_hex(0x1F1F1F), LV_PART_MAIN);
    lv_obj_set_style_border_width(list, 0, LV_PART_MAIN);

    for (int i = 1; i <= 15; i++)
    {
        char itemText[32];
        snprintf(itemText, sizeof(itemText), "Item #%02d", i);
        lv_list_add_button(list, nullptr, itemText);
    }
}