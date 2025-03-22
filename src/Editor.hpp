#pragma once

#include <optional>
#include <vector>
#include <ncurses.h>

#include "Buffer.hpp"

class Editor {
    BufferManager _bufferMgr;

public:
    Editor(size_t yMax, size_t xMax, std::optional<std::string> fileOrDir = std::nullopt);
    ~Editor();

    void start();
};
