#pragma once

#include <ncurses.h>

#include <optional>
#include <vector>

enum class Mode {
    NORMAL,
    INSERT,
    VISUAL,
    EXIT
};

struct Line : std::string {
    size_t size() {
        return std::ranges::count_if(*this, [](char ch){ return ch != '\n'; });
    }
};

using Id = uint8_t;
class Buffer {
    static inline Id sId = 0;
    Id _id;

    std::string _logHandle;

    std::optional<std::string> _filename;

    WINDOW *_win;

    // Text
    std::vector<std::shared_ptr<Line>> _lines;
    std::shared_ptr<Line> _currentLine;

    // Current position
    struct Position {
        size_t x;
        size_t y;

        size_t xOffset;
        size_t yOffset;

        Position(size_t xBeg, size_t yBeg) : xOffset(xBeg), yOffset(yBeg) {}

        constexpr size_t xActual() {
            return x + xOffset;
        };

        constexpr size_t yActual() {
            return y + yOffset;
        };
    } _pos;

    // Bounds
    size_t _yMax;
    size_t _xMax;

    Mode _mode;

    enum class Direction: char {
        LEFT = 'h',
        DOWN = 'j',
        UP = 'k',
        RIGHT = 'l'
    };

    void printBuffer() const;

    void loadFromFile();
    bool saveToFile() const;

    void handleNormalCmd(char ch);

    void moveCursor(Direction key);
    void insertChar(char ch);
    void deleteChar();
    void createNewline();

public:
    Buffer(WINDOW *win, std::optional<std::string> filename = std::nullopt) ;
    ~Buffer();

    void handleInput(char ch);

    Id getId() const {
        return _id;
    }

    Mode getMode() const {
        return _mode;
    }

    WINDOW *getWindow() const {
        return _win;
    }
};

class BufferManager {
    std::vector<std::shared_ptr<Buffer>> _buffers;
    std::shared_ptr<Buffer> _activeBuf;

public:
    BufferManager() : _buffers{}, _activeBuf(nullptr) {}

    void createBuffer(WINDOW *win, std::optional<std::string> filename = std::nullopt) {
        _buffers.emplace_back(std::shared_ptr<Buffer>(new Buffer(win, filename)));
        _activeBuf = _buffers.back();
    }

    bool handleInput(char ch) {
        _activeBuf->handleInput(ch);

        if (_activeBuf->getMode() == Mode::EXIT) {
            _buffers.erase(_buffers.begin() + _activeBuf->getId());

            if (!_buffers.empty()) {
                _activeBuf = _buffers.front();
            } else {
                return false;
            }
        }

        return true;
    }

    WINDOW *getActiveWindow() const {
        return _activeBuf->getWindow();
    }
};
