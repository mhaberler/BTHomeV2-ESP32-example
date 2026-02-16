---
name: threadedButtonScan
description: Move polling/tick code into an RTOS task and convert shared state to atomic globals.
argument-hint: Provide the target file or selected code, the polling function name (e.g. `button.tick()`), and shared variable names to convert.
---
You are editing embedded C++ code (ESP32/Arduino or similar). Refactor the selected code so the polling/tick call for an input or sensor runs in its own RTOS thread/task and shared mutable globals become atomic (no mutexes).

Requirements
- Add `#include <atomic>` and convert listed shared globals to `std::atomic<T>`.
- Create a polling task (e.g. `pollTask`) that repeatedly calls the polling/tick function (placeholder: `{{pollFunction}}`) and sleeps with `vTaskDelay(pdMS_TO_TICKS({{interval_ms|10}}))`.
- Start the task in initialization (`setup()`/init) using the RTOS API (e.g. `xTaskCreate`) with sensible defaults (stack: 2048, priority: 1) unless specified.
- Remove the original polling call from the main loop.
- Update callbacks and other reads/writes to use atomic operations (`store`, `load`, `exchange`, `compare_exchange_weak`) and implement toggles with `compare_exchange` where needed.
- When the main loop consumes an event counter, use `exchange(0)` to atomically read-and-clear.
- Keep changes minimal and maintain existing behavior; do not introduce mutexes.
- Ensure the code compiles; if not, report the compile error and a one-line fix.

Placeholders
- `{{selected code}}` — the code or file to change
- `{{pollFunction}}` — polling/tick call to move (e.g. `button.tick()`)
- `{{atomicVars}}` — names/types of globals to convert (e.g. `clickCount:int`, `modeFlag:bool`)
- `{{interval_ms}}` — polling interval in milliseconds (default 10)
- `{{taskStack}}` — RTOS task stack size (default 2048)
- `{{taskPriority}}` — task priority (default 1)

Output required
1. Modified file(s) with minimal, compile-safe edits.
2. Short changelog (file → changes).
3. One-line rationale for design choices (why atomics / why a task).
4. If build fails, show compile error and the fix.

Example instruction you can pass to the assistant
"Refactor `src/main.cpp`: move `button.tick()` into a FreeRTOS task, convert `clickCount:int` and `modeFlag:bool` to `std::atomic`, scan every 10ms, keep behavior unchanged."
