#include "Editor.hpp"
#include "Buffer.hpp"


Editor::Editor(size_t yMax, size_t xMax, std::optional<std::string> fileOrDir) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, true);

    _bufferMgr = {};

    WINDOW *testWin = newwin(yMax, xMax+2, 0, 2);
    wborder(testWin, 0, ' ', ' ', 0, ACS_VLINE, ' ' , 0, ' ');
    wmove(testWin, 0, 2);

    _bufferMgr.createBuffer(testWin, fileOrDir);

    wrefresh(testWin);
}

Editor::~Editor() {
    endwin();
}

void Editor::start() {
    auto *win = _bufferMgr.getActiveWindow();
    while (_bufferMgr.handleInput(wgetch(win))) {
        win = _bufferMgr.getActiveWindow();
        wrefresh(win);
    }
    endwin();
}

