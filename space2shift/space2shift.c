#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

// clang-format off
const struct input_event
shift_up       = {.type = EV_KEY, .code = KEY_LEFTSHIFT, .value = 0},
shift_down     = {.type = EV_KEY, .code = KEY_LEFTSHIFT, .value = 1},
space_down     = {.type = EV_KEY, .code = KEY_SPACE,    .value = 1};
// clang-format on

__s32 prevCode;

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

int is_input_key_space(const struct input_event input) {
    if (input.value != 0) {
        prevCode = input.code;
    }

    if (input.code == KEY_SPACE) {
        return 1;
    }

    write_event(&input);
    return 0;
}

void space_to_shift(const struct input_event input) {
    if (!does_keyboard_changes(input)) {
        return;
    }
    if (!is_input_key_space(input)) {
        return;
    }

    if (input.value == 1) {
        write_event(&shift_down);
    }
    if (input.value == 0) {
        write_event(&shift_up);

        if (prevCode == KEY_SPACE) {
            write_event(&space_down);
            write_event(&input);  // space_up
        }
    }
}

int main(void) {
    struct input_event input;
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        space_to_shift(input);
    }
}
