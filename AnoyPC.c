

/*
 * AnoyingPC Linux - Main Prank Application in C
 * 
 * A minimalist prank application that executes harmless-but-annoying
 * system disruptions on Linux. Features are toggled via marker files
 * and executed via a cron scheduler (every 6 minutes).
 * 
 * Compilation: gcc -o AnoyPC AnoyPC.c
 * Usage: ./AnoyPC [FEATURE_NAME]
 *   - If FEATURE_NAME is omitted, a random enabled feature is selected
 *   - If FEATURE_NAME is provided, that specific feature is executed
 * 
 * Author: CreaTico6@gmail.com (Linux Edition)
 * Date: March 2026
 */

#define _DEFAULT_SOURCE  /* For usleep - modern replacement for _BSD_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>  /* For usleep */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

/* ==============================================================================
 * FEATURE DEFINITIONS - Define all available prank features here
 * ============================================================================*/

/* Enum to represent all available features */
typedef enum {
    FEAT_BELL,        /* 1. Terminal Bell - Emits loud beep */
    FEAT_MESSAGE,     /* 2. Random Message - Prints fake system message */
    FEAT_BLOCK_SCREEN,/* 3. Block Screen - Full-screen black overlay */
    FEAT_FLASH,       /* 4. Screen Flash - Flashes terminal with colors */
    FEAT_REVERSE,     /* 5. Alert Screen - Reverse video warning screen */
    FEAT_CALENDAR,    /* 6. Calendar Joke - Shows fake date warning */
    FEAT_SYSINFO,     /* 7. System Info Spoof - Prints fake system info */
    FEAT_UPSIDE_DOWN,   /* 8. UPSIDE_DOWN - Inverts display for 42 seconds */
    FEAT_CAPS_ON,      /* 9. CAPS_ON - Forces CAPS LOCK on */
    FEAT_COUNT        /* Total number of features (used for random selection) */
} Feature;

/* Array of feature names (must match enum order) */
static const char* FEATURE_NAMES[] = {
    "BELL",
    "MESSAGE",
    "BLOCK_SCREEN",
    "FLASH",
    "ALERT_SCREEN",
    "CALENDAR",
    "SYSINFO",
    "UPSIDE_DOWN",
    "CAPS_ON"
};

/* ==============================================================================
 * UTILITY FUNCTIONS - Logging and file management
 * ============================================================================*/

/* 
 * Log a message to the AnoyPC log file
 * path: Home directory path
 * message: Message to log
 */
void log_message(const char* homedir, const char* message) {
    FILE* logfile;
    time_t now;
    struct tm* timeinfo;
    char timestamp[100];
    char logpath[512];
    
    /* Construct log file path: ~/.anoypc/anoypc.log */
    snprintf(logpath, sizeof(logpath), "%s/.anoypc/anoypc.log", homedir);
    
    /* Open in append mode */
    logfile = fopen(logpath, "a");
    if (logfile == NULL) {
        /* Log directory might not exist yet; create it */
        return;
    }
    
    /* Get current time and format it */
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    /* Write log entry */
    fprintf(logfile, "[%s] %s\n", timestamp, message);
    fclose(logfile);
}

/*
 * Get the home directory of the current user
 * Returns: pointer to static buffer containing home directory path
 */
const char* get_home_dir(void) {
    const char* homedir = getenv("HOME");
    
    if (homedir == NULL) {
        /* Fallback: use getpwuid to get home directory */
        struct passwd* pw = getpwuid(getuid());
        if (pw != NULL) {
            homedir = pw->pw_dir;
        } else {
            homedir = "/tmp";
        }
    }
    
    return homedir;
}

/*
 * Execute a shell command where failure is non-fatal by design.
 * We still store the return value to avoid compiler warnings.
 */
static void run_command_best_effort(const char* cmd) {
    int rc = system(cmd);
    (void)rc;
}

/*
 * Check if a feature is enabled (marker file exists)
 * homedir: Home directory path
 * feature_name: Name of the feature
 * Returns: 1 if enabled (file exists), 0 otherwise
 */
