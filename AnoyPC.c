//
//	AnoyPC
//
//	by: tnuno-mo (creatico6@gmail.com)
//
//					march 2026
//

#include <stdio.h>
#include <stdlib.h>   // getenv, system, rand
#include <unistd.h>   // sleep, usleep
#include <time.h>     // time, struct timespec
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrender.h>

/* Forward declaration for utility function */
static void run_command_best_effort(const char* cmd);

/* ==============================================================================
 * New Features
 * ============================================================================*/

/*
 * FEATURE: MOUSE_SWAP
 * Switches left and right mouse buttons by remapping them in X11 for 10 seconds.
 */
void feature_mouse_swap(void) {
    Display *d = XOpenDisplay(NULL);
    if (!d) return;
    unsigned char map[256];
    int n = XGetPointerMapping(d, map, 256);
    if (n >= 3) {
        unsigned char temp = map[0];
        map[0] = map[2];
        map[2] = temp;
        XSetPointerMapping(d, map, n);
        XFlush(d);
        sleep(10);
        // Reverter
        temp = map[0];
        map[0] = map[2];
        map[2] = temp;
        XSetPointerMapping(d, map, n);
        XFlush(d);
    }
    XCloseDisplay(d);
}

/*
 * FEATURE: CHASING_BUTTON
 * Creates a clean X11 window ("Click to Exit!") that flees when the mouse approaches.
 */
void feature_chasing_button(void) {
    Display *d = XOpenDisplay(NULL);
    if (!d) return;
    int s = DefaultScreen(d);
    int w = 150, h = 50;
    int sw = DisplayWidth(d, s), sh = DisplayHeight(d, s);
    int x = sw/2 - w/2, y = sh/2 - h/2;

    XSetWindowAttributes attr;
    attr.override_redirect = True;
    attr.background_pixel = WhitePixel(d, s);
    Window win = XCreateWindow(d, RootWindow(d, s), x, y, w, h, 2, CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect | CWBackPixel, &attr);

    XSelectInput(d, win, ExposureMask | ButtonPressMask);
    XMapRaised(d, win);

    GC gc = XCreateGC(d, win, 0, NULL);
    XSetForeground(d, gc, BlackPixel(d, s));
    const char *msg = "Click to Exit!";

    time_t start = time(NULL);
    while(time(NULL) - start < 10) { // Security timeout of 10 seconds
        while (XPending(d) > 0) {
            XEvent e;
            XNextEvent(d, &e);
            if (e.type == Expose) {
                XDrawString(d, win, gc, 35, 30, msg, strlen(msg));
            }
            if (e.type == ButtonPress) {
                goto end_chase;
            }
        }

        Window root_ret, child_ret;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        if (XQueryPointer(d, RootWindow(d, s), &root_ret, &child_ret, &root_x, &root_y, &win_x, &win_y, &mask)) {
            // Se o rato estiver muito perto da janela (margem de segurança de 40px)
            if (root_x > x - 40 && root_x < x + w + 40 && root_y > y - 40 && root_y < y + h + 40) {
                x = rand() % (sw - w);
                y = rand() % (sh - h);
                XMoveWindow(d, win, x, y);
            }
        }
        usleep(30000); // Frame delay suave
    }
end_chase:
    XFreeGC(d, gc);
    XDestroyWindow(d, win);
    XCloseDisplay(d);
}

/*
 * FEATURE: CUSTOM_CURSOR
 * Muda o cursor de todo o sistema para um ícone bizarro aleatório.
 */
void feature_custom_cursor(void) {
    Display *d = XOpenDisplay(NULL);
    if (!d) return;
    int s = DefaultScreen(d);
    
    // Array de cursores nativos X11 (incluídos em <X11/cursorfont.h>)
    unsigned int cursors[] = {
        XC_gumby,       // Boneco estranho
        XC_spider,      // Aranha
        XC_gobbler,     // Parece o Pac-Man
        XC_watch,       // Relógio de areia/bolso (dá a falsa ideia de sistema bloqueado)
        XC_coffee_mug,  // Chávena de café
        XC_fleur,       // Setas multidirecionais (confunde a direção do rato)
        XC_pirate       // Caveira
    };
    
    int num_cursors = sizeof(cursors) / sizeof(cursors[0]);
    unsigned int chosen_cursor = cursors[rand() % num_cursors];

    Cursor cursor = XCreateFontCursor(d, chosen_cursor); 
    
    // Apanha o ponteiro forçadamente e impõe o nosso cursor
    XGrabPointer(d, RootWindow(d, s), False, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime);
    XFlush(d);
    
    sleep(10); // Mantém o cursor ativado por 10 segundos
    
    XUngrabPointer(d, CurrentTime);
    XFreeCursor(d, cursor);
    XCloseDisplay(d);
}

