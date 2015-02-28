#include <pebble.h>

#define DATA_LOG_TAG_ACCEL 1

static Window *window;
static TextLayer *text_layer;
static bool measuring;
static DataLoggingSessionRef log_ref_x;
static DataLoggingSessionRef log_ref_y;
static DataLoggingSessionRef log_ref_z;

static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  
  if (measuring) {
    /*DataLoggingResult res_x = data_logging_log(log_ref_x, &data[0].x, num_samples);
    DataLoggingResult res_y = data_logging_log(log_ref_y, &data[0].y, num_samples);
    DataLoggingResult res_z = data_logging_log(log_ref_z, &data[0].z, num_samples);*/
    data_logging_log(log_ref_x, &data[0].x, num_samples);
    
    data_logging_log(log_ref_y, &data[0].y, num_samples);
    data_logging_log(log_ref_z, &data[0].z, num_samples);
    APP_LOG(APP_LOG_LEVEL_INFO, "N X\tY\tZ\n0 %d\t%d\t%d", 
      data[0].x, data[0].y, data[0].z, 
      );
  } else {
    text_layer_set_text(text_layer, "No logging");
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    // Freeze shock display
    measuring = false;
    text_layer_set_text(text_layer, "Measuring...");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    // Resume shock display
    measuring = true;
    text_layer_set_text(text_layer, "Inactive");
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    text_layer = text_layer_create((GRect) { .origin = { 0, 30 }, .size = { bounds.size.w, 20 } });
    layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
}

static void init(void) {
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
            });
    accel_data_service_subscribe(1, accel_data_handler);
    log_ref_x = data_logging_create(DATA_LOG_TAG_ACCEL, DATA_LOGGING_INT, sizeof(int), true);
    log_ref_y = data_logging_create(DATA_LOG_TAG_ACCEL, DATA_LOGGING_INT, sizeof(int), true);
    log_ref_z = data_logging_create(DATA_LOG_TAG_ACCEL, DATA_LOGGING_INT, sizeof(int), true);
    const bool animated = true;
    window_stack_push(window, animated);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