int is_feature_enabled(const char* homedir, const char* feature_name) {
    char filepath[512];
    char legacy_filepath[512];
    
    /* Construct path: ~/.anoypc/feat_FEATURENAME.on */
    snprintf(filepath, sizeof(filepath), "%s/.anoypc/feat_%s.on", homedir, feature_name);

    if (strcmp(feature_name, "ALERT_SCREEN") == 0) {
        snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_REVERSE.on", homedir);
        if (access(legacy_filepath, F_OK) == 0) {
            return 1;
        }
    }

    if (strcmp(feature_name, "UPSIDE_DOWN") == 0) {
        snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_SCREEN_ROTATE.on", homedir);
        if (access(legacy_filepath, F_OK) == 0) {
            return 1;
        }
    }
    
    /* Check if file exists (accessible with read permission) */
    return access(filepath, F_OK) == 0;
}

/* ==============================================================================
 * PRANK FEATURE IMPLEMENTATIONS - Each prank is a separate function
 * ============================================================================*/

/*
 * FEATURE 1: Terminal Bell
 * Emits a loud terminal bell sound and prints a fake system alert
 */
void feature_bell(void) {
    /* Maximize volume before playing sound */
    const char* cmd_volume = "if command -v pactl >/dev/null 2>&1; then pactl set-sink-volume @DEFAULT_SINK@ 150% >/dev/null 2>&1; fi";
    
    const char* cmd_sound = "if command -v paplay >/dev/null 2>&1 && [ -f /usr/share/sounds/freedesktop/stereo/phone-incoming-call.oga ]; then paplay /usr/share/sounds/freedesktop/stereo/phone-incoming-call.oga >/dev/null 2>&1; elif command -v tput >/dev/null 2>&1; then tput bel >/dev/null 2>&1; else printf '\\a'; fi";

    /* Set volume to maximum once */
    run_command_best_effort(cmd_volume);
    
    /* Ring the bell 5 times for maximum annoyance */
    for (int i = 0; i < 5; i++) {
        printf("\a");  /* ASCII BEL character (0x07) */
        fflush(stdout);
        run_command_best_effort(cmd_sound);
        usleep(100000);  /* 100ms delay between beeps */
    }
    
    /* Print fake system alert message */
    printf("\n=== AUDIO SUBSYSTEM ALERT ===\n");
    printf("High-priority notification stream detected on tty session.\n");
    printf("Acknowledgement pending...\n");
    printf("=============================\n\n");
    fflush(stdout);
    
    usleep(1000000);  /* 1 second delay before clearing */
}

/*
 * FEATURE 2: Random Message
 * Prints one of several funny/scary fake system messages
 */
void feature_message(void) {
    /* Array of fake system messages */
    const char* messages[] = {
        "systemd[1]: Delayed job queue exceeded soft timing threshold.",
        "kernel: ACPI warning: firmware responded with stale timestamp.",
        "NetworkManager: DNS resolver switched to fallback profile.",
        "udisksd: transient I/O latency spike detected on /dev/sda.",
        "journald: log rate limit reached for user session scope.",
        "pipewire: output stream underrun, auto-recovery in progress.",
        "display-manager: monitor EDID checksum mismatch (temporary).",
        "cron: skipped one cycle due to local clock drift correction.",
        "dbus-daemon: service activation timeout reached, retry queued.",
        "kernel: thermal zone briefly crossed passive cooling threshold."
    };
    
    int num_messages = sizeof(messages) / sizeof(messages[0]);
    int random_msg = rand() % num_messages;
    
    char formatted[44];
    snprintf(formatted, sizeof(formatted), "%-43.43s", messages[random_msg]);

    printf("\n┌─────────────────────────────────────────────┐\n");
    printf("│ SYSTEM EVENT LOG                            │\n");
    printf("├─────────────────────────────────────────────┤\n");
    printf("│ %s │\n", formatted);
    printf("└─────────────────────────────────────────────┘\n\n");
    fflush(stdout);
    
    sleep(30);  /* Display message for 30 seconds */
}

