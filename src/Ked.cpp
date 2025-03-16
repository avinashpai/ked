#include <format>
#include <ncurses.h>

#include "Log.hpp"
#include "Editor.hpp"


Log logger {"Ked.log"};

int main() {
    //	std::string_view _logHandle = "Entry";
    //	logger.info(_logHandle, "Starting Ked...\n");

    // Init screen
    size_t y = 40, x = 178;
    logger.info("Init", std::format("Screen size: ({}, {})", x, y));

    Editor keditor(y, x);
    keditor.start();
}
