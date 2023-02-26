// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD,
    TD_QUAD_TAP,
    TD_QUAD_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance enums
enum {
    TD_A,
    TD_E,
    TD_I,
    TD_O,
    TD_U,
    SOME_OTHER_DANCE
};

td_state_t cur_dance(qk_tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void o_finished(qk_tap_dance_state_t *state, void *user_data);
void o_reset(qk_tap_dance_state_t *state, void *user_data);

// for sending f13 when holding down thumbkey no 6 (delete)
enum custom_keycodes {
    LEFT_F13 = SAFE_RANGE,
    RIGHT_F13
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t my_hash_timer;
    switch (keycode) {

        case RIGHT_F13:
            if( record -> event.pressed) {
                my_hash_timer = timer_read();
                register_code(KC_F13);
            } else {
                unregister_code(KC_F13);
                if (timer_elapsed(my_hash_timer) < TAPPING_TERM) {
                    SEND_STRING(SS_TAP(X_DOT));
                }
            }
        return false;

        case LEFT_F13:
            if( record -> event.pressed) {
                my_hash_timer = timer_read();
                register_code(KC_F13);
            } else {
                unregister_code(KC_F13);
                if (timer_elapsed(my_hash_timer) < TAPPING_TERM) {
                    SEND_STRING(SS_TAP(X_X));
                }
            }
        return false;

    //  case LT(0, KC_W):
    //     if (record->tap.count && record->event.pressed) {

    //         register_code(KC_F13);
    //         tap_code16(KC_W);

    //         unregister_code(KC_F13);
    //         return false;
    //     }

    }
    return true;
}

enum layers { MIRYOKU_LAYER_NAMES };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE]   = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_BASE),
  [NAV]    = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_NAV),
  [MOUSE]  = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_MOUSE),
  [MEDIA]  = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_MEDIA),
  [NUM]    = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_NUM),
  [SYM]    = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_SYM),
  [FUN]    = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_FUN),
  [BUTTON] = U_MACRO_VA_ARGS(LAYOUT_miryoku, MIRYOKU_LAYER_BUTTON)
};

#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_caps[] = {LT(NAV, KC_SPC), LT(NUM, KC_BSPC), COMBO_END};
// const uint16_t PROGMEM thumbcombos_base_right[] = {LT(SYM, KC_ENT), LT(NUM, KC_BSPC), COMBO_END};
// const uint16_t PROGMEM thumbcombos_base_left[] = {LT(NAV, KC_SPC), LT(MOUSE, KC_TAB), COMBO_END};
// const uint16_t PROGMEM thumbcombos_nav[] = {KC_ENT, KC_BSPC, COMBO_END};
// const uint16_t PROGMEM thumbcombos_mouse[] = {KC_BTN1, KC_BTN3, COMBO_END};
// const uint16_t PROGMEM thumbcombos_media[] = {KC_MSTP, KC_MPLY, COMBO_END};
// const uint16_t PROGMEM thumbcombos_num[] = {KC_0, KC_MINS, COMBO_END};
//   #if defined (MIRYOKU_LAYERS_FLIP)
// const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
//   #else
// const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
//   #endif
// const uint16_t PROGMEM thumbcombos_fun[] = {KC_SPC, KC_TAB, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
  COMBO(thumbcombos_caps, KC_F13)
//   COMBO(thumbcombos_base_right, LT(FUN, KC_DEL)),
//   COMBO(thumbcombos_base_left, LT(MEDIA, KC_ESC)),
//   COMBO(thumbcombos_nav, KC_DEL),
//   COMBO(thumbcombos_mouse, KC_BTN2),
//   COMBO(thumbcombos_media, KC_MUTE),
//   COMBO(thumbcombos_num, KC_DOT),
//   #if defined (MIRYOKU_LAYERS_FLIP)
//   COMBO(thumbcombos_sym, KC_RPRN),
//   #else
//   COMBO(thumbcombos_sym, KC_LPRN),
//   #endif
//   COMBO(thumbcombos_fun, KC_APP)

};
#endif


// TAP DANCE FOR ACCENTED CHARACTERS
td_state_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    }

    if (state->count == 4) {
        if (state->interrupted || !state->pressed) return TD_QUAD_TAP;
        else return TD_QUAD_HOLD;
    }

    else return TD_UNKNOWN;
}

