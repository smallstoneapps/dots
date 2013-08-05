#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "config.h"

#if ROCKSHOT
#include "http.h"
#include "httpcapture.h"
#endif

#define MY_UUID { 0x63, 0x77, 0xDC, 0xE6, 0xDB, 0x4D, 0x4E, 0x52, 0x89, 0xF3, 0x70, 0xDC, 0xCC, 0x05, 0x20, 0x82 }

PBL_APP_INFO(MY_UUID, "Dots", "Matthew Tole", 1, 0, RESOURCE_ID_MENU_ICON, APP_INFO_WATCH_FACE);

#if INVERT_COLORS
#define COLOR_FOREGROUND GColorBlack
#define COLOR_BACKGROUND GColorWhite
#else
#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack
#endif

Window window;
Layer layer_hours;
Layer layer_minutes;
Layer layer_seconds;
HeapBitmap bmp_dot, bmp_dot_small;

void layer_hours_update_callback(Layer* me, GContext* ctx);
void layer_minutes_update_callback(Layer* me, GContext* ctx);
void layer_seconds_update_callback(Layer* me, GContext* ctx);
void handle_init(AppContextRef ctx);
void handle_tick(AppContextRef ctx, PebbleTickEvent *t);
void window_load(Window *me);
void window_unload(Window *me);
void draw_cell(GContext* ctx, int x, int y, bool small);
void load_bitmaps();
void unload_bitmaps();

#if ROCKSHOT
void  http_success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx);
#endif

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };

  #if ROCKSHOT
  handlers.messaging_info = (PebbleAppMessagingInfo) {
    .buffer_sizes = {
      .inbound = 124,
      .outbound = 124,
    },
  };
  http_capture_main(&handlers);
  #endif

  app_event_loop(params, &handlers);
}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Dots Watchface");
  window_stack_push(&window, true);
  window_set_window_handlers(&window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload
  });
  window_set_background_color(&window, COLOR_BACKGROUND);

  resource_init_current_app(&APP_RESOURCES);

  #if ROCKSHOT
  http_set_app_id(15);
  http_register_callbacks((HTTPCallbacks) {
    .success = http_success
  }, NULL);
  http_capture_init(ctx);
  #endif

  layer_init(&layer_hours, GRect(0, 0, 144, 28));
  layer_hours.update_proc = &layer_hours_update_callback;
  layer_add_child(&window.layer, &layer_hours);

  layer_init(&layer_minutes, GRect(0, 28, 144, 70));
  layer_minutes.update_proc = &layer_minutes_update_callback;
  layer_add_child(&window.layer, &layer_minutes);

  layer_init(&layer_seconds, GRect(0, 98, 144, 70));
  layer_seconds.update_proc = &layer_seconds_update_callback;
  layer_add_child(&window.layer, &layer_seconds);

}

void handle_tick(AppContextRef ctx, PebbleTickEvent *t) {

  PblTm now;
  get_time(&now);

  layer_mark_dirty(&layer_seconds);
  if (now.tm_sec == 0) {
    layer_mark_dirty(&layer_minutes);
  }
  if (now.tm_min == 0) {
    layer_mark_dirty(&layer_hours);
  }
}

void window_load(Window *me) {
  load_bitmaps();
}

void window_unload(Window *me) {
  unload_bitmaps();
}

void load_bitmaps() {
  #if INVERT_COLORS
  heap_bitmap_init(&bmp_dot, RESOURCE_ID_DOT_BLACK);
  heap_bitmap_init(&bmp_dot_small, RESOURCE_ID_DOT_SMALL_BLACK);
  #else
  heap_bitmap_init(&bmp_dot, RESOURCE_ID_DOT_WHITE);
  heap_bitmap_init(&bmp_dot_small, RESOURCE_ID_DOT_SMALL_WHITE);
  #endif
}

void unload_bitmaps() {
  heap_bitmap_deinit(&bmp_dot);
}

void layer_hours_update_callback(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);

  PblTm now;
  get_time(&now);

  int x_pos = 1;
  for (int x = 0; x < 12; x += 1) {
    int y_pos = 1;
    for (int y = 0; y < 2; y += 1) {
      draw_cell(ctx, x_pos, y_pos, now.tm_hour <= (y * 12 + x));
      y_pos += 14;
    }
    x_pos += 12;
  }

}

void layer_minutes_update_callback(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);

  PblTm now;
  get_time(&now);

  int x_pos = 1;
  for (int x = 0; x < 12; x += 1) {
    int y_pos = 1;
    for (int y = 0; y < 5; y += 1) {
      draw_cell(ctx, x_pos, y_pos, now.tm_min <= (y * 12 + x));
      y_pos += 14;
    }
    x_pos += 12;
  }

}

void layer_seconds_update_callback(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);

  PblTm now;
  get_time(&now);

  int x_pos = 1;
  for (int x = 0; x < 12; x += 1) {
    int y_pos = 1;
    for (int y = 0; y < 5; y += 1) {
      draw_cell(ctx, x_pos, y_pos, now.tm_sec <= (y * 12 + x));
      y_pos += 14;
    }
    x_pos += 12;
  }

}

void draw_cell(GContext* ctx, int x, int y, bool small) {
  if (small) {
    graphics_draw_bitmap_in_rect(ctx, &bmp_dot_small.bmp, GRect(x, y, 12, 14));
  }
  else {
    graphics_draw_bitmap_in_rect(ctx, &bmp_dot.bmp, GRect(x, y, 12, 14));
  }
}

#if ROCKSHOT
void http_success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx) {}
#endif