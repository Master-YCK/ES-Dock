#ifndef LV_CONF_H
#define LV_CONF_H

/* Use 16-bit RGB565 colors, which is common for ESP32 LCD panels. */
#define LV_COLOR_DEPTH 16

/* Do not swap the byte order of RGB565 color values. */
#define LV_COLOR_16_SWAP 0

/* Enable LVGL logging. */
#define LV_USE_LOG 1

/* Output warning-level and more severe LVGL messages. */
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

/***** Development options *****/

/* Enable LVGL performance monitoring for future diagnostics. */
#define LV_USE_PERF_MONITOR 1

/* Enable LVGL memory monitoring for future diagnostics. */
#define LV_USE_MEM_MONITOR 1

#endif /* LV_CONF_H */