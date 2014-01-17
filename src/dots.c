#include <pebble.h>
#include "libs/bitmap-loader/bitmap-loader.h"
#include "libs/pebble-assist/pebble-assist.h"

#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack

static Window* window;
static Layer* layer_hours;
static Layer* layer_minutes;
static Layer* layer_seconds;

static void layer_hours_update(Layer* me, GContext* ctx);
static void layer_minutes_update(Layer* me, GContext* ctx);
static void layer_seconds_update(Layer* me, GContext* ctx);
static void init(void);
static void deinit(void);
static void handle_tick(struct tm* tick_time, TimeUnits units_changed);
static void window_load(Window *me);
static void window_unload(Window *me);
static void draw_cell(GContext* ctx, int x, int y, bool small);

int main(void) {
  init();
  app_event_loop();
  deinit();
}

void init() {

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_set_background_color(window, COLOR_BACKGROUND);

  window_stack_push(window, true);
  bitmaps_init();
}

void deinit(void) {

}

static void handle_tick(struct tm* tick_time, TimeUnits units_changed) {
  if (units_changed | MINUTE_UNIT) {
    layer_mark_dirty(layer_minutes);
  }
  if (units_changed | HOUR_UNIT) {
    layer_mark_dirty(layer_hours);
  }
  layer_mark_dirty(layer_seconds);
}

static void window_load(Window *me) {
  layer_hours = layer_create(GRect(0, 0, 144, 28));
  layer_set_update_proc(layer_hours, layer_hours_update);
  layer_add_to_window(layer_hours, window);

  layer_minutes = layer_create(GRect(0, 28, 144, 70));
  layer_set_update_proc(layer_minutes, layer_minutes_update);
  layer_add_to_window(layer_minutes, window);

  layer_seconds = layer_create(GRect(0, 98, 144, 70));
  layer_set_update_proc(layer_seconds, layer_seconds_update);
  layer_add_to_window(layer_seconds, window);

  tick_timer_service_subscribe(SECOND_UNIT, &handle_tick);
}

void window_unload(Window *me) {
  layer_destroy(layer_hours);
  layer_destroy(layer_minutes);
  layer_destroy(layer_seconds);
}

void layer_hours_update(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);

  time_t now = time(NULL);
  struct tm *time_now = localtime(&now);

  int x_pos = 1;
  for (int x = 0; x < 12; x += 1) {
    int y_pos = 1;
    for (int y = 0; y < 2; y += 1) {
      draw_cell(ctx, x_pos, y_pos, time_now->tm_hour <= (y * 12 + x));
      y_pos += 14;
    }
    x_pos += 12;
  }

}

void layer_minutes_update(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);

  time_t now = time(NULL);
  struct tm *time_now = localtime(&now);

  int x_pos = 1;
  for (int x = 0; x < 12; x += 1) {
    int y_pos = 1;
    for (int y = 0; y < 5; y += 1) {
      draw_cell(ctx, x_pos, y_pos, time_now->tm_min <= (y * 12 + x));
      y_pos += 14;
    }
    x_pos += 12;
  }

}

void layer_seconds_update(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);

  time_t now = time(NULL);
  struct tm *time_now = localtime(&now);

  int x_pos = 1;
  for (int x = 0; x < 12; x += 1) {
    int y_pos = 1;
    for (int y = 0; y < 5; y += 1) {
      draw_cell(ctx, x_pos, y_pos, time_now->tm_sec <= (y * 12 + x));
      y_pos += 14;
    }
    x_pos += 12;
  }

}

void draw_cell(GContext* ctx, int x, int y, bool small) {
  if (small) {
    graphics_draw_bitmap_in_rect(ctx, bitmaps_get_bitmap(RESOURCE_ID_DOT_SMALL_WHITE), GRect(x, y, 12, 14));
  }
  else {
    graphics_draw_bitmap_in_rect(ctx, bitmaps_get_bitmap(RESOURCE_ID_DOT_WHITE), GRect(x, y, 12, 14));
  }
}