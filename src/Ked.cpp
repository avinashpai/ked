#include <ncurses.h>

#include <filesystem>
#include <format>

#include "Log.hpp"
#include "Editor.hpp"


int main(int argc, char *argv[]) {
    std::optional<std::string> filename = std::nullopt;
    if (argc > 1) {
        filename = std::move(argv[1]);
    }

    // Init screen
    size_t y = 40, x = 178;
    Log::info("Init", std::format("Screen size: ({}, {})", x, y));

    Editor keditor(y, x, filename);
    keditor.start();
}
