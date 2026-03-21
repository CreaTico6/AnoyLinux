

/*
 * AnoyingPC Linux - Main Prank Application in C
 * 
 * A minimalist prank application that executes harmless-but-annoying
 * system disruptions on Linux. Features are toggled via marker files
 * and executed via a cron scheduler (every 5 minutes).
 * 
 * Compilation: gcc -o AnoyPC AnoyPC.c
 * Usage: ./AnoyPC [FEATURE_NAME]
 *   - If FEATURE_NAME is omitted, a random enabled feature is selected
 *   - If FEATURE_NAME is provided, that specific feature is executed
 * 
 * Author: CreaTico6 (Linux Edition)
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

/* ==============================================================================
 * FEATURE DEFINITIONS - Define all available prank features here
 * ============================================================================*/

/* Enum to represent all available features */
typedef enum {
    FEAT_BELL,        /* 1. Terminal Bell - Emits loud beep */
    FEAT_MESSAGE,     /* 2. Random Message - Prints fake system message */
    FEAT_MOUSE,       /* 3. Mouse Hide/Show - Toggles mouse cursor visibility */
    FEAT_FLASH,       /* 4. Screen Flash - Flashes terminal with colors */
    FEAT_REVERSE,     /* 5. Reverse Video - Prints text in reverse mode */
    FEAT_CALENDAR,    /* 6. Calendar Joke - Shows fake date warning */
    FEAT_SYSINFO,     /* 7. System Info Spoof - Prints fake system info */
    FEAT_COUNT        /* Total number of features (used for random selection) */
} Feature;

/* Array of feature names (must match enum order) */
static const char* FEATURE_NAMES[] = {
    "BELL",
    "MESSAGE",
    "MOUSE",
    "FLASH",
    "REVERSE",
    "CALENDAR",
    "SYSINFO"
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
 * Check if a feature is enabled (marker file exists)
 * homedir: Home directory path
 * feature_name: Name of the feature
 * Returns: 1 if enabled (file exists), 0 otherwise
 */
int is_feature_enabled(const char* homedir, const char* feature_name) {
    char filepath[512];
    
    /* Construct path: ~/.anoypc/feat_FEATURENAME.on */
    snprintf(filepath, sizeof(filepath), "%s/.anoypc/feat_%s.on", homedir, feature_name);
    
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
    /* Ring the bell 5 times for maximum annoyance */
    for (int i = 0; i < 5; i++) {
        printf("\a");  /* ASCII BEL character (0x07) */
        fflush(stdout);
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
    
    sleep(3);  /* Display message for 3 seconds */
}

/*
 * FEATURE 3: Mouse Hide/Show
 * Attempts to hide or show mouse cursor using xdotool or X11 commands
 * Falls back gracefully if X11 is not available
 */
void feature_mouse(void) {
    int action = rand() % 2;  /* 0=hide, 1=show */
    const char* action_name = (action == 0) ? "HIDING" : "SHOWING";
    const char* cmd_hide = "if command -v unclutter >/dev/null 2>&1; then unclutter --timeout 0 --jitter 0 --fork >/dev/null 2>&1; elif command -v xdotool >/dev/null 2>&1; then xdotool mousemove 9999 9999 >/dev/null 2>&1; fi";
    const char* cmd_show = "pkill -x unclutter >/dev/null 2>&1 || true; if command -v xdotool >/dev/null 2>&1; then xdotool mousemove 50%% 50%% >/dev/null 2>&1; fi";
    
    printf("\n>>> Mouse cursor %s... <<<\n", action_name);
    fflush(stdout);
    
    /* Always affect terminal cursor, then attempt desktop cursor controls */
    if (action == 0) {
        printf("\033[?25l");  /* Hide terminal cursor */
        fflush(stdout);
        (void)system(cmd_hide);
        printf(">>> Cursor hidden (terminal + desktop attempt) <<<\n");
    } else {
        printf("\033[?25h");  /* Show terminal cursor */
        fflush(stdout);
        (void)system(cmd_show);
        printf(">>> Cursor restored (terminal + desktop attempt) <<<\n");
    }
    
    printf("\n");
    fflush(stdout);
    sleep(2);
}

/*
 * FEATURE 4: Screen Flash
 * Flashes the terminal with colors or escape sequences
 * Creates a visual distraction effect
 */
void feature_flash(void) {
    int used_fullscreen = 0;

    printf("\n");
    fflush(stdout);

    /* Full-screen desktop flash attempt (no sudo required) */
    if (getenv("DISPLAY") != NULL && system("command -v xrandr >/dev/null 2>&1") == 0) {
        used_fullscreen = 1;
        for (int i = 0; i < 6; i++) {
            (void)system("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness 0.20 >/dev/null 2>&1; done");
            usleep(90000);
            (void)system("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness 1.00 >/dev/null 2>&1; done");
            usleep(90000);
        }
    }

    /* Fallback for TTY/terminals */
    if (!used_fullscreen) {
        printf("\033[2J\033[H");
        for (int i = 0; i < 12; i++) {
            printf("\033[41m████████████████████████████████████████████████████████████████\033[0m\n");
            printf("\033[43m████████████████████████████████████████████████████████████████\033[0m\n");
            fflush(stdout);
            usleep(50000);
        }
    }

    printf("\033[0m");
    printf("\n>>> Display refresh cycle complete <<<\n\n");
    fflush(stdout);
}

/*
 * FEATURE 5: Reverse Video Text
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

    sleep(3);

    printf("\033[0m\033[?1049l");
    printf("\n>>> Reverse display cycle complete <<<\n\n");
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
        printf("║ CALENDAR ANOMALY DETECTED!                ║\n");
        printf("║ ────────────────────────────────────────── ║\n");
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
    } else if (strcmp(upper_name, "MOUSE") == 0) {
        feature_mouse();
    } else if (strcmp(upper_name, "FLASH") == 0) {
        feature_flash();
    } else if (strcmp(upper_name, "REVERSE") == 0) {
        feature_reverse();
    } else if (strcmp(upper_name, "CALENDAR") == 0) {
        feature_calendar();
    } else if (strcmp(upper_name, "SYSINFO") == 0) {
        feature_sysinfo();
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
