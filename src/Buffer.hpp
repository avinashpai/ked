#pragma once

#include <optional>
#include <vector>

enum class Mode {
    NORMAL,
    INSERT,
    VISUAL,
    EXIT
};

struct Line : std::vector<char> {
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

    // Text
    std::vector<std::shared_ptr<Line>> _lines;
    std::shared_ptr<Line> _currentLine;

    // Current position
    size_t _y = 0;
    size_t _x = 0;

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
    void insertNewline();

public:
    Buffer(size_t yMax, size_t xMax, std::optional<std::string> filename = std::nullopt) ;

    void handleInput(char ch);

    Id getId() const {
        return _id;
    }

    Mode getMode() const {
        return _mode;
    }
};

class BufferManager {
    std::vector<std::shared_ptr<Buffer>> _buffers;
    std::shared_ptr<Buffer> _activeBuf;

public:
    BufferManager() : _buffers{}, _activeBuf(nullptr) {}

    void createBuffer(size_t yMax, size_t xMax, std::optional<std::string> filename = std::nullopt) {
        _buffers.emplace_back(std::shared_ptr<Buffer>(new Buffer(yMax, xMax, filename)));
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
};