/*
 * FEATURE 3: Block Screen
 * Displays a full-screen black overlay with instructions.
 * Exit only with '*' key or timeout (42 seconds).
 */
void feature_block_screen(void) {
    Display* display;
    int screen;
    int sw;
    int sh;
    Window window;
    XSetWindowAttributes attr;
    Pixmap blank_pixmap;
    Cursor blank_cursor;
    XColor dummy;
    GC gc;
    time_t started_at;
    const int timeout_seconds = 42;
    struct timespec tick = {0, 100000000};
    const char* line1 = "The answer for everything, is also the way to exit this screen. (only one char).";
    char line2[160];

    display = XOpenDisplay(NULL);
    if (!display) {
        printf("\n[BLOCK_SCREEN] X11 display unavailable.\n\n");
        fflush(stdout);
        sleep(2);
        return;
    }

    screen = DefaultScreen(display);
    sw = DisplayWidth(display, screen);
    sh = DisplayHeight(display, screen);

    attr.override_redirect = True;
    attr.background_pixel = BlackPixel(display, screen);

    window = XCreateWindow(display, RootWindow(display, screen),
                           0, 0, (unsigned int)sw, (unsigned int)sh,
                           0, CopyFromParent, InputOutput, CopyFromParent,
                           CWOverrideRedirect | CWBackPixel, &attr);

    XMapRaised(display, window);
    XSelectInput(display, window, KeyPressMask | ExposureMask);
    XSetInputFocus(display, window, RevertToParent, CurrentTime);
    XFlush(display);

    blank_pixmap = XCreatePixmap(display, window, 1, 1, 1);
    blank_cursor = XCreatePixmapCursor(display, blank_pixmap, blank_pixmap,
                                       &dummy, &dummy, 0, 0);
    XDefineCursor(display, window, blank_cursor);
    XGrabPointer(display, window, False,
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                 GrabModeAsync, GrabModeAsync,
                 None, blank_cursor, CurrentTime);
    XGrabKeyboard(display, window, True,
                  GrabModeAsync, GrabModeAsync, CurrentTime);
    XFreeCursor(display, blank_cursor);
    XFreePixmap(display, blank_pixmap);

    gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, WhitePixel(display, screen));

    started_at = time(NULL);
    while (1) {
        time_t now = time(NULL);
        int elapsed = (int)(now - started_at);
        int remaining = timeout_seconds - elapsed;

        if (remaining <= 0) {
            break;
        }

        XClearWindow(display, window);
        XSetForeground(display, gc, WhitePixel(display, screen));
        XDrawString(display, window, gc, 40, (sh / 2) - 20, line1, (int)strlen(line1));
        snprintf(line2, sizeof(line2), "Anyway if you cannot figure it out, this screen will exit in %d", remaining);
        XDrawString(display, window, gc, 40, (sh / 2) + 10, line2, (int)strlen(line2));
        XFlush(display);

        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                char buffer[8] = {0};
                KeySym keysym = 0;
                int count = XLookupString(&event.xkey, buffer, sizeof(buffer), &keysym, NULL);
                if (keysym == XK_KP_Multiply || keysym == XK_asterisk || (count > 0 && buffer[0] == '*')) {
                    XFreeGC(display, gc);
                    XUngrabKeyboard(display, CurrentTime);
                    XUngrabPointer(display, CurrentTime);
                    XUndefineCursor(display, window);
                    XDestroyWindow(display, window);
                    XFlush(display);
                    XCloseDisplay(display);
                    return;
                }
            }
        }

        nanosleep(&tick, NULL);
    }

    XFreeGC(display, gc);
    XUngrabKeyboard(display, CurrentTime);
    XUngrabPointer(display, CurrentTime);
    XUndefineCursor(display, window);
    XDestroyWindow(display, window);
    XFlush(display);
    XCloseDisplay(display);
}

