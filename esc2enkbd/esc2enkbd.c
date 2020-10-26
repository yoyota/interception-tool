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

void esc_to_en_kbd(const struct input_event input) {
    if (!does_keyboard_changes(input)) {
        return;
    }
    write_event(&input);

    if (input.code == KEY_ESC && input.value == 0) {
        system("sudo -u yoyota /usr/bin/fcitx-remote -s fcitx-keyboard-us");
    }
}

int main(void) {
    struct input_event input;
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        esc_to_en_kbd(input);
    }
}
