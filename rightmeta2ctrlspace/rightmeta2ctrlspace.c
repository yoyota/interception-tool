#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

// clang-format off
const struct input_event
CTRL_UP    = {.type = EV_KEY, .code = KEY_LEFTCTRL, .value = 0},
CTRL_DOWN  = {.type = EV_KEY, .code = KEY_LEFTCTRL, .value = 1},
SPACE_UP   = {.type = EV_KEY, .code = KEY_SPACE,    .value = 0},
SPACE_DOWN = {.type = EV_KEY, .code = KEY_SPACE,    .value = 1};
// clang-format on

__s32 PREV_CODE;

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

void rightmeta_to_ctrl_space(const struct input_event input) {
    if (!does_keyboard_changes(input)) {
        return;
    }
    write_event(&input);

    if (input.value == 1) {
        PREV_CODE = input.code;
    }

    if (PREV_CODE == KEY_RIGHTMETA && input.code == KEY_RIGHTMETA &&
        input.value == 0) {
        write_event(&CTRL_DOWN);
        write_event(&SPACE_DOWN);
        write_event(&SPACE_UP);
        write_event(&CTRL_UP);
    }
}

int main(void) {
    struct input_event input;
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        rightmeta_to_ctrl_space(input);
    }
}
