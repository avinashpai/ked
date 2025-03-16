#include "Editor.hpp"
#include "Buffer.hpp"


Editor::Editor(size_t yMax, size_t xMax, std::optional<std::string> fileOrDir) : _yMax(yMax), _xMax(xMax) {
    initscr();
    raw();
    noecho();

    _bufferMgr = {};
    _bufferMgr.createBuffer(yMax, xMax, fileOrDir);

    refresh();
}

Editor::~Editor() {
    endwin();
}

void Editor::start() {
    while (_bufferMgr.handleInput(getch())) {
        refresh();
    }
    endwin();
}

