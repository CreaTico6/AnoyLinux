//
//	AnoyPC
//
//	by: creatico6@gmail.com
//
//					march 2026
//


/*
 * AnoyingPC Linux - Main Prank Application in C
 * 
 * A minimalist prank application that executes harmless system
 * disruptions on Linux. Features are toggled via marker files
 * and executed via a cron scheduler (every 30 minutes).
 * 
 * Compilation: gcc -o AnoyPC AnoyPC.c
 * Usage: ./AnoyPC [FEATURE_NAME]
 *   - If FEATURE_NAME is omitted, a random enabled feature is selected
 *   - If FEATURE_NAME is provided, that specific feature is executed
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

/* Forward declaration for signal handler use */
static void run_command_best_effort(const char* cmd);

#include <signal.h>

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
	FEAT_BELL,				/* 1. Terminal Bell */
	FEAT_MESSAGE,			/* 2. Random Message */
	FEAT_BLOCK_SCREEN,		/* 3. Block Screen */
	FEAT_FLASH,				/* 4. Screen Flash */
	FEAT_REVERSE,			/* 5. Alert Screen */
	FEAT_CALENDAR,			/* 6. Calendar Joke */
	FEAT_SYSINFO,			/* 7. System Info Spoof */
	FEAT_UPSIDE_DOWN,		/* 8. Upside Down */
	FEAT_CAPS_ON,			/* 9. CAPS ON */
	FEAT_MOUSE_JITTER,		/* 10. Mouse Jitter */
	FEAT_BRIGHTNESS_PULSE,	/* 11. Brightness Pulse */
	FEAT_MATRIX,			/* 12. Matrix */
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
	"BRIGHTNESS_PULSE",
	"MATRIX"
};
/*
 * FEATURE 12: MATRIX
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
 * FEATURE 11: BRIGHTNESS_PULSE
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
 * FEATURE 10: MOUSE_JITTER
 * Move o rato de forma aleatória durante 2 segundos
 */
void feature_mouse_jitter(void) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) return;
	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);
	int x, y, win_x, win_y;
	unsigned int mask;
	Window child;
	for (int i = 0; i < 20; i++) {
		if (XQueryPointer(display, root, &root, &child, &x, &y, &win_x, &win_y, &mask)) {
			int dx = (rand() % 11) - 5; // Move -5 to +5
			int dy = (rand() % 11) - 5;
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
int is_feature_enabled(const char* homedir, const char* feature_name)
{
	char filepath[512];
	char legacy_filepath[512];

	/* Construct path: ~/.anoypc/feat_FEATURENAME.on */
	snprintf(filepath, sizeof(filepath), "%s/.anoypc/feat_%s.on", homedir, feature_name);

	if (strcmp(feature_name, "ALERT_SCREEN") == 0)
	{
		snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_REVERSE.on", homedir);
		if (access(legacy_filepath, F_OK) == 0)
		{
			return 1;
		}
	}

	if (strcmp(feature_name, "UPSIDE_DOWN") == 0)
	{
		snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_SCREEN_ROTATE.on", homedir);
		if (access(legacy_filepath, F_OK) == 0) {
			return 1;
		}
	}

	if (strcmp(feature_name, "CAPS_ON") == 0)
	{
		snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/.anoypc/feat_KEYBOARD_SWAP.on", homedir);
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

	const char* cmd_sound = "if command -v paplay >/dev/null 2>&1 && [ -f /usr/share/sounds/freedesktop/stereo/phone-incoming-call.oga ]; then paplay /usr/share/sounds/freedesktop/stereo/phone-incoming-call.oga >/dev/null 2>&1; elif command -v tput >/dev/null 2>&1; then tput bel >/dev/null 2>&1; else printf '\a'; fi";

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
	sleep(8);  /* Display message briefly without blocking too long */
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
	int tk_available = 0;

	printf("\n");
	fflush(stdout);

	if (getenv("DISPLAY") != NULL && system("command -v python3 >/dev/null 2>&1 && python3 -c 'import tkinter' >/dev/null 2>&1") == 0) {
		tk_available = 1;
	}

	/* Full-screen desktop flash attempt (no sudo required) */
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
	if (getenv("DISPLAY") == NULL) return;

	upside_down_active = 1;
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	// Rotate all connected displays to inverted
	run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --rotate inverted >/dev/null 2>&1; done");

	sleep(42);

	// Restore all displays to normal
	revert_display_rotation();
	upside_down_active = 0;
}
void feature_keyboard_swap(void) {
	if (getenv("DISPLAY") == NULL) {
		return;
	}

	Display* display = XOpenDisplay(NULL);
	if (display != NULL) {
		unsigned int indicator_state = 0;

		/* Force CAPS ON (do not toggle) */
		if (XkbGetIndicatorState(display, XkbUseCoreKbd, &indicator_state) == Success) {
			XkbLockModifiers(display, XkbUseCoreKbd, LockMask, LockMask);
			XFlush(display);
			usleep(50000);
		}

		XCloseDisplay(display);
	}

	/* Fallback: try xset indicator directly */
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
	} else if (strcmp(upper_name, "MOUSE_JITTER") == 0) {
		feature_mouse_jitter();
	} else if (strcmp(upper_name, "BRIGHTNESS_PULSE") == 0) {
		feature_brightness_pulse();
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
	snprintf(msg, sizeof(msg), "Executing random feature: %s", FEATURE_NAMES[chosen_feature]);
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