/*
 * FEATURE 4: Screen Flash
 * Flashes the terminal with colors or escape sequences
 * Creates a visual distraction effect
 */
void feature_flash(void) {
    int used_fullscreen = 0;
    const char* cmd_flash_tk = "python3 -c \"import tkinter as t,time; r=t.Tk(); r.attributes('-fullscreen',True); r.attributes('-topmost',True); r.configure(bg='black'); r.update(); [(r.configure(bg=c), r.update(), time.sleep(0.20)) for c in (['white','black']*15)]; r.destroy()\" >/dev/null 2>&1";

    printf("\n");
    fflush(stdout);

    /* Full-screen desktop flash attempt (no sudo required) */
    if (getenv("DISPLAY") != NULL && system(cmd_flash_tk) == 0) {
        used_fullscreen = 1;
    }

    if (!used_fullscreen && getenv("DISPLAY") != NULL && system("command -v xrandr >/dev/null 2>&1") == 0) {
        used_fullscreen = 1;
        for (int i = 0; i < 15; i++) {
            run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness 0.20 >/dev/null 2>&1; done");
            usleep(200000);
            run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness 1.00 >/dev/null 2>&1; done");
            usleep(200000);
        }
    }

    /* Fallback for TTY/terminals */
    if (!used_fullscreen) {
        printf("\033[2J\033[H");
        for (int i = 0; i < 30; i++) {
            printf("\033[41m████████████████████████████████████████████████████████████████\033[0m\n");
            printf("\033[43m████████████████████████████████████████████████████████████████\033[0m\n");
            fflush(stdout);
            usleep(200000);
        }
    }

    printf("\033[0m");
    printf("\n>>> Display refresh cycle complete <<<\n\n");
    fflush(stdout);
}

/*
 * FEATURE 5: Alert Screen
 * Prints text in reverse video mode (white text on black background)
 * Creates an eerie "system error" appearance
 */
void feature_reverse(void) {
    int rows = 24;
    int cols = 80;
    const char* env_lines = getenv("LINES");
    const char* env_cols = getenv("COLUMNS");

    if (env_lines != NULL) {
        int parsed = atoi(env_lines);
        if (parsed >= 10 && parsed <= 120) {
            rows = parsed;
        }
    }

    if (env_cols != NULL) {
        int parsed = atoi(env_cols);
        if (parsed >= 40 && parsed <= 240) {
            cols = parsed;
        }
    }

    printf("\033[?1049h\033[2J\033[H\033[7m");
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            putchar(' ');
        }
        putchar('\n');
    }

    printf("\033[%d;%dHCRITICAL SYSTEM ERROR DETECTED", rows / 2 - 1, (cols / 2) - 14);
    printf("\033[%d;%dHUNAUTHORIZED ACCESS ATTEMPT", rows / 2, (cols / 2) - 13);
    printf("\033[%d;%dHPROTECTION MODE ENABLED", rows / 2 + 1, (cols / 2) - 11);
    fflush(stdout);

    sleep(6);

    printf("\033[0m\033[?1049l");
    printf("\n>>> Alert screen cycle complete <<<\n\n");
    fflush(stdout);
}

/*
 * FEATURE 6: Calendar Joke
 * Shows a fake calendar warning or date-related anomaly
 */