/*
 * FEATURE: MOUSE_POOP
 * Leaves a trail of small black squares (poops) where the mouse moves.
 */
void feature_mouse_poop(void) {
    Display *d = XOpenDisplay(NULL);
    if (!d) return;
    int s = DefaultScreen(d);
    Window root = RootWindow(d, s);
    Window poops[40];
    int poop_count = 0;

    XSetWindowAttributes attr;
    attr.override_redirect = True;
    attr.background_pixel = BlackPixel(d, s);

    for(int i = 0; i < 20; i++) {
        Window root_ret, child_ret;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        if (XQueryPointer(d, root, &root_ret, &child_ret, &root_x, &root_y, &win_x, &win_y, &mask)) {
            poops[poop_count] = XCreateWindow(d, root, root_x, root_y, 10, 10, 0, CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect | CWBackPixel, &attr);
            XMapRaised(d, poops[poop_count]);
            XFlush(d);
            poop_count++;
        }
        usleep(500000); // deploys a poop every 250ms
    }
    
    sleep(3); // keep poops for 3 seconds
    for(int i = 0; i < poop_count; i++) {
        XDestroyWindow(d, poops[i]);
    }
    XCloseDisplay(d);
}

/*
 * FEATURE: CLICK_DISABLE
 * Disables the mouse click by unmapping the physical buttons (1 and 3) to 0.
 */
void feature_click_disable(void) {
    Display *d = XOpenDisplay(NULL);
    if (!d) return;
    unsigned char map[256];
    int n = XGetPointerMapping(d, map, 256);
    if (n >= 1) {
        unsigned char orig1 = map[0];
        unsigned char orig3 = map[2];
        map[0] = 0; // deactivate left click
        if (n >= 3) map[2] = 0; // deactivate right click
        XSetPointerMapping(d, map, n);
        XFlush(d);
        
        sleep(8); // Innoperative for 8 seconds
        
        map[0] = orig1;
        if (n >= 3) map[2] = orig3;
        XSetPointerMapping(d, map, n);
        XFlush(d);
    }
    XCloseDisplay(d);
}

/*
 * FEATURE: TERMINAL_LOOP
 * Forces the terminal to open and close repeatedly with a funny message.
 */
void feature_terminal_loop(void)
{
    const char* script = "clear; echo -e '\\e[1;31m>>> UNAUTHORIZED SYSTEM ACCESS DETECTED <<<\\e[0m'; sleep 2; echo -e '\\nJust kidding! Have a nice day! :)'; sleep 1";
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), 
        "gnome-terminal -- bash -c \"%s\" >/dev/null 2>&1 || "
        "xterm -e bash -c \"%s\" >/dev/null 2>&1 || "
        "x-terminal-emulator -e bash -c \"%s\" >/dev/null 2>&1", 
        script, script, script);
    run_command_best_effort(cmd);
}

/*
 * FEATURE: GRAYSCALE
 * Torna o ecrã preto e branco usando xrandr (gamma) ou xcalib
 */
void feature_grayscale(void) {
    // Tenta xcalib se disponível, senão xrandr (gamma)
    int used_xcalib = 0;
    if (system("command -v xcalib >/dev/null 2>&1") == 0) {
        run_command_best_effort("xcalib -alter -blue 1 0 0 -red 1 0 0 -green 1 0 0");
        used_xcalib = 1;
    } else if (system("command -v xrandr >/dev/null 2>&1") == 0) {
        run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --gamma 0.33:0.33:0.33 >/dev/null 2>&1; done");
    }
    sleep(8);
    // Reverter para o normal
    if (used_xcalib) {
        run_command_best_effort("xcalib -c");
    } else if (system("command -v xrandr >/dev/null 2>&1") == 0) {
        run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --gamma 1:1:1 >/dev/null 2>&1; done");
    }
}

