#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x63, 0x77, 0xDC, 0xE6, 0xDB, 0x4D, 0x4E, 0x52, 0x89, 0xF3, 0x70, 0xDC, 0xCC, 0x05, 0x20, 0x82 }

PBL_APP_INFO(MY_UUID, "Dots", "Matthew Tole", 1, 0, DEFAULT_MENU_ICON, APP_INFO_WATCH_FACE);

// Commented out because inverted colours is gross.
#define INVERT_COLORS
#ifndef INVERT_COLORS
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

void layer_hours_update_callback(Layer* me, GContext* ctx);
void layer_minutes_update_callback(Layer* me, GContext* ctx);
void layer_seconds_update_callback(Layer* me, GContext* ctx);
void handle_init(AppContextRef ctx);
void handle_tick(AppContextRef ctx, PebbleTickEvent *t);
void draw_cell(GContext* ctx, int x, int y, bool small);

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Dots Watchface");
  window_stack_push(&window, true);
  window_set_background_color(&window, COLOR_BACKGROUND);

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
  GRect cell;
  if (small) {
    cell.size = GSize(5, 6);
    cell.origin = GPoint(x + 3, y + 3);
  }
  else {
    cell.size = GSize(10, 12);
    cell.origin = GPoint(x, y);
  }
  graphics_fill_rect(ctx, cell, small ? 2 : 4, GCornersAll);
}