void feature_calendar(void) {
    time_t now;
    struct tm* timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    
    int fake_day = (timeinfo->tm_mday % 31) + 1;
    int fake_month = (timeinfo->tm_mon % 12) + 1;
    
        char current_date[64];
        char recorded_date[64];

        snprintf(current_date, sizeof(current_date), "Current system date: %04d-%02d-%02d",
               timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
        snprintf(recorded_date, sizeof(recorded_date), "Recorded date: %04d-%02d-%02d",
               timeinfo->tm_year + 1900, fake_month, fake_day);

        printf("\n╔════════════════════════════════════════════╗\n");
        printf("║ CALENDAR ANOMALY DETECTED!                 ║\n");
        printf("║ ------------------------------------------ ║\n");
        printf("║ %-42s ║\n", current_date);
        printf("║ %-42s ║\n", recorded_date);
        printf("║                                            ║\n");
        printf("║ WARNING: Temporal inconsistency detected!  ║\n");
        printf("║ Your system clock may be unstable.         ║\n");
        printf("╚════════════════════════════════════════════╝\n\n");
    fflush(stdout);
    
    sleep(3);
}

/*
 * FEATURE 7: System Info Spoof
 * Prints fake system diagnostic information
 * Mimics output of system information commands
 */
void feature_sysinfo(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║ CRITICAL SYSTEM DIAGNOSTIC REPORT!                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("CPU Status:        WARNING - Throttled by 67%%\n");
    printf("RAM Usage:         CRITICAL - 94.2%% utilized\n");
    printf("Disk I/O:          FAULT - Write cache corrupted\n");
    printf("Network:           UNSTABLE - 47 packet losses\n");
    printf("Thermal:           CRITICAL - CPU at 89°C\n");
    printf("Power:             ABNORMAL - Battery degradation 33%%\n");
    printf("\n");
    printf("RECOMMENDATION: Contact your system administrator IMMEDIATELY\n");
    printf("Reference code: PANIC-0xDEADBEEF-8842-LINUX\n");
    printf("\n");
    fflush(stdout);
    
    sleep(3);
}

/*
 * FEATURE 8: UPSIDE_DOWN (legacy SCREEN_ROTATE alias)
 * Inverts the display for 42 seconds, then reverts to normal.
 */
void feature_upside_down(void) {
    printf("\n>>> Display upside-down mode initiated (42s, then reverts) <<<\n");
    fflush(stdout);
    
    if (getenv("DISPLAY") == NULL) {
        printf(">>> No X11 display found <<<\n\n");
        fflush(stdout);
        sleep(1);
        return;
    }
    
    run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --rotate inverted >/dev/null 2>&1; done");
    printf("  Display inverted. Reverting in 42 seconds...\n");
    fflush(stdout);
    sleep(42);
    
    /* Revert to normal rotation */
    run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --rotate normal >/dev/null 2>&1; done");
    printf("\n>>> Display reverted to normal <<<\n\n");
    fflush(stdout);
}

/*
 * FEATURE 9: CAPS_ON (legacy KEYBOARD_SWAP alias)
 * Silently forces CAPS LOCK on.
 */
void feature_keyboard_swap(void) {
    if (getenv("DISPLAY") == NULL) {
        return;
    }

    Display* display = XOpenDisplay(NULL);
    if (display != NULL) {
        unsigned int indicator_state = 0;

        /* Check current state and try XKB lock */
        if (XkbGetIndicatorState(display, XkbUseCoreKbd, &indicator_state) == Success) {
            int is_on = (indicator_state & 0x01U) != 0;
            
            /* Try to toggle CAPS - may not work on Xwayland, but try anyway */
            XkbLockModifiers(display, XkbUseCoreKbd, LockMask, is_on ? 0 : LockMask);
            XFlush(display);
            usleep(50000);
        }

        XCloseDisplay(display);
    }

    /* Fallback: try setxkbmap to reset keyboard state (may indirectly affect CAPS in some environments) */
    run_command_best_effort("setxkbmap -layout us >/dev/null 2>&1");
    
    /* Last resort: try xset indicators */
    run_command_best_effort("xset led 1 >/dev/null 2>&1");
}

/* ==============================================================================
 * MAIN FEATURE EXECUTION DISPATCHER
 * ============================================================================*/

/*
 * Run a specific feature by name
 * feature_name: Name of the feature (must match FEATURE_NAMES array)
 */
void run_feature(const char* feature_name) {
    /* Convert feature name to uppercase for comparison */
    char upper_name[64];
    strncpy(upper_name, feature_name, sizeof(upper_name) - 1);
    upper_name[sizeof(upper_name) - 1] = '\0';
    
    for (int i = 0; upper_name[i]; i++) {
        if (upper_name[i] >= 'a' && upper_name[i] <= 'z') {
            upper_name[i] -= 32;  /* Convert to uppercase */
        }
    }
    
    /* Dispatch to appropriate feature function */
    if (strcmp(upper_name, "BELL") == 0) {
        feature_bell();
    } else if (strcmp(upper_name, "MESSAGE") == 0) {
        feature_message();
    } else if (strcmp(upper_name, "BLOCK_SCREEN") == 0 || strcmp(upper_name, "MOUSE") == 0) {
        feature_block_screen();
    } else if (strcmp(upper_name, "FLASH") == 0) {
        feature_flash();
    } else if (strcmp(upper_name, "ALERT_SCREEN") == 0 || strcmp(upper_name, "REVERSE") == 0) {
        feature_reverse();
    } else if (strcmp(upper_name, "CALENDAR") == 0) {
        feature_calendar();
    } else if (strcmp(upper_name, "SYSINFO") == 0) {
        feature_sysinfo();
            } else if (strcmp(upper_name, "UPSIDE_DOWN") == 0 || strcmp(upper_name, "SCREEN_ROTATE") == 0) {
                feature_upside_down();
    } else if (strcmp(upper_name, "CAPS_ON") == 0 || strcmp(upper_name, "KEYBOARD_SWAP") == 0) {
        feature_keyboard_swap();
    } else {
        fprintf(stderr, "ERROR: Unknown feature '%s'\n", feature_name);
        exit(1);
    }
}

/* ==============================================================================
 * RANDOM FEATURE SELECTION AND EXECUTION
 * ============================================================================*/

/*
 * Select and execute a random enabled feature
 * homedir: Home directory path
 */
void run_random_feature(const char* homedir) {
    Feature enabled_features[FEAT_COUNT];
    int enabled_count = 0;
    
    /* Build list of enabled features */
    for (int i = 0; i < FEAT_COUNT; i++) {
        if (is_feature_enabled(homedir, FEATURE_NAMES[i])) {
            enabled_features[enabled_count++] = i;
        }
    }
    
    /* If no features are enabled, exit silently */
    if (enabled_count == 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "No features enabled. Exiting.");
        log_message(homedir, msg);
        return;
    }
    
    /* Select a random enabled feature */
    int random_idx = rand() % enabled_count;
    Feature chosen_feature = enabled_features[random_idx];
    
    /* Log the execution */
    char msg[256];
    snprintf(msg, sizeof(msg), "Executing random feature: %s", 
             FEATURE_NAMES[chosen_feature]);
    log_message(homedir, msg);
    
    /* Execute the chosen feature */
    run_feature(FEATURE_NAMES[chosen_feature]);
}

