#include "Editor.hpp"


Editor::Editor(size_t yMax, size_t xMax) : _yMax(yMax), _xMax(xMax) {
    _buffers = { std::shared_ptr<Buffer>(new Buffer(_yMax, _xMax)) };
    _activeBuf = _buffers.front();
}

void Editor::start() {
    initscr();
    raw();
    noecho();

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