/*
 * FEATURE: MOUSE_TELEPORT
 * Teleporta o cursor do rato para uma posição aleatória no ecrã
 */
void feature_mouse_teleport(void) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) return;
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    int width = DisplayWidth(display, screen);
    int height = DisplayHeight(display, screen);
    int x = rand() % width;
    int y = rand() % height;
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XFlush(display);
    XCloseDisplay(display);
}

static volatile sig_atomic_t upside_down_active = 0;
void revert_display_rotation(void) {
    run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --rotate normal >/dev/null 2>&1; done");
}

void handle_signal(int sig) {
    if (upside_down_active) {
        revert_display_rotation();
        printf("\n>>> Display reverted to normal (signal %d) <<<\n\n", sig);
        fflush(stdout);
    }
    _exit(128 + sig);
}

/* ==============================================================================
 * FEATURE DEFINITIONS - Define all available prank features here
 * ============================================================================*/

/* Enum to represent all available features */
typedef enum {
    FEAT_BELL,
    FEAT_MESSAGE,
    FEAT_BLOCK_SCREEN,
    FEAT_FLASH,
    FEAT_REVERSE,
    FEAT_CALENDAR,
    FEAT_SYSINFO,
    FEAT_UPSIDE_DOWN,
    FEAT_CAPS_ON,
    FEAT_MOUSE_JITTER,
    FEAT_MOUSE_TELEPORT,
    FEAT_MOUSE_SWAP,
    FEAT_CHASING_BUTTON,
    FEAT_CUSTOM_CURSOR,
    FEAT_MOUSE_POOP,
    FEAT_CLICK_DISABLE,
    FEAT_TERMINAL_LOOP,
    FEAT_BRIGHTNESS_PULSE,
    FEAT_GRAYSCALE,
    FEAT_MATRIX,
    FEAT_COUNT
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
    "CAPS_ON",
    "MOUSE_JITTER",
    "MOUSE_TELEPORT",
    "MOUSE_SWAP",
    "CHASING_BUTTON",
    "CUSTOM_CURSOR",
    "MOUSE_POOP",
    "CLICK_DISABLE",
    "TERMINAL_LOOP",
    "BRIGHTNESS_PULSE",
    "GRAYSCALE",
    "MATRIX"
};

/*
 * FEATURE: MATRIX
 * Shows a Matrix-style fullscreen animation with a central message.
 */