/* ==============================================================================
 * MAIN FUNCTION
 * ============================================================================*/

/*
 * Main entry point
 * 
 * Usage:
 *   ./AnoyPC                    # Run a random enabled feature
 *   ./AnoyPC BELL               # Run the BELL feature specifically
 *   ./AnoyPC MESSAGE            # Run the MESSAGE feature specifically
 *   etc.
 */
int main(int argc, char* argv[]) {
    const char* homedir;
    
    /* Seed the random number generator */
    srand(time(NULL) ^ getpid());
    
    /* Get home directory */
    homedir = get_home_dir();
    
    /* Check if ~/.anoypc directory exists */
    char anoypc_dir[512];
    snprintf(anoypc_dir, sizeof(anoypc_dir), "%s/.anoypc", homedir);
    
    if (access(anoypc_dir, F_OK) != 0) {
        fprintf(stderr, "ERROR: ~/.anoypc directory does not exist.\n");
        fprintf(stderr, "Please run './setup.sh' first.\n");
        return 1;
    }
    
    /* If a feature name is provided as argument, run that specific feature */
    if (argc > 1) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Executing requested feature: %s", argv[1]);
        log_message(homedir, msg);
        run_feature(argv[1]);
    } else {
        /* Otherwise, run a random enabled feature */
        run_random_feature(homedir);
    }
    
    return 0;
}
