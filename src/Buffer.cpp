#include <ncurses.h>

#include <format>

#include "Buffer.hpp"
#include "Log.hpp"

extern Log logger;
// this is a test.

Buffer::Buffer(size_t yMax, size_t xMax) : _yMax(yMax), _xMax(xMax), _mode(Mode::NORMAL) {
	_id = Buffer::sId++;
	_logHandle = std::format("Buffer ({})", _id);

	_lines = { std::shared_ptr<Line>(new Line) };
	_currentLine = _lines[0];
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
			logger.info(_logHandle, "INSERT -> NORMAL");
			break;
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
	case 'i':
		_mode = INSERT;
		logger.info(_logHandle, "NORMAL -> INSERT");
		break;
	case 'q':
		_mode = EXIT;
		logger.info(_logHandle, "NORMAL -> EXIT");
		break;
	case 'h':
	case 'j':
	case 'k':
	case 'l':
		moveCursor(Direction(ch));
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

			_x = std::min(_x, _currentLine->size() - 1);
		}
		break;
	case UP:
		if (_y != 0) {
			_y--;
			_currentLine = _lines[_y];

			_x = std::min(_x, _currentLine->size() - 1);
		}
		break;
	case RIGHT:
		if (_x != _xMax && _x != _currentLine->size() - 1) {
			_x++;
		}
		break;
	}

	move(_y, _x);
	logger.info(_logHandle, std::format("Moving to ({}, {})", _x, _y));
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

	logger.info(_logHandle, std::format("Inserting \'{}\' at ({}, {})", ch, _x, _y));

	if (ch == '\n') {
		insertNewline();
	} else {
		_x++;
	}
}

void Buffer::deleteChar() {
	if (_x != 0 && !_currentLine->empty()) {
		_x--;
		logger.info(_logHandle, std::format("Delete {} at ({}, {})", (*_currentLine)[_x], _x, _y));
		mvdelch(_y, _x);
		_currentLine->erase(_currentLine->begin() + _x);
	}
}