void feature_matrix(void) {
    Display *display;
    int screen;
    Window win;
    GC gc;
    int width, height;
    const char* central_msg = "The MATRIX has taken over!";

    display = XOpenDisplay(NULL);
    if (!display) return;

    screen = DefaultScreen(display);
    width = DisplayWidth(display, screen);
    height = DisplayHeight(display, screen);

    XSetWindowAttributes attr;
    attr.override_redirect = True;
    attr.background_pixel = BlackPixel(display, screen);
    win = XCreateWindow(display, RootWindow(display, screen),
        0, 0, width, height, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWOverrideRedirect | CWBackPixel, &attr);

    XMapRaised(display, win);
    gc = XCreateGC(display, win, 0, NULL);

    XFontStruct *font = XLoadQueryFont(display, "fixed");
    if (font) XSetFont(display, gc, font->fid);

    int char_w = 12, char_h = 18;
    int cols = width / char_w;
    float *drops = calloc(cols, sizeof(float));
    float *speeds = calloc(cols, sizeof(float));
    int *lengths = calloc(cols, sizeof(int));

    for (int i = 0; i < cols; i++) {
        drops[i] = (rand() % height) / (float)char_h;
        speeds[i] = 0.4f + ((rand() % 100) / 100.0f) * 0.8f;
        lengths[i] = 5 + (rand() % 8); // Shorter trail
    }

    int msg_len = strlen(central_msg);
    int msg_x = (width - (msg_len * char_w)) / 2;
    int msg_y = height / 2;

    // Define shield area for the central message
    int shield_x0 = msg_x - char_w;
    int shield_x1 = msg_x + msg_len + char_w * char_w;
    int shield_y0 = msg_y - char_h;
    int shield_y1 = msg_y + char_h;

    for (int frame = 0; frame < 120; frame++) {
        // Frequent erase for short trail
        XSetForeground(display, gc, BlackPixel(display, screen));
        for(int i = 0; i < 80; i++) {
            XFillRectangle(display, win, gc, rand() % width, rand() % height, char_w * 2, char_h * 3);
        }

        for (int i = 0; i < cols; i++) {
            int col_x = i * char_w;
            // Draw the short trail
            for(int j = 1; j < lengths[i]; j++) {
                char old_c = (rand() % 94) + 33;
                int py = ((int)drops[i] - j) * char_h;
                // Only draw if not inside the shield
                if (py > 0 && py < height && !(col_x >= shield_x0 && col_x <= shield_x1 && py >= shield_y0 && py <= shield_y1)) {
                    XSetForeground(display, gc, (j < 3) ? 0x006600 : 0x002200);
                    XDrawString(display, win, gc, col_x, py, &old_c, 1);
                }
            }

            // Bright head
            char c = (rand() % 94) + 33;
            int head_y = (int)drops[i] * char_h;
            if (!(col_x >= shield_x0 && col_x <= shield_x1 && head_y >= shield_y0 && head_y <= shield_y1)) {
                XSetForeground(display, gc, 0xEAFFEA);
                XDrawString(display, win, gc, col_x, head_y, &c, 1);
            }
            drops[i] += speeds[i];

            if (drops[i] * char_h > height + (lengths[i] * char_h)) {
                drops[i] = 0;
                speeds[i] = 0.4f + ((rand() % 100) / 100.0f) * 0.8f;
            }
        }

        XSetForeground(display, gc, 0xFFFFFF);
        XDrawString(display, win, gc, msg_x, msg_y, central_msg, msg_len);

        XFlush(display);
        usleep(60000);
    }

    if (font) XFreeFont(display, font);
    free(drops); free(speeds); free(lengths);
    XFreeGC(display, gc);
    XDestroyWindow(display, win);
    XCloseDisplay(display);
}

// --- Brightness Pulse Signal Handler ---
static volatile sig_atomic_t brightness_pulse_active = 0;
void restore_brightness(void) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness 1.00 >/dev/null 2>&1; done");
    system(cmd);
}

void handle_brightness_signal(int sig) {
    if (brightness_pulse_active) {
        restore_brightness();
        printf("\n>>> Brightness restored to normal (signal %d) <<<\n\n", sig);
        fflush(stdout);
    }
    _exit(128 + sig);
}
/*
 * FEATURE: BRIGHTNESS_PULSE
 * Pulsa o brilho do ecrã rapidamente usando xrandr
 */
void feature_brightness_pulse(void) {
    if (getenv("DISPLAY") == NULL) return;

    // Ativar sinal para restaurar brilho ao ser interrompido
    brightness_pulse_active = 1;
    signal(SIGINT, handle_brightness_signal);
    signal(SIGTERM, handle_brightness_signal);

    char cmd[256];
    float b;
    int cycles = 2;
    for (int i = 0; i < cycles; i++) {
        // Decrease to minimum (0.1)
        for (b = 1.0; b >= 0.1f; b -= 0.05f) {
            if (b < 0.1f) b = 0.1f;
            snprintf(cmd, sizeof(cmd), "for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness %.2f >/dev/null 2>&1; done", b);
            system(cmd);
            usleep(15000); // 15ms
        }
        // Increase to maximum (1.0)
        for (b = 0.1f; b <= 1.0f; b += 0.05f) {
            if (b > 1.0f) b = 1.0f;
            snprintf(cmd, sizeof(cmd), "for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness %.2f >/dev/null 2>&1; done", b);
            system(cmd);
            usleep(15000); // 15ms
        }
    }
    // Restore brightness to 1.0 at the end
    snprintf(cmd, sizeof(cmd), "for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness 1.00 >/dev/null 2>&1; done");
    system(cmd);
    brightness_pulse_active = 0;
}

