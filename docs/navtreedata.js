/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "fiber", "index.html", [
    [ "⚡ Motivation", "index.html#autotoc_md0", [
      [ "What does \"fiber-like\" mean?", "index.html#autotoc_md1", null ]
    ] ],
    [ "Example", "index.html#autotoc_md3", [
      [ "⏱ Real-Time scheduling", "index.html#autotoc_md4", null ],
      [ "💡 Stackless, deterministic memory usage", "index.html#autotoc_md5", null ]
    ] ],
    [ "✨ Features", "index.html#autotoc_md6", null ],
    [ "🧩 Integration", "index.html#autotoc_md7", [
      [ "(Preferred) CMake and CPM.cmake", "index.html#autotoc_md8", null ],
      [ "CMake with its command FetchContent", "index.html#autotoc_md9", null ],
      [ "CMake with its command add_subdirectory", "index.html#autotoc_md10", null ],
      [ "🛠 Don't Want to Use CMake?", "index.html#autotoc_md11", null ]
    ] ],
    [ "📜 Licensing", "index.html#autotoc_md12", [
      [ "✅ Open Source License (MIT)", "index.html#autotoc_md13", null ],
      [ "💼 Commercial License (for Closed/Proprietary Projects)", "index.html#autotoc_md14", null ]
    ] ],
    [ "Compile Options & Flags", "d0/de7/page_compile_options_and_flags.html", [
      [ "Tests", "d0/de7/page_compile_options_and_flags.html#section_compile_options_tests", null ],
      [ "System Stubs", "d0/de7/page_compile_options_and_flags.html#section_compile_options_sys_stubs", null ],
      [ "Exceptions and Assertions", "d0/de7/page_compile_options_and_flags.html#section_compile_options_exceptions_assertions", null ],
      [ "Encodings", "d0/de7/page_compile_options_and_flags.html#section_compile_options_encodings", null ],
      [ "Formating", "d0/de7/page_compile_options_and_flags.html#section_compile_options_formating", null ]
    ] ],
    [ "Coroutine Execution Model", "d2/d42/page_coroutine_execution_model.html", [
      [ "Overview", "d2/d42/page_coroutine_execution_model.html#section_coroutine_execution_model_overview", null ],
      [ "Roles", "d2/d42/page_coroutine_execution_model.html#section_coroutine_execution_model_roles", [
        [ "Coroutines are only logically nested but physically flat.", "d2/d42/page_coroutine_execution_model.html#autotoc_md39", null ]
      ] ],
      [ "Control Flow", "d2/d42/page_coroutine_execution_model.html#section_coroutine_execution_model_control_flow", null ],
      [ "Handling", "d2/d42/page_coroutine_execution_model.html#section_coroutine_execution_model_handling", null ],
      [ "Coroutine Chain Destruction", "d2/d42/page_coroutine_execution_model.html#section_coroutine_execution_model_coroutine_chain_destruction", null ],
      [ "Core Invariants", "d2/d42/page_coroutine_execution_model.html#section_coroutine_execution_model_core_invariants", null ],
      [ "Analogy", "d2/d42/page_coroutine_execution_model.html#section_coroutine_execution_model_analogy", null ]
    ] ],
    [ "Entangled Future-Promise Pairs", "dd/d10/page_entangled_future_promise_pairs.html", [
      [ "Introduction", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_introduction", null ],
      [ "Use Case: Asynchronous Data Retrieval", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_use_case", null ],
      [ "Life Cycle", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_life_cycle", null ],
      [ "API Summary", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_api_summary", null ],
      [ "Coroutine Integration", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_coroutine_integration", null ],
      [ "Teardown Safety", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_teardown_safety", null ],
      [ "Final Analogy", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_final_analogy", null ],
      [ "Optimisations", "dd/d10/page_entangled_future_promise_pairs.html#section_entangled_future_promise_pairs_optimisations", null ]
    ] ],
    [ "Exceptions and Assertions", "de/d1c/page_exceptions_and_assertions.html", [
      [ "Philosophy", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_philosophy", null ],
      [ "Customisation", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_customisation", [
        [ "Assertion Levels", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_assertion_levels", null ],
        [ "Turning Assertions into Optimisations", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_optimisations", null ],
        [ "But Be Careful!", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_be_careful", null ]
      ] ],
      [ "How to use Assertion Levels", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_usage", null ],
      [ "Redirect assertions from exceptions to callbacks", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_callbacks", null ],
      [ "Disable all checks and assertions", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_disable", null ],
      [ "Defining Custom Exceptions", "de/d1c/page_exceptions_and_assertions.html#page_exceptions_and_assertions_custom_exceptions", null ]
    ] ],
    [ "Getting Started", "d1/d66/getting_started.html", "d1/d66/getting_started" ],
    [ "Lines and Borders", "de/d65/lines_and_borders.html", [
      [ "Single Line Drawing Characters", "de/d65/lines_and_borders.html#single_lines", null ],
      [ "Double Line Drawing Characters", "de/d65/lines_and_borders.html#double_lines", null ],
      [ "Mixed Single/Double Line Characters", "de/d65/lines_and_borders.html#mixed_lines", null ],
      [ "Light Line Variants", "de/d65/lines_and_borders.html#light_lines", null ],
      [ "Heavy Line Drawing Characters", "de/d65/lines_and_borders.html#heavy_lines", null ],
      [ "Rounded Corners", "de/d65/lines_and_borders.html#rounded_corners", null ]
    ] ],
    [ "Output Streams and Formating", "de/d40/page_output_streams_and_formating.html", [
      [ "Overview", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_overview", null ],
      [ "Getting Started", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_getting_started", null ],
      [ "Formating and Streaming Capabilities", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_capabilities", null ],
      [ "Special Stream Functions", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_sepecial_functions", null ],
      [ "String Formatting (FormatStr)", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_FormatStr", null ],
      [ "Boolean Formatting (FormatBool)", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_FormatBool", null ],
      [ "Integer Formatting (FormatInt)", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_FormatInt", null ],
      [ "Integer with Suffix (FormatIntSuffix)", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_FormatIntSuffix", null ],
      [ "Floating-Point Formatting (FormatFloat)", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_FormatFloat", null ],
      [ "Hexadecimal Formatting (FormatHex)", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_FormatHex", null ],
      [ "Chrono Durations", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_format_chrono", null ],
      [ "Customization with Compiler Flags", "de/d40/page_output_streams_and_formating.html#section_output_stream_and_formating_customisations", null ]
    ] ],
    [ "Overflow aware: Clocks, Time-Points and Durations", "d8/dd3/page_overflow_aware_clocks.html", [
      [ "TODO: Rewrite for new TimePoint and Duration", "d8/dd3/page_overflow_aware_clocks.html#autotoc_md50", null ],
      [ "Overflow Awareness", "d8/dd3/page_overflow_aware_clocks.html#page_overflow_aware_clocks_overflow_awareness", null ],
      [ "Integration with <chrono>", "d8/dd3/page_overflow_aware_clocks.html#page_overflow_aware_clocks_overflow_integration_with_stdchrono", null ],
      [ "Example: Creating Your Own Clock", "d8/dd3/page_overflow_aware_clocks.html#page_overflow_aware_clocks_overflow_awareness_example", null ],
      [ "Why Use This?", "d8/dd3/page_overflow_aware_clocks.html#page_overflow_aware_clocks_overflow_awareness_why", null ],
      [ "Summary", "d8/dd3/page_overflow_aware_clocks.html#page_overflow_aware_clocks_summary", null ]
    ] ],
    [ "System Stubs: Wrappers for Embedded Binary Size Reduction", "d4/d7a/fiber_sys_stubs.html", [
      [ "Overview", "d4/d7a/fiber_sys_stubs.html#overview", null ],
      [ "Usage Example", "d4/d7a/fiber_sys_stubs.html#cmake_usage", null ],
      [ "It Works", "d4/d7a/fiber_sys_stubs.html#How", null ],
      [ "Results and Measured Impact", "d4/d7a/fiber_sys_stubs.html#results", null ],
      [ "Summary", "d4/d7a/fiber_sys_stubs.html#summary", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Concepts", "concepts.html", "concepts" ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"d0/d83/structfiber_1_1_format_float.html#a985ee35adbaf752265fb41f8ad5a318d",
"d1/d07/classfiber_1_1_left_dual_array_list_ref.html#a95bf659ef1b2994fafb1e7881c7fd62b",
"d2/d5a/classfiber_1_1_right_dual_array_list_const_ref.html#a0ed8726391a4a99fc15798587a5b8bdb",
"d3/d96/classfiber_1_1_o_stream.html#ae18989af181c620fde13e363601497ed",
"d3/dd6/classfiber_1_1_future.html#a85feff0702816ce1704d163f4374aa05",
"d6/d1d/namespacefiber_1_1ansi.html#a5183dcd83b4bc6a73467369d5f21769f",
"d6/db3/classfiber_1_1_dual_array_list.html#a6f793e320703279c1bf2887268d2fa6b",
"d7/d9e/_exceptions_8cpp.html",
"da/d1f/classfiber_1_1_soft_periodic_task.html",
"db/d7f/classfiber_1_1_time_point.html#a189de5196bfabcffc6c4475c82a27ace",
"dc/ded/structfiber_1_1_task_base_1_1less__priority__s.html#a16d28e81cd74c37079de5878b770a210",
"de/d65/lines_and_borders.html#heavy_lines",
"dir_e05846a5d2127b48398043025b7e8347.html"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';