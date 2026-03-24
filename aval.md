# AnoyLinux – Comprehensive Project Evaluation

> **Scope:** Full code review of all files in the repository as of March 2026.
> **Evaluator:** Static analysis, logic review, and documentation audit.

---

## 1. Project Overview

AnoyLinux (AnoyingPC Linux) is a C-based prank application for Linux desktops.
It uses a cron job to execute one of 9 harmless-but-annoying pranks every 6 minutes.
Features are toggled via marker files in `~/.anoypc/` and can be managed through interactive shell scripts.

---

## 2. Summary Scores

| Category                  | Score   |
|---------------------------|---------|
| Code correctness          | 5 / 10  |
| Code quality / style      | 6 / 10  |
| Documentation             | 5 / 10  |
| Security / safety         | 6 / 10  |
| Build / packaging         | 4 / 10  |
| Feature completeness      | 7 / 10  |
| **Overall**               | **5.5 / 10** |

---

## 3. Critical Bugs

### 3.1 Hardcoded User Path in `anoyon.sh`

**File:** `setup.sh` (lines 402–404, inside the heredoc for `anoyon.sh`)

```bash
# para debugging
CRON_JOB="*/1 * * * * /home/tnuno-mo/.anoypc/run.sh 2>&1 | tee -a /home/tnuno-mo/cron_debug.log"
```

The `anoyon.sh` script that gets written to `~/.anoypc/anoyon.sh` uses an absolute,
hardcoded path belonging to the developer (`/home/tnuno-mo/`).
On every other machine, this renders `anoyon.sh` completely broken — the cron job it registers will point to a non-existent path.
The correct line (commented out just above it) uses `$HOME` and should replace the debugging line.

**Severity: HIGH** — This breaks the re-enable feature for every user except the original developer.

---

### 3.2 Debug Artifact in `install_and_start.sh`

**File:** `install_and_start.sh` (lines 54–55)

```bash
echo "tnuno-mo"
echo
```

The script prints the developer's username after completing installation.
This is an unremoved debug artifact that should not be present in production code.

**Severity: MEDIUM** — Confusing to end users; reveals developer information.

---

### 3.3 `CAPS_ON` Feature Toggles Instead of Forcing ON

**File:** `AnoyPC.c` — `feature_keyboard_swap()` (approx. line 570)

```c
XkbLockModifiers(display, XkbUseCoreKbd, LockMask, is_on ? 0 : LockMask);
```

The feature is named `CAPS_ON` and documented as "Forces CAPS LOCK on", but the code toggles:
if CAPS is already on, it turns it off; if off, it turns it on.
To unconditionally force CAPS ON, the mask should always be `LockMask`:

```c
XkbLockModifiers(display, XkbUseCoreKbd, LockMask, LockMask);
```

**Severity: MEDIUM** — The feature does not match its documented and intended behaviour.

---

### 3.4 `--no-now` Option Documented But Not Implemented

**File:** `README.md` (line 103), `install_and_start.sh`

The README documents `./install_and_start.sh --no-now` as an option to "install only, no immediate prank".
However, `install_and_start.sh` does not accept or handle `--no-now` — it rejects any unknown argument with an error.

**Severity: MEDIUM** — Users who follow the documented usage will receive an error.

---

### 3.5 `cls` Called on Linux

**File:** `install_and_start.sh` (line 53)

```bash
clear || cls
```

`cls` is a Windows shell command. It is not available on Linux.
The `|| cls` branch will always silently fail, producing no visible breakage in most cases (since `clear` succeeds), but it is incorrect and should be removed.

**Severity: LOW** — Functionally harmless but indicates lack of cross-platform care.

---

## 4. Code Quality Issues

### 4.1 Use of `system()` for Shell Commands

**File:** `AnoyPC.c` — `run_command_best_effort()` and several feature functions

