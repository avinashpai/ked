#include "Editor.hpp"


Editor::Editor(size_t yMax, size_t xMax, std::optional<std::string> fileOrDir) : _yMax(yMax), _xMax(xMax) {
    initscr();
    raw();
    noecho();

    _buffers = { std::shared_ptr<Buffer>(new Buffer(_yMax, _xMax, fileOrDir)) };
    _activeBuf = _buffers.front();

    refresh();
}

Editor::~Editor() {
    endwin();
}

void Editor::start() {
    while (true) {
        // TODO: Buffer switching

        _activeBuf->handleInput(getch());

        if (_activeBuf->getMode() == Mode::EXIT) {
            _buffers.erase(_buffers.begin() + _activeBuf->getId());

            if (!_buffers.empty()) {
                _activeBuf = _buffers.front();
            } else {
                goto end;
            }
        }

        refresh();
    }
end:
    endwin();
}

