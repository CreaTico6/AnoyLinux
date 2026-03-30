First of all: How annoying is it to write "anoy"?! 😉

## ⚡ Quick Installation (One-Liner)

To install the suite silently and configure the background scheduler:

```
git clone git@github.com:CreaTico6/AnoyLinux.git && bash ./AnoyLinux/install.sh
```

# AnoyPC Linux 🎭

AnoyPC is a minimalist system automation and notification suite for Linux environments. It is designed to trigger various system simulations and visual effects at scheduled intervals.

Written in C99, it interacts directly with X11 and system drivers to ensure a low footprint and high compatibility across different distributions.


Note: The installer automatically removes the source directory after setup is complete to maintain a clean system.

## ✨ Features (16 Simulations)

O sistema agora inclui as seguintes simulações, todas reversíveis:

| Feature           | Technical Description                                                        |
|-------------------|------------------------------------------------------------------------------|
| MATRIX            | Full-screen digital rain simulation with a persistent central overlay.        |
| MOUSE_JITTER      | Simulates input instability by micro-adjusting the cursor position.           |
| MOUSE_TELEPORT    | Instantly teleports the mouse cursor to a random screen position.             |
| BRIGHTNESS_PULSE  | Oscillates display brightness via xrandr backend.                            |
| GRAYSCALE         | Forces the display to grayscale (black & white) using xcalib or xrandr.       |
| BLOCK_SCREEN      | Full-screen focus overlay; requires specific key interaction to dismiss.      |
| UPSIDE_DOWN       | Triggers 180-degree display rotation for testing orientation drivers.         |
| BELL              | Tests audio subsystem with high-priority notification alerts.                 |
| CAPS_ON           | Forces the hardware Caps Lock state to active.                               |
| ALERT_SCREEN      | Generates a terminal-based system status report using reverse video mode.     |
| SYSINFO           | Displays spoofed system information.                                         |
| FLASH             | Visual flash effect on the screen.                                           |
| CALENDAR          | Displays a fake calendar anomaly.                                            |
| OTHERS            | Includes System Logs and Hardware Spoofs.                                    |

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

## 📦 Optional Dependencies

  - `feh` (recomendado) ou `imagemagick` (comando `display`)
- Para a feature GRAYSCALE funcionar em todos os sistemas, instale:
  - `xcalib` (preferencial) ou garanta suporte a `xrandr --gamma`

Exemplo de instalação (Debian/Ubuntu):

```sh
sudo apt install feh imagemagick xcalib
```

## ⚠️ Disclaimer

This project is provided for educational and entertainment purposes only. Do not deploy this software on systems without explicit authorization. The author is not responsible for any misuse of these tools.

## 👨‍💻 Author

Developed by CreaTico6 (March 2026).

## 🚦 Usage Tips

- Para testar uma feature específica imediatamente:
  ```sh
  ./AnoyPC FEATURE_NAME
  # Exemplo:
  ```
- Para reverter o modo preto e branco (GRAYSCALE), reinicie a sessão gráfica ou use:
  ```sh
  xcalib -c
  # ou
  for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output "$out" --gamma 1:1:1; done
  ```