// Create an instance of 'td_tap_t' for the 'x' tap dance.
static td_tap_t xtap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void a_finished(qk_tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_A); break;
        case TD_SINGLE_HOLD: register_code(KC_RSFT); break;
        case TD_DOUBLE_TAP: register_code(KC_QUOT); break;

        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_A); register_code(KC_A); break;
        default: break;
    }
}

void a_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_A); break;
        case TD_SINGLE_HOLD: unregister_code(KC_RSFT); break;
        case TD_DOUBLE_TAP: unregister_code(KC_QUOT); break;

        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_A); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}

void e_finished(qk_tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_E); break;
        case TD_DOUBLE_TAP: register_code(KC_SCLN); break;

        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_E); register_code(KC_E); break;
        default: break;
    }
}

void e_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_E); break;
        case TD_DOUBLE_TAP: unregister_code(KC_SCLN); break;

        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_E); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}

void i_finished(qk_tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_I); break;
        case TD_DOUBLE_TAP: register_code(KC_GRV); break;

        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_I); register_code(KC_I); break;
        default: break;
    }
}

void i_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_I); break;
        case TD_DOUBLE_TAP: unregister_code(KC_GRV); break;

        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_I); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}

void o_finished(qk_tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_O); break;
        case TD_DOUBLE_TAP: register_code(KC_EQL); break;
        case TD_TRIPLE_TAP: register_code(KC_0); break;
        case TD_QUAD_TAP: register_code(KC_LBRC); break;

        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_O); register_code(KC_O); break;
        default: break;
    }
}

void o_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_O); break;
        case TD_DOUBLE_TAP: unregister_code(KC_EQL); break;
        case TD_TRIPLE_TAP: unregister_code(KC_0); break;
        case TD_QUAD_TAP: unregister_code(KC_LBRC); break;
        // case TD_DOUBLE_HOLD: unregister_code(KC_LALT); break;
        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_O); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}

void u_finished(qk_tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_U); break;
        case TD_DOUBLE_TAP: register_code(KC_RBRC); break;
        case TD_TRIPLE_TAP: register_code(KC_MINS); break;
        case TD_QUAD_TAP: register_code(KC_BSLS); break;

        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_U); register_code(KC_U); break;
        default: break;
    }
}

void u_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_U); break;
        case TD_DOUBLE_TAP: unregister_code(KC_RBRC); break;
        case TD_TRIPLE_TAP: unregister_code(KC_MINS); break;
        case TD_QUAD_TAP: unregister_code(KC_BSLS); break;

        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_U); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_A] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, a_finished, a_reset),
    [TD_E] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, e_finished, e_reset),
    [TD_I] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, i_finished, i_reset),
    [TD_O] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, o_finished, o_reset),
    [TD_U] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, u_finished, u_reset)
};


// OLED CUSTOMIZATIONS

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  } else {
      return OLED_ROTATION_270;
  }
  return rotation;
}

#define BASE 0
#define NAV (1<<NAV)
#define MOUSE (1<<MOUSE)
#define MEDIA (1<<MEDIA)
#define NUM (1<<NUM)
#define SYM (1<<SYM)
#define FUN (1<<FUN)

void oled_render_layer_state(void) {
    oled_write_P(PSTR("\n\n\n\n\n\n\n"), false);
    switch (layer_state) {
        case BASE:
            oled_write_ln_P(PSTR("BASE "), false);
            break;
        case NAV:
            oled_write_ln_P(PSTR("NAV  "), false);
            break;
        case MOUSE:
            oled_write_ln_P(PSTR("MOUSE"), false);
            break;
        case MEDIA:
            oled_write_ln_P(PSTR("MEDIA"), false);
            break;
        case NUM:
            oled_write_ln_P(PSTR("NUM  "), false);
            break;
        case SYM:
            oled_write_ln_P(PSTR("SYM  "), false);
            break;
        case FUN:
            oled_write_ln_P(PSTR("FUN  "), false);
            break;
    }
    oled_write_P(PSTR("layer"), false);
}

static void oled_render_logo(void) {
    static const char PROGMEM qmk_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x00
    };

    oled_write_P(qmk_logo, false);
}

void oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
    } else {
        oled_render_logo();
    }
}
