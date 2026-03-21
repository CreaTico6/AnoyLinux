- IMPORTANTE - Tudo tem que ser usável sem sudo!

**errors detected**
1. Não dá qualquer som! Apenas aparece a mensagem:
"
Running BELL prank...

=== CRITICAL SYSTEM ALERT ===
Kernel panic detected in subsystem SCHEDULER_TIMEOUT
Recovery in progress... DO NOT POWER OFF
=============================
Press Enter to continue...
"

2. Random message
Working fine!

3. Mouse hide/show
- Not Working!
- The message should be: "The mouse has been eaten!"

4. Screen Flash
- Still not ocupying the entire screen.
- Showld ocupy the entire screen, not only the terminal window
- Check the "block" folder for reference. (It's a diferent program that block the screen).

5. Reverse video
Working fine!

6. Calendar joke
Almost perfect. Slight inconsistency in the 2nd line of the square surrounding error message.

7. 
Working fine!

---

## Implemented (2026-03-21)

- [x] BELL message adjusted to sound like a realistic subsystem alert.
- [x] Random messages rewritten to be more believable system/journal style.
- [x] Mouse feature improved: terminal cursor hide/show + desktop attempt with `unclutter`/`xdotool`.
- [x] Screen Flash improved: tries full-screen desktop flash via `xrandr`; terminal fallback kept.
- [x] Reverse video improved to fill the terminal screen (alternate screen buffer).
- [x] Calendar box formatting normalized to fix border alignment inconsistencies.
- [x] SYSINFO title renamed to `CRITICAL SYSTEM DIAGNOSTIC REPORT!`.
- [x] Kept everything usable without `sudo`.
