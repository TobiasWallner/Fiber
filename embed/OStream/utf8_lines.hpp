#pragma once

#include <string_view>

#ifdef EMBED_DISABLE_UTF8_CODES
    #define EMBED_UTF8_CODE(str, alternative) alternative
#else
    #define EMBED_UTF8_CODE(str, alternative) str
#endif

namespace embed::utf8_lines {

// Single Line Drawing Characters
constexpr std::string_view single_horizontal      = EMBED_UTF8_CODE("─", "-"); // ─ "─"
constexpr std::string_view single_vertical        = EMBED_UTF8_CODE("│", "|"); // │ "│"
constexpr std::string_view single_corner_topleft  = EMBED_UTF8_CODE("┌", "+"); // ┌ "┌"
constexpr std::string_view single_corner_topright = EMBED_UTF8_CODE("┐", "+"); // ┐ "┐"
constexpr std::string_view single_corner_botleft  = EMBED_UTF8_CODE("└", "+"); // └ "└"
constexpr std::string_view single_corner_botright = EMBED_UTF8_CODE("┘", "+"); // ┘ "┘"
constexpr std::string_view single_t_up            = EMBED_UTF8_CODE("┬", "+"); // ┬ "┬"
constexpr std::string_view single_t_down          = EMBED_UTF8_CODE("┴", "+"); // ┴ "┴"
constexpr std::string_view single_t_left          = EMBED_UTF8_CODE("├", "+"); // ├ "├"
constexpr std::string_view single_t_right         = EMBED_UTF8_CODE("┤", "+"); // ┤ "┤"
constexpr std::string_view single_cross           = EMBED_UTF8_CODE("┼", "+"); // ┼ "┼"

// Double Line Drawing Characters
constexpr std::string_view double_horizontal      = EMBED_UTF8_CODE("═", "-"); // ═ "═"
constexpr std::string_view double_vertical        = EMBED_UTF8_CODE("║", "|"); // ║ "║"
constexpr std::string_view double_corner_topleft  = EMBED_UTF8_CODE("╔", "+"); // ╔ "╔"
constexpr std::string_view double_corner_topright = EMBED_UTF8_CODE("╗", "+"); // ╗ "╗"
constexpr std::string_view double_corner_botleft  = EMBED_UTF8_CODE("╚", "+"); // ╚ "╚"
constexpr std::string_view double_corner_botright = EMBED_UTF8_CODE("╝", "+"); // ╝ "╝"
constexpr std::string_view double_t_up            = EMBED_UTF8_CODE("╦", "+"); // ╦ "╦"
constexpr std::string_view double_t_down          = EMBED_UTF8_CODE("╩", "+"); // ╩ "╩"
constexpr std::string_view double_t_left          = EMBED_UTF8_CODE("╠", "+"); // ╠ "╠"
constexpr std::string_view double_t_right         = EMBED_UTF8_CODE("╣", "+"); // ╣ "╣"
constexpr std::string_view double_cross           = EMBED_UTF8_CODE("╬", "+"); // ╬ "╬"

// Mixed Single/Double Line Characters
constexpr std::string_view mixed_corner_topleft   = EMBED_UTF8_CODE("╒", "+"); // ╒ "╒"
constexpr std::string_view mixed_corner_topright  = EMBED_UTF8_CODE("╕", "+"); // ╕ "╕"
constexpr std::string_view mixed_corner_botleft   = EMBED_UTF8_CODE("╘", "+"); // ╘ "╘"
constexpr std::string_view mixed_corner_botright  = EMBED_UTF8_CODE("╛", "+"); // ╛ "╛"
constexpr std::string_view mixed_t_up             = EMBED_UTF8_CODE("╤", "+"); // ╤ "╤"
constexpr std::string_view mixed_t_down           = EMBED_UTF8_CODE("╧", "+"); // ╧ "╧"
constexpr std::string_view mixed_t_left           = EMBED_UTF8_CODE("╞", "+"); // ╞ "╞"
constexpr std::string_view mixed_t_right          = EMBED_UTF8_CODE("╡", "+"); // ╡ "╡"
constexpr std::string_view mixed_cross            = EMBED_UTF8_CODE("╪", "+"); // ╪ "╪"

// Light Line Variants (similar to single, but sometimes spaced differently)
constexpr std::string_view light_horizontal       = EMBED_UTF8_CODE("─", "-");
constexpr std::string_view light_vertical         = EMBED_UTF8_CODE("│", "-");
constexpr std::string_view light_corner_topleft   = EMBED_UTF8_CODE("┌", "+");
constexpr std::string_view light_corner_topright  = EMBED_UTF8_CODE("┐", "+");
constexpr std::string_view light_corner_botleft   = EMBED_UTF8_CODE("└", "+");
constexpr std::string_view light_corner_botright  = EMBED_UTF8_CODE("┘", "+");

// Heavy Line Drawing Characters
constexpr std::string_view heavy_horizontal       = EMBED_UTF8_CODE("━", "-"); // ━ "━"
constexpr std::string_view heavy_vertical         = EMBED_UTF8_CODE("┃", "|"); // ┃ "┃"
constexpr std::string_view heavy_corner_topleft   = EMBED_UTF8_CODE("┏", "+"); // ┏ "┏"
constexpr std::string_view heavy_corner_topright  = EMBED_UTF8_CODE("┓", "+"); // ┓ "┓"
constexpr std::string_view heavy_corner_botleft   = EMBED_UTF8_CODE("┗", "+"); // ┗ "┗"
constexpr std::string_view heavy_corner_botright  = EMBED_UTF8_CODE("┛", "+"); // ┛ "┛"
constexpr std::string_view heavy_t_up             = EMBED_UTF8_CODE("┳", "+"); // ┳ "┳"
constexpr std::string_view heavy_t_down           = EMBED_UTF8_CODE("┻", "+"); // ┻ "┻"
constexpr std::string_view heavy_t_left           = EMBED_UTF8_CODE("┣", "+"); // ┣ "┣"
constexpr std::string_view heavy_t_right          = EMBED_UTF8_CODE("┫", "+"); // ┫ "┫"
constexpr std::string_view heavy_cross            = EMBED_UTF8_CODE("╋", "+"); // ╋ "╋"

// Rounded Corners (usually used with light lines)
constexpr std::string_view rounded_corner_topleft  = EMBED_UTF8_CODE("╭", "+"); // ╭ "╭"
constexpr std::string_view rounded_corner_topright = EMBED_UTF8_CODE("╮", "+"); // ╮ "╮"
constexpr std::string_view rounded_corner_botleft  = EMBED_UTF8_CODE("╰", "+"); // ╰ "╰"
constexpr std::string_view rounded_corner_botright = EMBED_UTF8_CODE("╯", "+"); // ╯ "╯"

} // namespace terminal_lines
