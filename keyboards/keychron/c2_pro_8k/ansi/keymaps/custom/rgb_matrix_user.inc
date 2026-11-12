RGB_MATRIX_EFFECT(WHITE_RED_SPLASH)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static hsv_t WHITE_RED_SPLASH_math(hsv_t hsv, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick) {
    // 1. Control how far the wave travels (smaller number = tighter radius around the key)
    uint8_t max_radius = 65; 
    if (dist > max_radius) {
        return hsv;
    }

    // 2. Calculate the wave ring using the difference between time (tick) and distance (dist)
    int16_t delta = (int16_t)tick - (int16_t)dist;
    
    // Wave thickness (how wide the red ring is)
    uint8_t wave_width = 25; 

    if (delta >= 0 && delta < wave_width) {
        // Create a smooth wave peak (0 at edges of the ring, 255 at the center of the ring)
        uint8_t wave_intensity = 255 - ((delta * 255) / wave_width);

        // Fade the wave out smoothly as it approaches max_radius
        uint8_t distance_fade = 255 - ((dist * 255) / max_radius);
        uint8_t total_redness = scale8(wave_intensity, distance_fade);

        // 3. Make it "translucent" (cap max saturation at ~180 instead of 255 so white bleeds through)
        total_redness = scale8(total_redness, 180);

        // Blend saturation upward toward red (Hue = 0)
        hsv.h = 0; 
        hsv.s = qadd8(hsv.s, total_redness);
    }

    return hsv;
}

#define SPLASH_SPEED ((uint8_t) 50)
bool WHITE_RED_SPLASH(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t count = g_last_hit_tracker.count;
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        // Start every LED as pure white (Hue = 0, Saturation = 0, Full Value)
        hsv_t hsv = {0, 0, rgb_matrix_config.hsv.v};

        // Loop through all recent keypresses (Multi-splash behavior)
        for (uint8_t j = 0; j < count; j++) {
            int16_t dx = g_led_config.point[i].x - g_last_hit_tracker.x[j];
            int16_t dy = g_led_config.point[i].y - g_last_hit_tracker.y[j];
            uint8_t dist = sqrt16(dx * dx + dy * dy);
            uint16_t tick = scale16by8(g_last_hit_tracker.tick[j], SPLASH_SPEED);
            
            hsv = WHITE_RED_SPLASH_math(hsv, dx, dy, dist, tick);
        }

        rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_region_set_color(params->region, i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
