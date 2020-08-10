#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

__s32 PREV_CODE;
__s32 RIGHT_ALT_PRESSED;
const __s32 EV_KEYS[] = {KEY_J, KEY_K, KEY_L, KEY_SEMICOLON};
const size_t LENGTH   = sizeof(EV_KEYS) / sizeof(EV_KEYS[0]);
// const int LENGTH = 4;

// clang-format off
const struct input_event
RIGHTALT_UP       = {.type = EV_KEY, .code = KEY_RIGHTALT, .value = 0},
RIGHTALT_DOWN     = {.type = EV_KEY, .code = KEY_RIGHTALT,    .value = 1};
// clang-format on

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

int does_keyboard_changes(const struct input_event event) {
    if (event.type == EV_MSC && event.code == MSC_SCAN)
        return 0;

    if (event.type == EV_KEY) {
        return 1;
    }

    write_event(&event);
    return 0;
}

void check_right_alt(const struct input_event input) {
    if (input.code != KEY_RIGHTALT) {
        return;
    }

    if (input.value == 0) {
        RIGHT_ALT_PRESSED = 0;
    }

    if (input.value == 1) {
        RIGHT_ALT_PRESSED = 1;
    }
}

void hjkl_to_arrow(const struct input_event input) {
    __u16 key_code;
    switch (input.code) {
        case KEY_J:
            key_code = KEY_LEFT;
            break;
        case KEY_K:
            key_code = KEY_DOWN;
            break;
        case KEY_L:
            key_code = KEY_UP;
            break;
        case KEY_SEMICOLON:
            key_code = KEY_RIGHT;
            break;
        default:
            return;
    }

    const struct input_event modified_input = {
        .type = EV_KEY, .code = key_code, .value = input.value};

    write_event(&RIGHTALT_UP);
    write_event(&modified_input);
    write_event(&RIGHTALT_DOWN);
}

void convert_right_alt_plus_hjkl_to_arrow(const struct input_event input) {
    if (!does_keyboard_changes(input)) {
        return;
    }

    check_right_alt(input);

    for (int i = 0; i < LENGTH; i++) {
        if (RIGHT_ALT_PRESSED && EV_KEYS[i] == input.code) {
            hjkl_to_arrow(input);
            return;
        }
    }

    write_event(&input);
}

int main(void) {
    struct input_event input;
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        convert_right_alt_plus_hjkl_to_arrow(input);
    }
}
