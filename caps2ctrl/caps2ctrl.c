#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

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

void caps_to_ctrl(const struct input_event input) {
    if (!does_keyboard_changes(input)) {
        return;
    }

    if (input.code == KEY_CAPSLOCK) {
        const struct input_event ctrl_key = {
            .type = EV_KEY, .code = KEY_LEFTCTRL, .value = input.value};
        write_event(&ctrl_key);
        return;
    }

    if (input.code == KEY_LEFTCTRL) {
        const struct input_event capslock_key = {
            .type = EV_KEY, .code = KEY_CAPSLOCK, .value = input.value};
        write_event(&capslock_key);
        return;
    }

    write_event(&input);
}

int main(void) {
    struct input_event input;
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        caps_to_ctrl(input);
    }
}
