# AnoyLinux 🎭

A minimalist system automation and prank suite for Linux. AnoyLinux triggers entertaining system simulations and visual effects at scheduled intervals—perfect for system testing, demos, or playful pranks on colleague terminals (with permission!).

**Language:** C | **Dependencies:** X11, Cron | **Status:** Complete

## 🎯 Overview

AnoyPC is a C99-based system automation suite featuring 20 reversible simulations that interact directly with X11 and system drivers. Designed for low system footprint and high compatibility across Linux distributions.

### 20 Simulations Available

| Effect | Description |
|--------|-------------|
| **MATRIX** | Full-screen digital rain with persistent overlay |
| **MOUSE_JITTER** | Micro-adjusts cursor position (input instability) |
| **MOUSE_TELEPORT** | Instantly moves mouse to random screen position |
| **MOUSE_SWAP** | Swaps left/right mouse button mapping |
| **MOUSE_POOP** | Leaves trail of black squares behind cursor |
| **CHASING_BUTTON** | Native X11 window that evades your cursor |
| **CLICK_DISABLE** | Temporarily disables mouse clicks |
| **CUSTOM_CURSOR** | Randomly changes cursor (skull, spider, etc.) |
| **BRIGHTNESS_PULSE** | Oscillates display brightness via xrandr |
| **GRAYSCALE** | Forces B&W display mode |
| **TERMINAL_LOOP** | Opens terminal with fake warning, auto-closes |
| **BLOCK_SCREEN** | Full-screen focus overlay (dismissible) |
| **UPSIDE_DOWN** | 180° display rotation |
| **BELL** | High-priority audio alert |
| **CAPS_ON** | Forces Caps Lock activation |
| **ALERT_SCREEN** | Terminal system status report (reverse video) |
| **SYSINFO** | Displays spoofed system information |
| **FLASH** | Screen flash effect |
| **CALENDAR** | Fake calendar anomaly display |
| **MESSAGE** | Realistic kernel/dmesg warning messages |

## 🏗️ Architecture

**Core Technologies:**
- **Language:** C99 with X11 (Xlib) integration
- **Scheduling:** Standard Cron daemon for unpredictable intervals
- **Persistence:** Shell environment injection (`.zshenv`/`.zshrc`)
- **Reversibility:** All effects respond to SIGINT/SIGTERM

## 🚀 Installation

### Mode 1: Test & Development (Interactive)
Full suite with interactive menus for testing and manual cron management:
```bash
git clone https://github.com/CreaTico6/AnoyLinux.git
make -C AnoyLinux install
```

### Mode 2: Stealth Mode (Minimalist)
Covert installation: binaries only, minimal footprint, scheduled execution:
```bash
git clone https://github.com/CreaTico6/AnoyLinux.git
make -C AnoyLinux test
```

## 📦 Optional Dependencies

For full feature support (especially GRAYSCALE on older displays):
```bash
sudo apt install xcalib  # Debian/Ubuntu
```

## 🔧 Usage Examples

```bash
# List all available simulations
./anoypc --help

# Trigger specific effect
./anoypc --matrix

# Run random effect
./anoypc --random

# Exit/Cancel any active effect
# Press: Ctrl+C (SIGINT) or Ctrl+Z (SIGTERM)
```

## 💡 What I Learned

- **X11 Programming:** Low-level graphics manipulation and input device control
- **System Integration:** Cron scheduling, environment persistence, signal handling
- **Process Management:** Fork/exec, child process cleanup, signal safety
- **Display Drivers:** xrandr, xcalib, and low-level brightness control
- **Compatibility:** Cross-distribution Linux support and graceful fallbacks

## ⚠️ Disclaimer

This project is for **educational and authorized system testing only**. Always obtain explicit permission before running on systems you don't own. The author assumes no responsibility for unauthorized use.

---

**Author:** tnuno-mo  
**Created:** March 2026 | **Last Updated:** June 2026  
**License:** Educational Use Only
