#pragma once

#include <optional>
#include <vector>
#include <ncurses.h>

#include "Buffer.hpp"

class Editor {
    size_t _yMax;
    size_t _xMax;

    std::vector<std::shared_ptr<Buffer>> _buffers;
    std::shared_ptr<Buffer> _activeBuf;

    std::optional<std::string> fileOrDir;

public:
    Editor(size_t yMax, size_t xMax, std::optional<std::string> fileOrDir = std::nullopt);
    ~Editor();

    void start();
};
