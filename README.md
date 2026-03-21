# AnoyingPC Linux 🎭 – The Art of Digital Disruption (Linux Edition)

A minimalist and robust Linux prank application designed to randomly trigger annoying-but-harmless system disruptions. It's the perfect tool for executing a harmless yet unsettling practical joke on friends or colleagues using Linux.

---

## ⚡ TL;DR (Quick Start)

1. **Clone/Download** the repository
2. **Run** `./install_and_start.sh`
3. **Chaos is ON** immediately and then every 6 minutes

### Control & Removal

The setup creates an `~/.anoypc` folder with these shell scripts:

* **`features.sh`**: Toggle individual features ON or OFF (interactive menu).
* **`test.sh`**: Immediately trigger any individual prank for testing (no waiting for the scheduler).
* **`anoyoff.sh`**: Stops ALL chaos temporarily (disables the cron job).
* **`anoyon.sh`**: Re-enables ALL chaos after using `anoyoff.sh`.
* **`anoydel.sh`**: **Removes the cron entry AND deletes the `~/.anoypc` folder.** (Total Cleanup)

---

## ⚠️ Disclaimer

This is a **prank application** intended strictly for entertainment purposes. All features are **completely harmless** and cause **no real system damage, data loss, or permanent changes**. Use responsibly, ethically, and only on systems where you have explicit permission. The authors are not responsible for any misuse of this software.

---

## ✨ Features

Every 6 minutes the scheduler randomly picks **one enabled feature** and triggers it. Each feature shows a message or performs a harmless action.

| # | Feature | What it does | How to undo |
|---|---------|-------------|------------|
| 1 | **Terminal Bell** | Emits a loud beep sound, prints a fake "system alert" to terminal | Just endure it or mute |
| 2 | **Random Message** | Prints random funny/scary fake system messages | Just read it |
| 3 | **Block Screen** | Displays a black overlay with instructions, blocks input for 42 seconds | Press * key or wait |
| 4 | **Screen Flash** | Flashes the terminal or X11 display with colors (6 seconds) | Wait a moment |
| 5 | **Alert Screen** | Prints text in reverse video mode with error messages | Auto-resets after 3 seconds |
| 6 | **Calendar Joke** | Shows a fake calendar warning or date anomaly | Just dismiss it |
| 7 | **System Info Spoof** | Prints fake system diagnostic information | Just ignore it |
| 8 | **Screen Rotate** | Rotates display through 4 orientations for 42 seconds | Auto-reverts after 42 seconds |
| 9 | **Keyboard Swap** | Swaps keyboard layout between Portuguese and US for 42 seconds | Auto-reverts after 42 seconds |

All 9 features are **ON by default** and can be individually toggled from the `features.sh` menu.

---

## 📋 Requirements

* Linux Operating System (Ubuntu, Debian, Fedora, CentOS, Arch, etc.)
* GCC compiler (for building from source)
* `make` utility
* Standard C library (libc)
* Optional: X11 libraries for advanced features (`libx11-dev`, `libxext-dev`)

### Installation of Build Dependencies

**Ubuntu/Debian:**
```bash
apt-get update
apt-get install build-essential
# Optional: for X11 features
apt-get install libx11-dev libxext-dev
```

**Fedora/RHEL/CentOS:**
```bash
dnf groupinstall "Development Tools"
# Optional: for X11 features
dnf install libX11-devel libXext-devel
```

**Arch:**
```bash
pacman -S base-devel
# Optional: for X11 features
pacman -S libx11 libxext
```

---

## 🚀 Installation & Usage

### Step 1: One-Command Install (Recommended)

```bash
cd AnoyLinux
./install_and_start.sh
```

This does everything automatically:
- Runs `setup.sh` (compile + install + cron setup)
- Enables scheduler
- Triggers one prank immediately

Optional (install only, no immediate prank):

```bash
./install_and_start.sh --no-now
```

### Step 2: Manual Install (Alternative)

### Compile the Application

```bash
cd AnoyLinux
make
```

This generates the `AnoyPC` binary.

### Run the Setup Script

```bash
./setup.sh
```

The setup script will automatically:
- Create `~/.anoypc/` directory with all necessary files
- Copy the compiled `AnoyPC` binary
- Enable all 9 prank features (via marker files)
- Register a cron job to fire every 6 minutes
- Create convenience symlinks in `~/.local/bin/` for easy access

### Done!

Pranks will start executing every 6 minutes. You'll see messages in `~/.anoypc/anoypc.log`.

---

## 📁 Post-Installation Control Scripts

