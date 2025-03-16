#pragma once

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

	void handleNormalCmd(char ch);

	void moveCursor(Direction key);
	void insertChar(char ch);
	void deleteChar();
	void insertNewline();

public:
	Buffer(size_t yMax, size_t xMax);

	void handleInput(char ch);

	Id getId() const {
		return _id;
	}

	Mode getMode() const {
		return _mode;
	}
};
