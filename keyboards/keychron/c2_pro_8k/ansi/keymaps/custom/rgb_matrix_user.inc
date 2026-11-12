RGB_MATRIX_EFFECT(BLOOD_FILL)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static hsv_t BLOOD_FILL_math(hsv_t hsv, uint8_t i, uint8_t time) {
    hsv.h = g_led_config.point[i].x - time;
    return hsv;
}

static bool BLOOD_FILL(effect_params_t* params) {
    return effect_runner_i(params, &BLOOD_FILL_math);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS