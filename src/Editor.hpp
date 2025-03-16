#pragma once

#include <vector>
#include <ncurses.h>

#include "Buffer.hpp"

class Editor {
    size_t _yMax;
    size_t _xMax;
    std::vector<std::shared_ptr<Buffer>> _buffers;
    std::shared_ptr<Buffer> _activeBuf;

public:
    Editor(size_t yMax, size_t xMax);
    void start();
};
