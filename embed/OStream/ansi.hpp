#pragma once

// Compile-time toggle to disable ANSI codes
#ifdef EMBED_ENABLE_ANSI_CODES
  #define EMBED_ANSI_CODE(str) str
#else
  #define EMBED_ANSI_CODE(str) ""
#endif

namespace embed::ansi {

// Reset
constexpr const char* reset         = EMBED_ANSI_CODE("\x1b[0m");

// Text styles
constexpr const char* bold          = EMBED_ANSI_CODE("\x1b[1m");
constexpr const char* dim           = EMBED_ANSI_CODE("\x1b[2m");
constexpr const char* italic        = EMBED_ANSI_CODE("\x1b[3m");
constexpr const char* underline     = EMBED_ANSI_CODE("\x1b[4m");
constexpr const char* inverse       = EMBED_ANSI_CODE("\x1b[7m");
constexpr const char* hidden        = EMBED_ANSI_CODE("\x1b[8m");
constexpr const char* strikethrough = EMBED_ANSI_CODE("\x1b[9m");

// Foreground colors (standard)
constexpr const char* black         = EMBED_ANSI_CODE("\x1b[30m");
constexpr const char* red           = EMBED_ANSI_CODE("\x1b[31m");
constexpr const char* green         = EMBED_ANSI_CODE("\x1b[32m");
constexpr const char* yellow        = EMBED_ANSI_CODE("\x1b[33m");
constexpr const char* blue          = EMBED_ANSI_CODE("\x1b[34m");
constexpr const char* magenta       = EMBED_ANSI_CODE("\x1b[35m");
constexpr const char* cyan          = EMBED_ANSI_CODE("\x1b[36m");
constexpr const char* white         = EMBED_ANSI_CODE("\x1b[37m");

// Foreground colors (bright)
constexpr const char* bright_black  = EMBED_ANSI_CODE("\x1b[90m");
constexpr const char* bright_red    = EMBED_ANSI_CODE("\x1b[91m");
constexpr const char* bright_green  = EMBED_ANSI_CODE("\x1b[92m");
constexpr const char* bright_yellow = EMBED_ANSI_CODE("\x1b[93m");
constexpr const char* bright_blue   = EMBED_ANSI_CODE("\x1b[94m");
constexpr const char* bright_magenta= EMBED_ANSI_CODE("\x1b[95m");
constexpr const char* bright_cyan   = EMBED_ANSI_CODE("\x1b[96m");
constexpr const char* bright_white  = EMBED_ANSI_CODE("\x1b[97m");

// Background colors (standard)
constexpr const char* bg_black      = EMBED_ANSI_CODE("\x1b[40m");
constexpr const char* bg_red        = EMBED_ANSI_CODE("\x1b[41m");
constexpr const char* bg_green      = EMBED_ANSI_CODE("\x1b[42m");
constexpr const char* bg_yellow     = EMBED_ANSI_CODE("\x1b[43m");
constexpr const char* bg_blue       = EMBED_ANSI_CODE("\x1b[44m");
constexpr const char* bg_magenta    = EMBED_ANSI_CODE("\x1b[45m");
constexpr const char* bg_cyan       = EMBED_ANSI_CODE("\x1b[46m");
constexpr const char* bg_white      = EMBED_ANSI_CODE("\x1b[47m");

// Background colors (bright)
constexpr const char* bg_bright_black  = EMBED_ANSI_CODE("\x1b[100m");
constexpr const char* bg_bright_red    = EMBED_ANSI_CODE("\x1b[101m");
constexpr const char* bg_bright_green  = EMBED_ANSI_CODE("\x1b[102m");
constexpr const char* bg_bright_yellow = EMBED_ANSI_CODE("\x1b[103m");
constexpr const char* bg_bright_blue   = EMBED_ANSI_CODE("\x1b[104m");
constexpr const char* bg_bright_magenta= EMBED_ANSI_CODE("\x1b[105m");
constexpr const char* bg_bright_cyan   = EMBED_ANSI_CODE("\x1b[106m");
constexpr const char* bg_bright_white  = EMBED_ANSI_CODE("\x1b[107m");

// Aliases:
constexpr const char* grey = bright_black;

} // namespace embed::ansi