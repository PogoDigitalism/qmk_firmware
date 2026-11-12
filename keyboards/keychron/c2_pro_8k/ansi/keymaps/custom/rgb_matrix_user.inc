RGB_MATRIX_EFFECT(WHITE_RED_SPLASH)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

extern uint32_t hit_timestamps[LED_HITS_TO_REMEMBER];

// static uint8_t hash_timestamp_to_hue(uint32_t ts) {
//     ts ^= ts >> 16;
//     ts *= 0x85ebca6b;
//     ts ^= ts >> 13;
//     return (uint8_t)(ts & 0xFF);
// }

static hsv_t WHITE_RED_SPLASH_math(hsv_t hsv, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick) {
    uint8_t max_radius = 180;

    if (dist > max_radius) {
        return hsv;
    }

    int16_t delta = (int16_t)tick - (int16_t)dist;
    uint8_t wave_width = 25;
    
    if (delta >= 0 && delta < wave_width) {
        uint8_t wave_intensity = 255 - ((delta * 255) / wave_width);

        uint8_t distance_fade = 255 - ((dist * 255) / max_radius);
        uint8_t total_redness = scale8(wave_intensity, distance_fade);

        total_redness = scale8(total_redness, 255);

        hsv.h = 0;
        hsv.s = qadd8(hsv.s, total_redness);
        //hsv.s = 255;
        //hsv.v = qadd8(hsv.v, total_redness);
    }

    return hsv;
}

#define SPLASH_SPEED ((uint8_t) 50)
bool WHITE_RED_SPLASH(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint8_t count = g_last_hit_tracker.count;
    //uint8_t offset = LED_HITS_TO_REMEMBER - count;
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        hsv_t hsv = {0, 0, rgb_matrix_config.hsv.v};
        for (uint8_t j = 0; j < count; j++) {
            int16_t dx = g_led_config.point[i].x - g_last_hit_tracker.x[j];
            int16_t dy = g_led_config.point[i].y - g_last_hit_tracker.y[j];
            uint8_t dist = sqrt16(dx * dx + dy * dy);
            uint16_t tick = scale16by8(g_last_hit_tracker.tick[j], SPLASH_SPEED);
            
            hsv = WHITE_RED_SPLASH_math(hsv, dx, dy, dist, tick);
            //uint8_t hit_hue = hash_timestamp_to_hue(hit_timestamps[offset + j]);
            //hsv.h = hit_hue;
        }

        rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_region_set_color(params->region, i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS