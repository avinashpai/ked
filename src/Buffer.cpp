#include <ncurses.h>

#include <filesystem>
#include <format>

#include "Buffer.hpp"
#include "Log.hpp"


Buffer::Buffer(size_t yMax, size_t xMax, std::optional<std::string> filename): _yMax(yMax), _xMax(xMax), _mode(Mode::NORMAL) {
    _id = Buffer::sId++;
    _logHandle = std::format("Buffer ({})", _id);

    if (filename.has_value()) {
        _filename = std::move(filename);
        loadFromFile();
    }

    if (_lines.empty()) {
        _lines = { std::shared_ptr<Line>(new Line) };
    }
    _currentLine = _lines[0];
}

void Buffer::loadFromFile() {
    if (!std::filesystem::exists(*_filename)) { return; }

    if (std::filesystem::is_directory(*_filename)) {
        // TODO: Directory explorer
        Log::info(_logHandle, std::format("{} is a directory -- to be implemented!", *_filename));
        return;
    }

    std::ifstream input(*_filename);
    size_t i = 0;
    for (std::string line; std::getline(input, line); i++) {
        _lines.emplace_back(std::shared_ptr<Line>(new Line));
        std::copy(line.begin(), line.end(), std::back_inserter(*_lines[i]));
        _lines[i]->emplace_back('\n');
    }
    Log::info(_logHandle, std::format("Successfully loaded {}", *_filename));

    printBuffer();
    move(_y, _x);
}

bool Buffer::saveToFile() const {
    if (!_filename.has_value()) {
        // TODO: prompt
        Log::err(_logHandle, "No file name!");
        return false;
    }

    // TODO: save file
    Log::info(_logHandle, std::format("Saved to {}", *_filename));
    return true;
}

void Buffer::printBuffer() const {
    // TODO: Only print within bounds
    // TODO: Scrolling

    auto maxLines = std::min(_yMax, _lines.size());
    for (size_t i = 0; i < maxLines; i++) {
        addstr(_lines[i]->data());
    }
}

void Buffer::handleInput(char ch) {
    using enum Mode;

    switch (_mode) {
        case NORMAL:
            handleNormalCmd(ch);
            break;
        case INSERT:
            if (ch == 27) {
                _mode = Mode::NORMAL;
                moveCursor(Direction::LEFT);
                Log::info(_logHandle, "INSERT -> NORMAL");
            } else if (ch == 127) {
                deleteChar();
            } else {
                insertChar(ch);
            }
            break;
        case VISUAL:
        case EXIT:
            _mode = EXIT;
            break;
    }
}

void Buffer::handleNormalCmd(char ch) {
    using enum Mode;

    switch (ch) {
        case 'a':
            _mode = INSERT;
            Log::info(_logHandle, "NORMAL -> INSERT (append)");
            moveCursor(Direction::RIGHT); // TODO:
            break;
        case 'i':
            _mode = INSERT;
            Log::info(_logHandle, "NORMAL -> INSERT");
            break;
        case 's':
            if (!saveToFile()) break;
        case 'q':
            _mode = EXIT;
            Log::info(_logHandle, "NORMAL -> EXIT");
            break;
        case 'h':
        case 'j':
        case 'k':
        case 'l':
            moveCursor(Direction(ch));
            break;
        case 'x':
            deleteChar();
            break;
        default:
            // TODO:
            break;
    }
}

void Buffer::moveCursor(Direction ch) {
    using enum Direction;

    switch (ch) {
        case LEFT:
            if (_x != 0) {
                _x--;
            }
            break;
        case DOWN:
            if (_y != _yMax && _y != _lines.size() - 1) {
                _y++;
                _currentLine = _lines[_y];

                auto lineSize = _currentLine->size();
                _x = lineSize == 0 ? 0 : std::min(_x, lineSize - 1);
            }
            break;
        case UP:
            if (_y != 0) {
                _y--;
                _currentLine = _lines[_y];

                auto lineSize = _currentLine->size();
                _x = lineSize == 0 ? 0 : std::min(_x, lineSize - 1);
            }
            break;
        case RIGHT:
            auto lineSize = _currentLine->size();
            auto lineEnd = _mode == Mode::INSERT ? lineSize : lineSize - 1;
            if (_x != _xMax && lineSize != 0  && _x != lineEnd) {
                _x++;
            }
            break;
    }

    move(_y, _x);
    Log::info(_logHandle, std::format("Moving to ({}, {})", _x, _y));
}

void Buffer::insertNewline() {
    _x = 0;
    _y++;

    _lines.insert(_lines.begin() + _y, std::shared_ptr<Line>(new Line));
    _currentLine = _lines[_y];
}

void Buffer::insertChar(char ch) {
    if (_x == _xMax) {
        insertNewline();
    }

    // TODO: Scrolling


    if (_x == _currentLine->size() ) {
        addch(ch);
    } else {
        mvinsch(_y, _x, ch);
        move(_y, _x+1); // FIX: Redo how we move, this is gross
    }
    _currentLine->insert(_currentLine->begin() + _x, ch);

    Log::info(_logHandle, std::format("Inserting \'{}\' at ({}, {})", ch, _x, _y));

    if (ch == '\n') {
        insertNewline();
    } else {
        _x++;
    }
}

void Buffer::deleteChar() {
    if (_x != 0 && !_currentLine->empty()) {
        if (_mode == Mode::INSERT) {
            _x--;
        }
        Log::info(_logHandle, std::format("Delete {} at ({}, {})", (*_currentLine)[_x], _x, _y));
        mvdelch(_y, _x);
        _currentLine->erase(_currentLine->begin() + _x);
    }
}