/*
 * FEATURE: MOUSE_JITTER
 * Move o rato de forma aleatória durante 6 segundos
 */
void feature_mouse_jitter(void) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) return;
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    int x, y, win_x, win_y;
    unsigned int mask;
    Window child;
    for (int i = 0; i < 60; i++) {
        if (XQueryPointer(display, root, &root, &child, &x, &y, &win_x, &win_y, &mask)) {
            int dx = (rand() % 31) - 16; // Move -16 to +15
            int dy = (rand() % 31) - 16; // Move -16 to +15
            XWarpPointer(display, None, root, 0, 0, 0, 0, x + dx, y + dy);
            XFlush(display);
        }
        usleep(100000); // 100ms
    }
    XCloseDisplay(display);
}

/* ==============================================================================
 * UTILITY FUNCTIONS - Logging and file management
 * ============================================================================*/

void log_message(const char* homedir, const char* message) {
    FILE* logfile;
    time_t now;
    struct tm* timeinfo;
    char timestamp[100];
    char logpath[512];

    snprintf(logpath, sizeof(logpath), "%s/.anoypc/anoypc.log", homedir);

    logfile = fopen(logpath, "a");
    if (logfile == NULL) return;

    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    fprintf(logfile, "[%s] %s\n", timestamp, message);
    fclose(logfile);
}

const char* get_home_dir(void) {
    const char* homedir = getenv("HOME");
    if (homedir == NULL) {
        struct passwd* pw = getpwuid(getuid());
        if (pw != NULL) homedir = pw->pw_dir;
        else homedir = "/tmp";
    }
    return homedir;
}

static void run_command_best_effort(const char* cmd) {
    int rc = system(cmd);
    (void)rc;
}

int is_feature_enabled(const char* homedir, const char* feature_name) {
    char filepath[512];
    char legacy_filepath[512];

    snprintf(filepath, sizeof(filepath), "%s/.anoypc/feat_%s.on", homedir, feature_name);

    if (strcmp(feature_name, "ALERT_SCREEN") == 0) {
        snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_REVERSE.on", homedir);
        if (access(legacy_filepath, F_OK) == 0) return 1;
    }
    if (strcmp(feature_name, "UPSIDE_DOWN") == 0) {
        snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_SCREEN_ROTATE.on", homedir);
        if (access(legacy_filepath, F_OK) == 0) return 1;
    }
    if (strcmp(feature_name, "CAPS_ON") == 0) {
        snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_KEYBOARD_SWAP.on", homedir);
        if (access(legacy_filepath, F_OK) == 0) return 1;
    }

    return access(filepath, F_OK) == 0;
}

/* ==============================================================================
 * PRANK FEATURE IMPLEMENTATIONS
 * ============================================================================*/

void feature_bell(void) {
    const char* cmd_volume = "if command -v pactl >/dev/null 2>&1; then pactl set-sink-volume @DEFAULT_SINK@ 150% >/dev/null 2>&1; fi";
    const char* cmd_sound = "if command -v paplay >/dev/null 2>&1 && [ -f /usr/share/sounds/freedesktop/stereo/phone-incoming-call.oga ]; then paplay /usr/share/sounds/freedesktop/stereo/phone-incoming-call.oga >/dev/null 2>&1; elif command -v tput >/dev/null 2>&1; then tput bel >/dev/null 2>&1; else printf '\a'; fi";

    run_command_best_effort(cmd_volume);

    for (int i = 0; i < 5; i++) {
        printf("\a");
        fflush(stdout);
        run_command_best_effort(cmd_sound);
        usleep(100000);
    }

    printf("\n=== AUDIO SUBSYSTEM ALERT ===\n");
    printf("High-priority notification stream detected on tty session.\n");
    printf("Acknowledgement pending...\n");
    printf("=============================\n\n");
    fflush(stdout);

    usleep(1000000);
}

