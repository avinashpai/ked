#include <ncurses.h>

#include <filesystem>
#include <format>

#include "Buffer.hpp"
#include "Log.hpp"


Buffer::Buffer(WINDOW *win, std::optional<std::string> filename): _win(win), _pos(getbegx(win), getbegy(win)), _mode(Mode::NORMAL) {
    _id = Buffer::sId++;
    _logHandle = std::format("Buffer ({})", _id);

    getmaxyx(_win, _yMax, _xMax);

    Log::info(_logHandle, std::format("Created with window size ({}, {}) at ({}, {})", _xMax, _yMax, _pos.xOffset, _pos.yOffset));

    if (filename.has_value()) {
        _filename = std::move(filename);
        loadFromFile();
    }

    if (_lines.empty()) {
        _lines = { std::shared_ptr<Line>(new Line) };
    }
    _currentLine = _lines.front();
}

Buffer::~Buffer() {
   delwin(_win);
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
        *_lines[i] += '\n';
    }
    Log::info(_logHandle, std::format("Successfully loaded {}", *_filename));

    printBuffer();
    wmove(_win, _pos.yActual(), _pos.xActual());
}

bool Buffer::saveToFile() const {
    if (!_filename.has_value()) {
        // TODO: prompt
        Log::err(_logHandle, "No file name!");
        return false;
    }

    // TODO: save file
    std::ofstream output(*_filename);
    for (auto line : _lines) {
        output << line->data();
    }

    Log::info(_logHandle, std::format("Saved to {}", *_filename));
    return true;
}

void Buffer::printBuffer() const {
    // TODO: Only print within bounds
    // TODO: Scrolling

    auto maxLines = std::min(_yMax, _lines.size());
    auto y = _pos.yOffset, x = _pos.xOffset;
    for (size_t i = 0; i < maxLines; i++) {
        mvwaddstr(_win, y++, x, _lines[i]->data());
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

    size_t prevX = _pos.xActual();
    size_t prevY = _pos.yActual();
    switch (ch) {
        case LEFT:
            if (_pos.x != 0) {
                _pos.x--;
            }
            break;
        case DOWN:
            if (_pos.y != _yMax && _pos.y != _lines.size() - 1) {
                _pos.y++;
                _currentLine = _lines[_pos.y];

                auto lineSize = _currentLine->size();
                _pos.x = lineSize == 0 ? 0 : std::min(_pos.x, lineSize - 1);
            }
            break;
        case UP:
            if (_pos.y != 0) {
                _pos.y--;
                _currentLine = _lines[_pos.y];

                auto lineSize = _currentLine->size();
                _pos.x = lineSize == 0 ? 0 : std::min(_pos.x, lineSize - 1);
            }
            break;
        case RIGHT:
            auto lineSize = _currentLine->size();
            auto lineEnd = _mode == Mode::INSERT ? lineSize : lineSize - 1;
            if (_pos.x != _xMax && lineSize != 0  && _pos.x != lineEnd) {
                _pos.x++;
            }
            break;
    }
 
    size_t newX = _pos.xActual();
    size_t newY = _pos.yActual();

    if (newX != prevX || newY != prevY) {
        wmove(_win, newY, newX);
        Log::info(_logHandle, std::format("Moving to ({}, {})", _pos.xActual(), _pos.yActual()));
    }
}

void Buffer::createNewline() {
    _pos.x = 0;
    _pos.y++;
    Log::info(_logHandle, std::format("Creating newline at y={}", _pos.yActual()));

    _lines.insert(_lines.begin() + _pos.y, std::shared_ptr<Line>(new Line));
    _currentLine = _lines[_pos.y];
}

void Buffer::insertChar(char ch) {
    // TODO: Scrolling

    mvwinsch(_win, _pos.yActual(), _pos.xActual(), ch);

    _currentLine->insert(_currentLine->begin() + _pos.x, ch);

    Log::info(_logHandle, std::format("Inserting \'{}\' at ({}, {})", ch, _pos.xActual(), _pos.yActual()));

    if (ch == '\n') {
        createNewline();
    } else {
        _pos.x++;
        if(_pos.x == _xMax - _pos.xOffset - 2) {
            createNewline();
        }
    }
    wmove(_win, _pos.yActual(), _pos.xActual()); // FIX: Redo how we move, this is gross
}

void Buffer::deleteChar() {
    if (_pos.x != 0 && !_currentLine->empty()) {
        if (_mode == Mode::INSERT) {
            _pos.x--;
        }
        Log::info(_logHandle, std::format("Delete {} at ({}, {})", (*_currentLine)[_pos.x], _pos.xActual(), _pos.yActual()));
        mvwdelch(_win, _pos.yActual(), _pos.xActual());
        _currentLine->erase(_currentLine->begin() + _pos.x);
    } else if (!_lines.empty() && _currentLine != _lines.front()) {
        _pos.y--;
        _currentLine = _lines[_pos.y];
        _pos.x = _currentLine->size();
        wmove(_win, _pos.yActual(), + _pos.xActual());

        // TODO: Move lines!

        if (_pos.x == _xMax - _pos.xOffset - 1) {
            deleteChar();
        }
    }
}
