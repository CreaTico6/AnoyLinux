First of all: How annoying is it to write "anoy"?! 😉

AnoyPC Linux 🎭

AnoyPC is a minimalist system automation and notification suite for Linux environments. It is designed to trigger various system simulations and visual effects at scheduled, unpredictable intervals.

Written in C99, it interacts directly with X11 and system drivers to ensure a low footprint and high compatibility across different distributions.

⚡ Installation

The project provides two distinct installation targets depending on your intent:

1. Test & Development Mode (Interactive)

Installs the suite into ~/.anoypc/, complete with interactive menus to test features, toggle settings, and manage the cron scheduler manually.

```sh
git clone https://github.com/CreaTico6/AnoyLinux.git && make -C AnoyLinux install
```

2. Stealth Mode (Minimalist)

A highly covert installation for practical field testing. It installs only the necessary binaries into an obfuscated directory named ... (three dots) inside the Home folder (~/...). It leaves no interactive scripts, no symlinks, and self-destructs the downloaded source code automatically.

```sh
git clone https://github.com/CreaTico6/AnoyLinux.git && make -C AnoyLinux test
```

✨ Features (20 Simulations)

The system includes the following simulations, all perfectly reversible upon standard interruption (SIGINT/SIGTERM):

Feature

Technical Description

MATRIX

Full-screen digital rain simulation with a persistent central overlay.

MOUSE_JITTER

Simulates input instability by micro-adjusting the cursor position.

MOUSE_TELEPORT

Instantly teleports the mouse cursor to a random screen position.

MOUSE_SWAP

Swaps left and right mouse buttons mapping temporarily.

MOUSE_POOP

Leaves a trail of small, temporary black squares behind the mouse cursor.

CHASING_BUTTON

Spawns a native X11 window that evades the user's cursor.

CLICK_DISABLE

Temporarily disables physical mouse clicks (maps buttons to 0).

CUSTOM_CURSOR

Randomly changes the system cursor to a bizarre icon (skull, spider, etc).

BRIGHTNESS_PULSE

Oscillates display brightness via xrandr backend.

GRAYSCALE

Forces the display to grayscale (black & white) using xcalib or xrandr.

TERMINAL_LOOP

Opens a terminal emulator, types a fake warning, and auto-closes.

BLOCK_SCREEN

Full-screen focus overlay; requires specific key interaction to dismiss.

UPSIDE_DOWN

Triggers 180-degree display rotation for testing orientation drivers.

BELL

Tests audio subsystem with high-priority notification alerts.

CAPS_ON

Forces the hardware Caps Lock state to active.

ALERT_SCREEN

Generates a terminal-based system status report using reverse video mode.

SYSINFO

Displays spoofed system information.

FLASH

Visual flash effect on the screen.

CALENDAR

Displays a fake calendar anomaly.

MESSAGE

Prints realistic but fake system kernel/dmesg warnings.

🔧 Technical Overview

Architecture: C99 binary with X11 (Xlib) integration.

Scheduling: Standard Cron daemon, configured for unpredictable intervals (Stealth Mode).

Persistence: Uses .zshenv / .zshrc injection to survive École 42 network logouts or system reboots.

📦 Optional Dependencies

To guarantee the GRAYSCALE feature works on older display managers, it is recommended to have xcalib installed:

sudo apt install xcalib


⚠️ Disclaimer

This project is provided for educational and system-testing purposes only. Do not deploy this software on systems without explicit authorization. The author is not responsible for any misuse of these tools.

👨‍💻 Author

Developed by tnuno-mo (March 2026).