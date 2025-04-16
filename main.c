#include <stdlib.h>

#include <ncurses.h>

#include "pdp11/pdp11.h"
#include "pdp11/pdp11_console.h"
#include "pdp11/pdp11_rom.h"

enum { UI_COL_BG, UI_COL_LOWER, UI_COL_HIGHER };

void draw_bordered_win(
    WINDOW *const window,
    int const window_h,
    int const window_w
) {
    static chtype const border_char_tl = '/' | COLOR_PAIR(UI_COL_BG),
                        border_char_tr = '\\' | COLOR_PAIR(UI_COL_BG),
                        border_char_bl = '\\' | COLOR_PAIR(UI_COL_BG),
                        border_char_br = '/' | COLOR_PAIR(UI_COL_BG),
                        border_char_v = '|' | COLOR_PAIR(UI_COL_BG),
                        border_char_h = '-' | COLOR_PAIR(UI_COL_BG);

    wbkgd(window, ' ' | COLOR_PAIR(UI_COL_BG));
    wrefresh(window);

    wmove(window, 0, 0);
    waddch(window, border_char_tl);
    for (int i = 1; i < window_w - 1; i++) waddch(window, border_char_h);
    waddch(window, border_char_tr);

    for (int j = 1; j < window_h - 1; j++) {
        mvwaddch(window, j, 0, border_char_h);
        mvwaddch(window, j, window_w - 1, border_char_v);
    }

    for (int j = 1; j < window_h - 1; j++) {
        for (int i = 1; i < window_w - 1; i++) { mvwaddch(window, j, i, ' '); }
    }

    waddch(window, border_char_bl);
    for (int i = 1; i < window_w - 1; i++) waddch(window, border_char_h);
    waddch(window, border_char_br);

    wrefresh(window);
}

int console_ui_run(Pdp11Console *const console) {
    WINDOW *const screen = initscr();
    if (screen == NULL) return fprintf(stderr, "failed to init ncurses!\n"), 1;

    if (!has_colors())
        return delwin(screen), endwin(),
               fprintf(stderr, "colors not supported!\n"), 2;

    start_color();
    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_WHITE, 200, 200, 200);
    init_color(COLOR_RED, 0xCC, 0x44, 0x55);
    init_color(COLOR_MAGENTA, 0x88, 0x44, 0x55);
    init_pair(UI_COL_BG, COLOR_WHITE, COLOR_BLACK);
    init_pair(UI_COL_LOWER, COLOR_WHITE, COLOR_RED);
    init_pair(UI_COL_HIGHER, COLOR_WHITE, COLOR_MAGENTA);

    WINDOW *const ui = newwin(10, 10, 0, 0);

    bool do_run = true;
    while (do_run) {
        /* draw_console(&console) */
        int const ui_h = getmaxy(screen) / 2, ui_w = getmaxx(screen);
        wresize(ui, ui_h, ui_w);
        draw_bordered_win(ui, ui_h, ui_w);
    }

    delwin(ui);
    delwin(screen);
    return endwin(), 0;
}

int main() {
    Pdp11 pdp = {0};
    assert(pdp11_init(&pdp) == Ok);

    Pdp11Rom rom = {0};
    FILE *const file = fopen("res/m9342-248f1.bin", "r");
    if (!file) return 1;
    assert(pdp11_rom_init_file(&rom, PDP11_BOOT_PC, file) == Ok);
    fclose(file);
    pdp.unibus.devices[PDP11_FIRST_USER_DEVICE + 0] =
        pdp11_rom_ww_unibus_device(&rom);

    Pdp11Console console = {0};
    pdp11_console_init(&console, &pdp);

    console_ui_run(&console);

    pdp11_rom_uninit(&rom);

    pdp11_power_down(&pdp);
    pdp11_uninit(&pdp);

    return 0;
}
