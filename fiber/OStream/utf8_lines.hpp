#pragma once

#include <string_view>

#ifdef FIBER_DISABLE_UTF8_CODES
    #define FIBER_UTF8_CODE(str, alternative) alternative
#else
    #define FIBER_UTF8_CODE(str, alternative) str
#endif

namespace fiber::utf8_lines {

// Single Line Drawing Characters
constexpr std::string_view single_horizontal      = FIBER_UTF8_CODE("─", "-"); // ─ "─"
constexpr std::string_view single_vertical        = FIBER_UTF8_CODE("│", "|"); // │ "│"
constexpr std::string_view single_corner_topleft  = FIBER_UTF8_CODE("┌", "+"); // ┌ "┌"
constexpr std::string_view single_corner_topright = FIBER_UTF8_CODE("┐", "+"); // ┐ "┐"
constexpr std::string_view single_corner_botleft  = FIBER_UTF8_CODE("└", "+"); // └ "└"
constexpr std::string_view single_corner_botright = FIBER_UTF8_CODE("┘", "+"); // ┘ "┘"
constexpr std::string_view single_t_up            = FIBER_UTF8_CODE("┬", "+"); // ┬ "┬"
constexpr std::string_view single_t_down          = FIBER_UTF8_CODE("┴", "+"); // ┴ "┴"
constexpr std::string_view single_t_left          = FIBER_UTF8_CODE("├", "+"); // ├ "├"
constexpr std::string_view single_t_right         = FIBER_UTF8_CODE("┤", "+"); // ┤ "┤"
constexpr std::string_view single_cross           = FIBER_UTF8_CODE("┼", "+"); // ┼ "┼"

// Double Line Drawing Characters
constexpr std::string_view double_horizontal      = FIBER_UTF8_CODE("═", "-"); // ═ "═"
constexpr std::string_view double_vertical        = FIBER_UTF8_CODE("║", "|"); // ║ "║"
constexpr std::string_view double_corner_topleft  = FIBER_UTF8_CODE("╔", "+"); // ╔ "╔"
constexpr std::string_view double_corner_topright = FIBER_UTF8_CODE("╗", "+"); // ╗ "╗"
constexpr std::string_view double_corner_botleft  = FIBER_UTF8_CODE("╚", "+"); // ╚ "╚"
constexpr std::string_view double_corner_botright = FIBER_UTF8_CODE("╝", "+"); // ╝ "╝"
constexpr std::string_view double_t_up            = FIBER_UTF8_CODE("╦", "+"); // ╦ "╦"
constexpr std::string_view double_t_down          = FIBER_UTF8_CODE("╩", "+"); // ╩ "╩"
constexpr std::string_view double_t_left          = FIBER_UTF8_CODE("╠", "+"); // ╠ "╠"
constexpr std::string_view double_t_right         = FIBER_UTF8_CODE("╣", "+"); // ╣ "╣"
constexpr std::string_view double_cross           = FIBER_UTF8_CODE("╬", "+"); // ╬ "╬"

// Mixed Single/Double Line Characters
constexpr std::string_view mixed_corner_topleft   = FIBER_UTF8_CODE("╒", "+"); // ╒ "╒"
constexpr std::string_view mixed_corner_topright  = FIBER_UTF8_CODE("╕", "+"); // ╕ "╕"
constexpr std::string_view mixed_corner_botleft   = FIBER_UTF8_CODE("╘", "+"); // ╘ "╘"
constexpr std::string_view mixed_corner_botright  = FIBER_UTF8_CODE("╛", "+"); // ╛ "╛"
constexpr std::string_view mixed_t_up             = FIBER_UTF8_CODE("╤", "+"); // ╤ "╤"
constexpr std::string_view mixed_t_down           = FIBER_UTF8_CODE("╧", "+"); // ╧ "╧"
constexpr std::string_view mixed_t_left           = FIBER_UTF8_CODE("╞", "+"); // ╞ "╞"
constexpr std::string_view mixed_t_right          = FIBER_UTF8_CODE("╡", "+"); // ╡ "╡"
constexpr std::string_view mixed_cross            = FIBER_UTF8_CODE("╪", "+"); // ╪ "╪"

// Light Line Variants (similar to single, but sometimes spaced differently)
constexpr std::string_view light_horizontal       = FIBER_UTF8_CODE("─", "-");
constexpr std::string_view light_vertical         = FIBER_UTF8_CODE("│", "-");
constexpr std::string_view light_corner_topleft   = FIBER_UTF8_CODE("┌", "+");
constexpr std::string_view light_corner_topright  = FIBER_UTF8_CODE("┐", "+");
constexpr std::string_view light_corner_botleft   = FIBER_UTF8_CODE("└", "+");
constexpr std::string_view light_corner_botright  = FIBER_UTF8_CODE("┘", "+");

// Heavy Line Drawing Characters
constexpr std::string_view heavy_horizontal       = FIBER_UTF8_CODE("━", "-"); // ━ "━"
constexpr std::string_view heavy_vertical         = FIBER_UTF8_CODE("┃", "|"); // ┃ "┃"
constexpr std::string_view heavy_corner_topleft   = FIBER_UTF8_CODE("┏", "+"); // ┏ "┏"
constexpr std::string_view heavy_corner_topright  = FIBER_UTF8_CODE("┓", "+"); // ┓ "┓"
constexpr std::string_view heavy_corner_botleft   = FIBER_UTF8_CODE("┗", "+"); // ┗ "┗"
constexpr std::string_view heavy_corner_botright  = FIBER_UTF8_CODE("┛", "+"); // ┛ "┛"
constexpr std::string_view heavy_t_up             = FIBER_UTF8_CODE("┳", "+"); // ┳ "┳"
constexpr std::string_view heavy_t_down           = FIBER_UTF8_CODE("┻", "+"); // ┻ "┻"
constexpr std::string_view heavy_t_left           = FIBER_UTF8_CODE("┣", "+"); // ┣ "┣"
constexpr std::string_view heavy_t_right          = FIBER_UTF8_CODE("┫", "+"); // ┫ "┫"
constexpr std::string_view heavy_cross            = FIBER_UTF8_CODE("╋", "+"); // ╋ "╋"

// Rounded Corners (usually used with light lines)
constexpr std::string_view rounded_corner_topleft  = FIBER_UTF8_CODE("╭", "+"); // ╭ "╭"
constexpr std::string_view rounded_corner_topright = FIBER_UTF8_CODE("╮", "+"); // ╮ "╮"
constexpr std::string_view rounded_corner_botleft  = FIBER_UTF8_CODE("╰", "+"); // ╰ "╰"
constexpr std::string_view rounded_corner_botright = FIBER_UTF8_CODE("╯", "+"); // ╯ "╯"

} // namespace terminal_lines
