# AGENTS.md

## Cursor Cloud specific instructions

This repository is the **FreeRTOS-Kernel** — a C static library for embedded systems, not a standalone application. There are no runtime services, Docker, or web servers.

### Building

- Use `cmake` with `GCC` (not Clang — the default `cc` on the VM is Clang, which causes unused-variable errors in the CMock test harness). Always pass `-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++` to cmake.
- The **GCC_POSIX** port allows building and running the kernel natively on Linux. Use `-DFREERTOS_PORT=GCC_POSIX -DFREERTOS_HEAP=4`.
- The `examples/cmake_example` uses `TEMPLATE` port by default (via `CACHE ... FORCE`), which compiles but produces a non-runnable binary. To build a runnable POSIX binary, create a standalone CMake project that sets `FREERTOS_PORT=GCC_POSIX`.

### Linting / Spell Check

- **cSpell**: `cspell lint "include/**/*.h" "*.c" "portable/MemMang/*.c" "portable/Common/*.c" --config cspell.config.yaml`
- **Kernel header checker**: Requires `common/header_checker.py` from the parent [FreeRTOS/FreeRTOS](https://github.com/FreeRTOS/FreeRTOS) repo (`.github/scripts/common/`). Copy it into `.github/scripts/common/` then run: `python3 .github/scripts/kernel_checker.py --json <modified.json> <added.json> <renamed.json>`
- **Formatting**: Uses `uncrustify` with config from [FreeRTOS/CI-CD-Github-Actions](https://github.com/FreeRTOS/CI-CD-Github-Actions/tree/main/formatting).

### Unit Tests (CMock)

Unit tests live in the **parent** [FreeRTOS/FreeRTOS](https://github.com/FreeRTOS/FreeRTOS) repository under `FreeRTOS/Test/CMock`, not in this repo. To run them locally:

1. Clone the parent repo: `git clone --depth 1 --recurse-submodules --shallow-submodules https://github.com/FreeRTOS/FreeRTOS.git /tmp/FreeRTOS`
2. Replace `FreeRTOS/Source` with a symlink to this workspace: `rm -rf /tmp/FreeRTOS/FreeRTOS/Source && ln -s /workspace /tmp/FreeRTOS/FreeRTOS/Source`
3. Run tests: `CC=gcc make -C /tmp/FreeRTOS/FreeRTOS/Test/CMock run_col_formatted`

**Known issue**: The `tasks` module has 2 pre-existing test failures (`test_vTaskPriorityDisinheritAfterTimeout_success` and `test_vTaskPriorityDisinheritAfterTimeout_success2`) due to struct size expectations differing with GCC 13 vs the CI's GCC version. These are not caused by code changes. The CMock tests also inject a `stopTimers()` function into `timers.c` — always run `git checkout -- timers.c` after tests to revert.

### Required System Packages

`build-essential`, `gcc`, `g++`, `cmake`, `make`, `ruby`, `lcov`, `cflow`, `unifdef`, `doxygen`, `python3`, `cspell` (via npm).