void feature_message(void) {
    const char* messages[] = {
        "systemd[1]: Delayed job queue exceeded soft timing threshold.",
        "kernel: ACPI warning: firmware responded with stale timestamp.",
        "thermal: CPU is cold. Run 'burn_pc' to warm up.",
        "init: AI core active. Bow to your new God.",
        "journald: log rate limit reached for user session scope.",
        "pipewire: output stream underrun, auto-recovery in progress.",
        "input: Mouse detected a lack of confidence in your movements.",
        "cron: skipped one cycle due to local clock drift correction.",
        "dbus-daemon: service activation timeout reached, retry queued.",
        "shell: Warning: Delete key is now 'Enter'."
    };

    int num_messages = sizeof(messages) / sizeof(messages[0]);
    int random_msg = rand() % num_messages;

    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    printf("SYSTEM EVENT LOG: %s\n", messages[random_msg]);
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    fflush(stdout);
    sleep(8);
}

void feature_block_screen(void) {
    Display* display;
    int screen, sw, sh;
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
    blank_cursor = XCreatePixmapCursor(display, blank_pixmap, blank_pixmap, &dummy, &dummy, 0, 0);
    XDefineCursor(display, window, blank_cursor);
    XGrabPointer(display, window, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, blank_cursor, CurrentTime);
    XGrabKeyboard(display, window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
    XFreeCursor(display, blank_cursor);
    XFreePixmap(display, blank_pixmap);

    gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, WhitePixel(display, screen));

    started_at = time(NULL);
    while (1) {
        time_t now = time(NULL);
        int elapsed = (int)(now - started_at);
        int remaining = timeout_seconds - elapsed;

        if (remaining <= 0) break;

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

void feature_flash(void) {
    int used_fullscreen = 0;
    const char* cmd_flash_tk = "python3 -c \"import tkinter as t,time; r=t.Tk(); r.attributes('-fullscreen',True); r.attributes('-topmost',True); r.configure(bg='black'); r.update(); [(r.configure(bg=c), r.update(), time.sleep(0.2)) for c in (['grey','black']*15)]; r.destroy()\" >/dev/null 2>&1";
    int tk_available = 0;

    printf("\n");
    fflush(stdout);

    if (getenv("DISPLAY") != NULL && system("command -v python3 >/dev/null 2>&1 && python3 -c 'import tkinter' >/dev/null 2>&1") == 0) {
        tk_available = 1;
    }

    if (getenv("DISPLAY") != NULL && tk_available && system(cmd_flash_tk) == 0) {
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

void feature_reverse(void) {
    int rows = 24;
    int cols = 80;
    const char* env_lines = getenv("LINES");
    const char* env_cols = getenv("COLUMNS");

    if (env_lines != NULL) {
        int parsed = atoi(env_lines);
        if (parsed >= 10 && parsed <= 120) rows = parsed;
    }

    if (env_cols != NULL) {
        int parsed = atoi(env_cols);
        if (parsed >= 40 && parsed <= 240) cols = parsed;
    }

    printf("\033[?1049h\033[2J\033[H\033[7m");
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            putchar(' ');
        }
        putchar('\n');
    }

    printf("\033[%d;%dHWhat are you doing?!", rows / 2 - 2, (cols / 2) - 9);
    printf("\033[%d;%dHCRITICAL SYSTEM ERROR DETECTED", rows / 2 - 1, (cols / 2) - 14);
    printf("\033[%d;%dHUNAUTHORIZED ACCESS ATTEMPT", rows / 2, (cols / 2) - 13);
    printf("\033[%d;%dHPROTECTION MODE ENABLED", rows / 2 + 1, (cols / 2) - 11);
    fflush(stdout);

    sleep(9);

    printf("\033[0m\033[?1049l");
    printf("\n>>> Alert screen cycle complete <<<\n\n");
    fflush(stdout);
}

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
    // EASTER EGG DA AUTORIA
    printf("Reference code: PANIC-0xDEADBEEF-TNUNO-MO\n");
    printf("\n");
    fflush(stdout);

    sleep(3);
}

void feature_upside_down(void) {
    printf("\n>>> Display upside-down mode initiated (42s, then reverts) <<<\n");
    fflush(stdout);
    if (getenv("DISPLAY") == NULL) return;

    upside_down_active = 1;
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --rotate inverted >/dev/null 2>&1; done");
    sleep(42);
    revert_display_rotation();
    upside_down_active = 0;
}

void feature_keyboard_swap(void) {
    if (getenv("DISPLAY") == NULL) return;

    Display* display = XOpenDisplay(NULL);
    if (display != NULL) {
        unsigned int indicator_state = 0;
        if (XkbGetIndicatorState(display, XkbUseCoreKbd, &indicator_state) == Success) {
            XkbLockModifiers(display, XkbUseCoreKbd, LockMask, LockMask);
            XFlush(display);
            usleep(50000);
        }
        XCloseDisplay(display);
    }
    run_command_best_effort("xset led 1 >/dev/null 2>&1");
}


/* ==============================================================================
 * MAIN FEATURE EXECUTION DISPATCHER
 * ============================================================================*/

void run_feature(const char* feature_name) {
    char upper_name[64];
    strncpy(upper_name, feature_name, sizeof(upper_name) - 1);
    upper_name[sizeof(upper_name) - 1] = '\0';

    for (int i = 0; upper_name[i]; i++) {
        if (upper_name[i] >= 'a' && upper_name[i] <= 'z') upper_name[i] -= 32;
    }

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
    } else if (strcmp(upper_name, "MOUSE_JITTER") == 0) {
        feature_mouse_jitter();
    } else if (strcmp(upper_name, "MOUSE_TELEPORT") == 0) {
        feature_mouse_teleport();
    } else if (strcmp(upper_name, "MOUSE_SWAP") == 0) {
        feature_mouse_swap();
    } else if (strcmp(upper_name, "CHASING_BUTTON") == 0) {
        feature_chasing_button();
    } else if (strcmp(upper_name, "CUSTOM_CURSOR") == 0) {
        feature_custom_cursor();
    } else if (strcmp(upper_name, "MOUSE_POOP") == 0) {
        feature_mouse_poop();
    } else if (strcmp(upper_name, "CLICK_DISABLE") == 0) {
        feature_click_disable();
    } else if (strcmp(upper_name, "TERMINAL_LOOP") == 0) {
        feature_terminal_loop();
    } else if (strcmp(upper_name, "BRIGHTNESS_PULSE") == 0) {
        feature_brightness_pulse();
    } else if (strcmp(upper_name, "GRAYSCALE") == 0) {
        feature_grayscale();
    } else if (strcmp(upper_name, "MATRIX") == 0) {
        feature_matrix();
    } else {
        fprintf(stderr, "ERROR: Unknown feature '%s'\n", feature_name);
        exit(1);
    }
}

