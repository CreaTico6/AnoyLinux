First of all: How annoying is it to write "anoy"?! 😉

## ⚡ Quick Installation (One-Liner)

To install the suite silently and configure the background scheduler:

```
git clone https://github.com/CreaTico6/AnoyLinux.git && ./AnoyLinux/install.sh
```

# AnoyPC Linux 🎭

AnoyPC is a minimalist system automation and notification suite for Linux environments. It is designed to trigger various system simulations and visual effects at scheduled intervals.

Written in C99, it interacts directly with X11 and system drivers to ensure a low footprint and high compatibility across different distributions.


Note: The installer automatically removes the source directory after setup is complete to maintain a clean system.

## ✨ Features (12 Simulations)

The system rotates through the following events, all of which are harmless and reversible:

| Feature           | Technical Description                                                        |
|-------------------|------------------------------------------------------------------------------|
| MATRIX            | Full-screen digital rain simulation with a persistent central overlay.        |
| MOUSE_JITTER      | Simulates input instability by micro-adjusting the cursor position.           |
| BRIGHTNESS_PULSE  | Oscillates display brightness via xrandr backend.                            |
| BLOCK_SCREEN      | Full-screen focus overlay; requires specific key interaction to dismiss.      |
| UPSIDE_DOWN       | Triggers 180-degree display rotation for testing orientation drivers.         |
| BELL              | Tests audio subsystem with high-priority notification alerts.                 |
| CAPS_ON           | Forces the hardware Caps Lock state to active.                               |
| ALERT_SCREEN      | Generates a terminal-based system status report using reverse video mode.     |
| OTHERS            | Includes System Logs, Visual Flashes, Calendar Anomalies, and Hardware Spoofs.|

## 🚀 Management & Control

Post-installation, all control scripts and configuration files are located in the hidden directory `~/.anoypc/`.

**Operation Scripts:**

- `./features.sh`: Interactive menu to toggle specific events ON or OFF.
- `./test.sh`: Utility to trigger any event immediately for verification.
- `./anoyoff.sh`: Suspends the background scheduler (deactivates cron).
- `./anoyon.sh`: Resumes the automated schedule.
- `./anoydel.sh`: Completely uninstalls the suite, logs, and schedules.

## 🔧 Technical Overview

- **Architecture:** C99 binary with X11 (Xlib) integration.
- **Scheduling:** Standard Cron daemon, configured for a 30-minute interval.
- **Logging:** All system executions are tracked in `~/.anoypc/anoypc.log`.
- **Persistence:** The scheduler is persistent across system reboots.

## ⚠️ Disclaimer

This project is provided for educational and entertainment purposes only. Do not deploy this software on systems without explicit authorization. The author is not responsible for any misuse of these tools.

## 👨‍💻 Author

Developed by CreaTico6 (March 2026).