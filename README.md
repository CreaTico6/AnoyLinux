First of all: How annoying is it to write "anoy"?! 😉

⚡ Quick Installation (One-Liner)

To install the suite silently and configure the background scheduler:

git clone git@github.com:CreaTico6/AnoyLinux.git && bash ./AnoyLinux/install.sh


AnoyPC Linux 🎭

AnoyPC is a minimalist system automation and notification suite for Linux environments. It is designed to trigger various system simulations and visual effects at scheduled, unpredictable intervals.

Written in C99, it interacts directly with X11 and system drivers to ensure a low footprint and high compatibility across different distributions.

Note: The installer automatically removes the source directory after setup is complete to maintain a clean system.

✨ Features (20 Simulations)

O sistema inclui as seguintes simulações, todas perfeitamente reversíveis:

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

🚀 Management & Control

Post-installation, all control scripts and configuration files are located in the hidden directory ~/.anoypc/.

Operation Scripts:

~/.local/bin/anoypc-features: Interactive menu to toggle specific events ON or OFF.

~/.local/bin/anoypc-test: Utility to trigger any event immediately for verification.

~/.anoypc/anoyoff.sh: Suspends the background scheduler (deactivates cron).

~/.anoypc/anoyon.sh: Resumes the automated schedule.

~/.anoypc/anoydel.sh: Completely uninstalls the suite, logs, and schedules.

🔧 Technical Overview

Architecture: C99 binary with X11 (Xlib) integration.

Scheduling: Standard Cron daemon, configured for unpredictable intervals (Stealth Mode).

Logging: All system executions are tracked in ~/.anoypc/anoypc.log.

Persistence: Uses .zshenv/.zprofile injection.

📦 Optional Dependencies

Para a feature GRAYSCALE funcionar em todos os sistemas, é recomendado instalar:

xcalib (preferencial) ou garanta suporte a xrandr --gamma

Exemplo de instalação (Sistemas baseados em Debian):

sudo apt install xcalib


⚠️ Disclaimer

This project is provided for educational and system-testing purposes only. Do not deploy this software on systems without explicit authorization. The author is not responsible for any misuse of these tools.

👨‍💻 Author

Developed by tnuno-mo (March 2026).