The program relies heavily on `system()` to spawn external tools (`xrandr`, `pactl`, `python3`, `setxkbmap`, `xset`).
Each `system()` call forks a shell, which is inefficient inside loops (e.g., `feature_flash()` calls `xrandr` 30 times in a loop).
Additionally, `system()` with complex inline shell strings is harder to audit for correctness.

**Recommendation:** Use `fork()`/`execv()` or batch the shell calls into a single script.

---

### 4.2 `feature_flash()` Incorrectly Marks Fullscreen as Used

**File:** `AnoyPC.c` — `feature_flash()` (approx. line 380)

```c
if (getenv("DISPLAY") != NULL && system(cmd_flash_tk) == 0) {
    used_fullscreen = 1;
}
```

`system()` returns 0 if the shell exited with 0.
The Python command will still exit 0 even if `tkinter` is not installed (it exits with 0 from the outer shell).
In that case, `used_fullscreen` is incorrectly set to 1 and the xrandr fallback is skipped, meaning no visual flash occurs.

---

### 4.3 Inconsistent Indentation in `feature_calendar()`

**File:** `AnoyPC.c` — `feature_calendar()` (approx. lines 474–490)

The local variable declarations and `printf` calls inside `feature_calendar()` use a mix of 4-space and 8-space indentation (due to a redundant extra block level), while all other functions use consistent 4-space indentation.

---

### 4.4 `feature_message()` — 30-Second Sleep is Disproportionate

**File:** `AnoyPC.c` — `feature_message()` (approx. line 230)

```c
sleep(30);  /* Display message for 30 seconds */
```

All other features wait 1–6 seconds. A 30-second hang for a one-line message is inconsistent and occupies the terminal far longer than expected. Consider reducing this to 5–10 seconds.

---

### 4.5 No Signal Handling for Long-Running Features

**File:** `AnoyPC.c` — `feature_upside_down()` and `feature_block_screen()`

`feature_upside_down()` calls `sleep(42)` after rotating the display.
If the process is killed (SIGINT, SIGTERM) during those 42 seconds, the display stays inverted permanently until the user manually runs `xrandr --rotate normal`.
A signal handler should restore the display on termination.

---

### 4.6 `get_home_dir()` Returns Pointer to Potentially Freed Memory

**File:** `AnoyPC.c` — `get_home_dir()` (approx. line 110)

```c
struct passwd* pw = getpwuid(getuid());
if (pw != NULL) {
    homedir = pw->pw_dir;
}
```

`getpwuid()` returns a pointer to a static internal buffer that may be overwritten by subsequent calls.
The returned pointer is used throughout the rest of `main()`, which is safe in practice (no subsequent `getpwuid` call exists), but it is fragile and the intent should be documented or the string should be copied with `strdup()`.

---

### 4.7 `feature_block_screen()` — Resource Leak on Grab Failure

**File:** `AnoyPC.c` — `feature_block_screen()`

If `XGrabPointer` or `XGrabKeyboard` fail (returns a value other than `GrabSuccess`), the code continues anyway without a fallback cleanup path. While this will not crash the program, it can leave the window mapped without proper input control, making cleanup less reliable.

---

## 5. Documentation Issues

### 5.1 Inconsistent Feature Count

- `STRUCTURE.md` refers to "7 features" (section header "As 7 Funcionalidades") and "7 marker files", but the project actually has **9 features**.
- `PROJECT_SUMMARY.md` lists 9 features correctly.
- The mismatch suggests STRUCTURE.md was not updated after features were added.

---

### 5.2 Cron Interval Inconsistency

- `README.md` states pranks execute "every 6 minutes".
- `STRUCTURE.md` (cycle diagram and customisation section) states "every 5 minutes" (`*/5`).
- `setup.sh` (function `setup_cron`) uses `*/6`.

The documented interval is not consistent across files.

---

### 5.3 Feature Names Differ Between README and Code

