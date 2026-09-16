#include "porytiles/utilities/text/terminal_width.hpp"

#include <cstddef>
#include <cstdlib>
#include <optional>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace {

/// @brief Parses the COLUMNS environment variable into a positive width, if it is set and valid.
std::optional<std::size_t> width_from_columns_env()
{
    const char *columns = std::getenv("COLUMNS");
    if (columns == nullptr) {
        return std::nullopt;
    }
    try {
        std::size_t consumed = 0;
        const long value = std::stol(std::string{columns}, &consumed);
        // Require the whole value to parse and to be positive; ignore junk like "abc" or "80x".
        if (consumed == std::string{columns}.size() && value > 0) {
            return static_cast<std::size_t>(value);
        }
    }
    catch (...) {
        // Fall through to the next source on any parse failure.
    }
    return std::nullopt;
}

/// @brief Queries the terminal column count for @p fd, if it is a terminal reporting a width.
std::optional<std::size_t> width_from_terminal([[maybe_unused]] const int fd)
{
#ifdef _WIN32
    // Windows: query the console screen buffer info. The `fd` argument is ignored
    // (POSIX-style file descriptors don't exist on Windows consoles).
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        const int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        if (width > 0) {
            return static_cast<std::size_t>(width);
        }
    }
    return std::nullopt;
#else
    struct winsize ws{};
    if (ioctl(fd, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        return static_cast<std::size_t>(ws.ws_col);
    }
    return std::nullopt;
#endif
}

} // namespace

namespace porytiles {

std::size_t resolve_terminal_width(const int fd, const std::size_t fallback)
{
    if (const auto from_env = width_from_columns_env()) {
        return *from_env;
    }
    if (const auto from_terminal = width_from_terminal(fd)) {
        return *from_terminal;
    }
    return fallback;
}

} // namespace porytiles