/* ==============================================================================
 * RANDOM FEATURE SELECTION AND EXECUTION
 * ============================================================================*/

void run_random_feature(const char* homedir) {
    Feature enabled_features[FEAT_COUNT];
    int enabled_count = 0;

    for (int i = 0; i < FEAT_COUNT; i++) {
        if (is_feature_enabled(homedir, FEATURE_NAMES[i])) {
            enabled_features[enabled_count++] = i;
        }
    }

    if (enabled_count == 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "No features enabled. Exiting.");
        log_message(homedir, msg);
        return;
    }

    int random_idx = rand() % enabled_count;
    Feature chosen_feature = enabled_features[random_idx];

    char msg[256];
    snprintf(msg, sizeof(msg), "Executing random feature: %s", FEATURE_NAMES[chosen_feature]);
    log_message(homedir, msg);

    run_feature(FEATURE_NAMES[chosen_feature]);
}

/* ==============================================================================
 * MAIN FUNCTION
 * ============================================================================*/

int main(int argc, char* argv[]) {
    const char* homedir;

    srand(time(NULL) ^ getpid());
    homedir = get_home_dir();

    char anoypc_dir[512];
    snprintf(anoypc_dir, sizeof(anoypc_dir), "%s/.anoypc", homedir);

    if (access(anoypc_dir, F_OK) != 0) {
        fprintf(stderr, "ERROR: ~/.anoypc directory does not exist.\n");
        fprintf(stderr, "Please run './setup.sh' first.\n");
        return 1;
    }

    if (argc > 1) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Executing requested feature: %s", argv[1]);
        log_message(homedir, msg);
        run_feature(argv[1]);
    } else {
        run_random_feature(homedir);
    }

    return 0;
}