`README.md` lists feature 8 as "**Screen Rotate**" and feature 9 as "**Keyboard Swap**".
The actual internal feature names and marker files are `UPSIDE_DOWN` and `CAPS_ON`.
Users following the README to toggle features manually will use the wrong file names.

---

### 5.4 `STRUCTURE.md` Refers to Removed Features

`STRUCTURE.md` documents "Feature 3: Mouse Hide/Show (`MOUSE`)" as a distinct feature.
In the current code, `MOUSE` is treated as an alias for `BLOCK_SCREEN` in the dispatcher, and the mouse-hide logic described in STRUCTURE.md does not exist in the current `AnoyPC.c`.

---

### 5.5 Mixed Languages in Documentation

Documentation files (`PROJECT_SUMMARY.md`, `STRUCTURE.md`) are written in Portuguese,
while `README.md` and the source code comments are in English.
This inconsistency makes the project harder to navigate for international contributors.

---

### 5.6 `PROJECT_SUMMARY.md` Lists Wrong Installation Path

**File:** `PROJECT_SUMMARY.md` (line 11)

```
### Na pasta `/home/tnuno-mo/Projects/AnoyLinux/`
```

This hardcodes the developer's local machine path, which is meaningless to anyone else.

---

### 5.7 `QUICKSTART.sh` Is Not Executable

The file `QUICKSTART.sh` is a shell script that acts as a help display, but it is not marked as executable in the repository.
Users attempting to run it will receive "Permission denied" unless they manually `chmod +x QUICKSTART.sh`.

---

## 6. Build and Packaging Issues

### 6.1 Compiled Artifacts Committed to Repository

The compiled binary `AnoyPC` and object file `AnoyPC.o` are tracked in the repository.
Binary files should not be committed; they should be generated locally by `make`.
There is no `.gitignore` file in the repository (STRUCTURE.md references one, but it does not exist).

---

### 6.2 Missing `.gitignore`

`STRUCTURE.md` documents a `.gitignore` file as one of the "Ficheiros Incluídos", but the file does not exist in the repository.
At minimum, it should exclude:

```
AnoyPC
AnoyPC.o
```

---

### 6.3 `Makefile` Links With `CFLAGS` Instead of `LDFLAGS` Separately

**File:** `Makefile` (line 49)

```makefile
$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
```

Compiler flags (`CFLAGS`) are passed during the link step.
While this works with GCC, the conventional and correct practice is to use `LDFLAGS` (and optionally `CFLAGS`) only during compilation, and use `LDFLAGS` alone during linking.
This can cause warnings with some compilers and is non-standard.

---

### 6.4 No Test Target in Makefile

The Makefile has no `test` target and no automated tests exist anywhere in the project.
There is no way to verify correctness after a change without manually running each prank.

---

## 7. Security Observations

### 7.1 Overly Permissive Log File

**File:** `setup.sh` (line 194)

```bash
chmod 666 "$ANOYPC_DIR/anoypc.log"
```

Setting the log file to world-writable (`666`) allows any user on the system to write to or truncate it.
The log should be owned by the installing user and be `640` or `644` at most.

---

### 7.2 `system()` With Long Inline Shell Strings

The program constructs complex shell command strings as C string literals and passes them to `system()`.
While none of the current strings involve user-supplied data (so there is no immediate injection risk), the pattern is fragile.
Future contributors adding user-facing input to these strings could inadvertently introduce shell injection vulnerabilities.

---

### 7.3 Python Execution via `system()`

**File:** `AnoyPC.c` — `feature_flash()` (approx. line 365)

The flash feature constructs and executes a multi-line Python one-liner via `system()`.
If an attacker could influence the `DISPLAY` environment variable or pre-install a malicious `python3`, they could redirect execution.
This is low risk in the prank context but worth noting.

---

## 8. Minor Issues