All scripts in `~/.anoypc/` can be run directly:

| File | Purpose |
| :--- | :--- |
| **`features.sh`** | Interactive menu — toggle individual prank features ON or OFF. |
| **`test.sh`** | Interactive menu — immediately trigger any individual prank for testing. |
| **`anoyoff.sh`** | Temporarily disables the cron job (all features paused). |
| **`anoyon.sh`** | Re-enables the cron job (all features resume). |
| **`anoydel.sh`** | **Complete removal** — deletes the cron job and the entire `~/.anoypc` folder. |
| **`run.sh`** | Internal runner that executes a random or specified prank. |

---

## 📁 Directory Structure

```
AnoyLinux/
├── README.md                # This file
├── Makefile                 # Build configuration
├── AnoyPC.c                 # Main C source code (all logic)
├── setup.sh                 # Installation script
└── (Generated after compilation)
    └── AnoyPC               # Compiled binary
```

### How Feature Toggling Works

Each feature is controlled by a small marker file (`feat_FEATURENAME.on`) inside `~/.anoypc/`:

- **File exists** → feature is **ON**
- **File absent** → feature is **OFF**

The `features.sh` menu creates or deletes these files when you toggle a feature. The main C program checks for these files each time it runs and only picks from enabled features.

---

## 🔧 How It Works

### Build Process

```bash
make clean      # Remove old binaries
make            # Compile the program
make install    # Install to system (optional)
```

### Cron Schedule

The setup registers a cron job that runs:

```bash
*/6 * * * * ~/.anoypc/run.sh
```

This means: every 6 minutes, execute `run.sh`, which calls the compiled `AnoyPC` binary with a random or specified feature.

### Logging

All prank executions are logged to `~/.anoypc/anoypc.log` for debugging and entertainment purposes.

---

## 🛠️ Development & Customization

### Adding a New Feature

1. Edit `AnoyPC.c` and add a new function (e.g., `void feature_my_prank()`)
2. Add the feature to the `FEATURES` enum
3. Register it in the `run_feature()` function
4. Create a marker file: `feat_MYPRANK.on`
5. Recompile: `make clean && make`

### Adjusting Schedule

Edit `setup.sh` and change the cron interval:

```bash
# Current: every 6 minutes
*/6 * * * * ~/.anoypc/run.sh

# Change to every 1 minute:
* * * * * ~/.anoypc/run.sh

# Change to every 10 minutes:
*/10 * * * * ~/.anoypc/run.sh
```

---

## 🔐 Security & Compatibility

✅ **Completely Harmless:**
- No file modifications outside `~/.anoypc/`
- No system file damage
- No permission escalations
- All changes are reversible via `anoydel.sh`

✅ **Portability:**
- Pure C with minimal dependencies
- POSIX-compliant code
- Works on most Linux distributions

⚠️ **X11 Features:**
- Advanced features (mouse hide/show) require X11
- Falls back gracefully on Wayland or headless systems
- Features degrade gracefully if X11 is unavailable

---

## 📝 Example Commands

```bash
# View all features and their status
~/.anoypc/features.sh

# Test feature #1 (Terminal Bell)
~/.anoypc/test.sh

# Temporarily disable all pranks
~/.anoypc/anoyoff.sh

# Re-enable all pranks
~/.anoypc/anoyon.sh

# View logs
tail -f ~/.anoypc/anoypc.log

# Remove everything (careful!)
~/.anoypc/anoydel.sh
```

---

## 📋 Cron vs Other Schedulers

This implementation uses **cron** for scheduling because it's:
- ✅ Universal on Linux systems
- ✅ Simple and reliable
- ✅ Already running in the background
- ✅ No additional services needed

If you prefer **systemd timer** instead of cron, see the optional `systemd-setup.sh` (not included by default but can be added on request).

---

## 🐛 Troubleshooting

### "Permission denied" when running setup.sh

```bash
chmod +x setup.sh
./setup.sh
```

### Pranks not executing

Check the log file:
```bash
cat ~/.anoypc/anoypc.log
tail -f ~/.anoypc/anoypc.log
```

Check cron status:
```bash
crontab -l
```

### X11 features not working

This is expected on Wayland or headless systems. The program will fall back to terminal-based features.

---

## 📜 License

This project is provided as-is for entertainment purposes. Use responsibly!

---

## 👨‍💻 Author

Created by **CreaTico6** - Linux Edition by **Assistant**

Made with ❤️ and a sense of humor 🎭

---

## 🎉 Have Fun!

Remember: pranks are best when everyone involved can laugh about it afterwards! 😄
