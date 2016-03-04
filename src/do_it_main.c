#include <pebble.h>

#define MAX_QUOTE_LENGTH 7

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_motivational_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static char* getQuote() {
  static char quote[MAX_QUOTE_LENGTH] = { "DO MORE"};
  return quote;
}

/* 
 * Initialise the elements of the main window
 */
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_motivational_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(20, 20), bounds.size.w, bounds.size.h-50));

  // Set layout parameters for motivational layer
  text_layer_set_background_color(s_motivational_layer, GColorClear);
  text_layer_set_text_color(s_motivational_layer, GColorBlack);
  text_layer_set_text(s_motivational_layer, getQuote());
  text_layer_set_overflow_mode(s_motivational_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(s_motivational_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_motivational_layer, GTextAlignmentCenter);

  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(bounds.size.h-60, bounds.size.h-40), 
            bounds.size.w, bounds.size.h-20));

  // Set layout parameters for time_layer
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_motivational_layer));
}

/* 
 * Destroy the elements of the main window
 */
static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
}


/* 
 * Initialise the watchface
 */
static void init() {
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();
}

/* 
 * Destroy the watchface
 */
static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}