| # | File | Issue |
|---|------|-------|
| 1 | `AnoyPC.c` | `upper_name` conversion loop uses magic number `32` instead of `'a' - 'A'` for clarity. |
| 2 | `AnoyPC.c` | `feature_keyboard_swap()` does not log its execution (unlike all other features). |
| 3 | `setup.sh` | Comment at line 400 reads `# Canonical schedule` but the canonical version is commented out. |
| 4 | `todo.md` | Lists "Trocar os espaços por tabs" (replace spaces with tabs) — not yet done in the Makefile recipe lines, which already correctly use tabs. Unclear what target file was intended. |
| 5 | `todo.md` | Lists `"Retirar as informações de que são pranks"` (remove prank references). This would make the application deceptive and misrepresent its nature to the victim. This item should be reconsidered from an ethical standpoint. |
| 6 | `install_and_start.sh` | `print_usage()` documents `--no-now` in README but the actual usage text in the script does not mention it either, causing a mismatch. |
| 7 | `AnoyPC.c` | `FEAT_COUNT` relies on the enum being contiguous and zero-based; this is fine in C99 but should be commented to warn maintainers not to assign explicit values to enum members. |
| 8 | `AnoyPC.c` | The `char formatted[44]` buffer in `feature_message()` is sized for 43 characters + NUL. Some messages are longer and will be silently truncated by the `%-43.43s` format, potentially cutting off the message mid-word. |
| 9 | `setup.sh` | The embedded `anoyon.sh` comment says "Canonical schedule" but the active line below is a debug variant hardcoded to 1-minute intervals and a specific user path. |
| 10 | Project-wide | The project name is spelled "AnoyLinux", "AnoyingPC Linux", and "AnoyPC" interchangeably across different files. A single canonical name should be chosen and used consistently. |

---

## 9. What Works Well

- The overall architecture (C binary + shell control scripts + marker files + cron) is simple, portable, and easy to understand.
- The Makefile is well-structured with `clean`, `install`, `uninstall`, and `help` targets.
- Feature isolation (each prank is a self-contained function) makes the code easy to extend.
- The `feature_block_screen()` implementation using X11 is technically sound and handles the timeout and keyboard grab correctly.
- The fallback chain in `feature_flash()` (tkinter → xrandr → ANSI terminal) is a good approach for maximising compatibility.
- `is_feature_enabled()` gracefully supports legacy feature names via aliases.
- `run_random_feature()` correctly builds a list of only enabled features before selecting randomly, avoiding bias.
- The control scripts (`anoydel.sh`, `anoyoff.sh`, `anoyon.sh`) are clean and do what they document.

---

## 10. Prioritised Fix List

| Priority | Item |
|----------|------|
| 🔴 HIGH | Fix hardcoded `/home/tnuno-mo/` path in `anoyon.sh` (setup.sh, line ~404) |
| 🔴 HIGH | Implement or remove the `--no-now` option in `install_and_start.sh` |
| 🟠 MEDIUM | Fix `CAPS_ON` logic to force ON unconditionally, not toggle |
| 🟠 MEDIUM | Remove `echo "tnuno-mo"` debug line from `install_and_start.sh` |
| 🟠 MEDIUM | Add `.gitignore` to exclude `AnoyPC` and `AnoyPC.o` |
| 🟠 MEDIUM | Align all documentation to say "9 features" and "6 minutes" consistently |
| 🟡 LOW | Add signal handler to `feature_upside_down()` to restore display on kill |
| 🟡 LOW | Reduce `feature_message()` sleep from 30 to 5–10 seconds |
| 🟡 LOW | Fix log file permissions from `666` to `644` |
| 🟡 LOW | Remove committed binary artifacts (`AnoyPC`, `AnoyPC.o`) from git tracking |
| 🟡 LOW | Fix feature name descriptions in README (Screen Rotate → UPSIDE_DOWN, Keyboard Swap → CAPS_ON) |
| 🟡 LOW | Mark `QUICKSTART.sh` as executable |

---

*Evaluation completed March 2026.*
