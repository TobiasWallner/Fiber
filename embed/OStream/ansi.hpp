#pragma once

#include <string_view>

// Compile-time toggle to disable ANSI codes
#ifdef EMBED_DISABLE_ANSI_CODES
  #define EMBED_ANSI_CODE(str) ""
#else
  #define EMBED_ANSI_CODE(str) str
#endif

namespace embed::ansi {

// Reset
constexpr std::string_view reset         = EMBED_ANSI_CODE("\x1b[0m");

// Text styles
constexpr std::string_view bold          = EMBED_ANSI_CODE("\x1b[1m");
constexpr std::string_view dim           = EMBED_ANSI_CODE("\x1b[2m");
constexpr std::string_view italic        = EMBED_ANSI_CODE("\x1b[3m");
constexpr std::string_view underline     = EMBED_ANSI_CODE("\x1b[4m");
constexpr std::string_view inverse       = EMBED_ANSI_CODE("\x1b[7m");
constexpr std::string_view hidden        = EMBED_ANSI_CODE("\x1b[8m");
constexpr std::string_view strikethrough = EMBED_ANSI_CODE("\x1b[9m");

// Foreground colors
constexpr std::string_view black         = EMBED_ANSI_CODE("\x1b[30m");
constexpr std::string_view red           = EMBED_ANSI_CODE("\x1b[31m");
constexpr std::string_view green         = EMBED_ANSI_CODE("\x1b[32m");
constexpr std::string_view yellow        = EMBED_ANSI_CODE("\x1b[33m");
constexpr std::string_view orange        = EMBED_ANSI_CODE("\x1b[38;2;255;165;0m");
constexpr std::string_view blue          = EMBED_ANSI_CODE("\x1b[34m");
constexpr std::string_view magenta       = EMBED_ANSI_CODE("\x1b[35m");
constexpr std::string_view cyan          = EMBED_ANSI_CODE("\x1b[36m");
constexpr std::string_view white         = EMBED_ANSI_CODE("\x1b[37m");

// Foreground colors light
constexpr std::string_view light_black         = EMBED_ANSI_CODE("\x1b[38;2;80;80;80m");
constexpr std::string_view light_red           = EMBED_ANSI_CODE("\x1b[38;2;255;85;85m");
constexpr std::string_view light_green         = EMBED_ANSI_CODE("\x1b[38;2;144;238;144m");
constexpr std::string_view light_yellow        = EMBED_ANSI_CODE("\x1b[38;2;255;255;145m");
constexpr std::string_view light_orange        = EMBED_ANSI_CODE("\x1b[38;2;255;190;20m");
constexpr std::string_view light_blue          = EMBED_ANSI_CODE("\x1b[38;2;173;216;230m");
constexpr std::string_view light_magenta       = EMBED_ANSI_CODE("\x1b[38;2;240;140;240m");
constexpr std::string_view light_cyan          = EMBED_ANSI_CODE("\x1b[38;2;128;255;255m");
constexpr std::string_view light_grey          = EMBED_ANSI_CODE("\x1b[38;2;200;200;200m");

// Foreground colors (bright)
constexpr std::string_view bright_black  = EMBED_ANSI_CODE("\x1b[90m");
constexpr std::string_view bright_red    = EMBED_ANSI_CODE("\x1b[91m");
constexpr std::string_view bright_green  = EMBED_ANSI_CODE("\x1b[92m");
constexpr std::string_view bright_yellow = EMBED_ANSI_CODE("\x1b[93m");
constexpr std::string_view bright_blue   = EMBED_ANSI_CODE("\x1b[94m");
constexpr std::string_view bright_magenta= EMBED_ANSI_CODE("\x1b[95m");
constexpr std::string_view bright_cyan   = EMBED_ANSI_CODE("\x1b[96m");
constexpr std::string_view bright_white  = EMBED_ANSI_CODE("\x1b[97m");

// Background colors (standard)
constexpr std::string_view bg_black      = EMBED_ANSI_CODE("\x1b[40m");
constexpr std::string_view bg_red        = EMBED_ANSI_CODE("\x1b[41m");
constexpr std::string_view bg_green      = EMBED_ANSI_CODE("\x1b[42m");
constexpr std::string_view bg_yellow     = EMBED_ANSI_CODE("\x1b[43m");
constexpr std::string_view bg_blue       = EMBED_ANSI_CODE("\x1b[44m");
constexpr std::string_view bg_magenta    = EMBED_ANSI_CODE("\x1b[45m");
constexpr std::string_view bg_cyan       = EMBED_ANSI_CODE("\x1b[46m");
constexpr std::string_view bg_white      = EMBED_ANSI_CODE("\x1b[47m");

// Background colors (bright)
constexpr std::string_view bg_bright_black  = EMBED_ANSI_CODE("\x1b[100m");
constexpr std::string_view bg_bright_red    = EMBED_ANSI_CODE("\x1b[101m");
constexpr std::string_view bg_bright_green  = EMBED_ANSI_CODE("\x1b[102m");
constexpr std::string_view bg_bright_yellow = EMBED_ANSI_CODE("\x1b[103m");
constexpr std::string_view bg_bright_blue   = EMBED_ANSI_CODE("\x1b[104m");
constexpr std::string_view bg_bright_magenta= EMBED_ANSI_CODE("\x1b[105m");
constexpr std::string_view bg_bright_cyan   = EMBED_ANSI_CODE("\x1b[106m");
constexpr std::string_view bg_bright_white  = EMBED_ANSI_CODE("\x1b[107m");

// Aliases:
constexpr std::string_view grey = bright_black;

} // namespace embed::ansi