# AnoyingPC Linux 🎭

A minimalist Linux prank suite that randomly triggers harmless, annoying effects.
All pranks are safe, reversible, and user-controllable.

---

## ⚡ Quick Start

Execute this:
```bash
git clone https://github.com/CreaTico6/AnoyLinux.git && cd AnoyLinux && make install 
```

1. Clone/download this repo
2. Run `./install.sh`
3. Pranks start immediately and every 6 minutes

**Control & Removal:**  
Scripts in `~/.anoypc/` let you toggle features (`features.sh`), test pranks (`test.sh`), pause/resume (`anoyoff.sh`/`anoyon.sh`), or remove everything (`anoydel.sh`).

---

## ✨ Features

- Terminal Bell: Loud beep + fake alert
- Random Message: Fake/funny system messages
- Block Screen: Black overlay, blocks input (42s)
- Screen Flash: Flashes colors
- Alert Screen: Reverse video error
- Calendar Joke: Fake date anomaly
- System Info Spoof: Fake diagnostics
- Upside Down: Inverts display (42s)
- Caps On: Forces Caps Lock
- Mouse Jitter: Moves mouse randomly
- Brightness Pulse: Pulses screen brightness
- Matrix Fullscreen: Matrix animation with message

All features can be toggled individually.

---

## 📋 Requirements

- Linux (Ubuntu, Fedora, Arch, etc.)
- GCC, make
- Optional: X11 libraries for advanced effects

---

## 🚀 Install & Usage

```bash
cd AnoyLinux
./install_and_start.sh
```
Or, for manual install:
```bash
make
./setup.sh
```
Pranks run every 6 minutes (via cron). Control scripts are in `~/.anoypc/`.

---

## 🔧 How It Works

- Each prank is toggled by a marker file in `~/.anoypc/`
- All executions are logged to `~/.anoypc/anoypc.log`
- Cron job: `*/6 * * * * ~/.anoypc/run.sh`

---

## 🛠️ Customization

- Add new pranks: Edit `AnoyPC.c`, update the enum and dispatcher, recompile.
- Change schedule: Edit cron line in `setup.sh`.

---

## 🐛 Troubleshooting

- Make scripts executable: `chmod +x setup.sh`
- Check logs: `tail -f ~/.anoypc/anoypc.log`
- Check cron: `crontab -l`
- X11 features require X11 (fallbacks provided)

---

## 📜 License & Author

Provided as-is for fun.  
Created by **CreaTico6**
---
## 🎉 Have Fun!