//
//	AnoyPC
//
//	by: tnuno-mo (creatico6@gmail.com)
//
//					March 2026
//

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
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

#ifndef ANOY_DIR
#define ANOY_DIR ".anoypc"
#endif

/* ==============================================================================
 * GLOBALS & SIGNAL HANDLING
 * ============================================================================*/

/* Flags to track if hardware-altering features are currently active */
static volatile sig_atomic_t upside_down_active = 0;
static volatile sig_atomic_t brightness_pulse_active = 0;
static volatile sig_atomic_t grayscale_active = 0;

/* Forward declaration for the utility command runner used in signal handlers */
static void run_command_best_effort(const char* cmd);

/* Restores display state (rotation, brightness, and gamma) upon interruption */
static void restore_display_state(void) {
	if (upside_down_active) {
		run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --rotate normal >/dev/null 2>&1; done");
	}
	if (brightness_pulse_active) {
		run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness 1.00 >/dev/null 2>&1; done");
	}
	if (grayscale_active) {
		/* Safely attempt to revert both possible grayscale methods */
		run_command_best_effort("xcalib -c >/dev/null 2>&1");
		run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --gamma 1:1:1 >/dev/null 2>&1; done");
	}
}

/* Unified signal handler to ensure safe exits */
void handle_interrupt(int sig) {
	restore_display_state();
	printf("\n>>> Display state restored safely (signal %d) <<<\n\n", sig);
	fflush(stdout);
	_exit(128 + sig);
}


/* ==============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================*/

/* Executes a shell command and ignores its return value */
static void run_command_best_effort(const char* cmd) {
	int rc = system(cmd);
	(void)rc;
}

/* Appends a message to the internal log file */
void log_message(const char* homedir, const char* message) {
	FILE* logfile;
	time_t now;
	struct tm* timeinfo;
	char timestamp[100];
	char logpath[512];

	snprintf(logpath, sizeof(logpath), "%s/%s/anoypc.log", homedir, ANOY_DIR);

	logfile = fopen(logpath, "a");
	if (logfile == NULL) return;

	time(&now);
	timeinfo = localtime(&now);
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

	fprintf(logfile, "[%s] %s\n", timestamp, message);
	fclose(logfile);
}

/* Retrieves the current user's home directory safely */
const char* get_home_dir(void) {
	const char* homedir = getenv("HOME");
	if (homedir == NULL) {
		struct passwd* pw = getpwuid(getuid());
		homedir = (pw != NULL) ? pw->pw_dir : "/tmp";
	}
	return homedir;
}

/* Checks if a specific feature marker file exists */
int is_feature_enabled(const char* homedir, const char* feature_name) {
	char filepath[512];
	char legacy_filepath[512];

	snprintf(filepath, sizeof(filepath), "%s/%s/feat_%s.on", homedir, ANOY_DIR, feature_name);

	/* Legacy fallbacks for older marker file names */
	if (strcmp(feature_name, "ALERT_SCREEN") == 0) {
		snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/%s/feat_REVERSE.on", homedir, ANOY_DIR);
		if (access(legacy_filepath, F_OK) == 0) return 1;
	}
	if (strcmp(feature_name, "UPSIDE_DOWN") == 0) {
		snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/%s/feat_SCREEN_ROTATE.on", homedir, ANOY_DIR);
		if (access(legacy_filepath, F_OK) == 0) return 1;
	}
	if (strcmp(feature_name, "CAPS_ON") == 0) {
		snprintf(legacy_filepath, sizeof(legacy_filepath), "%s/%s/feat_KEYBOARD_SWAP.on", homedir, ANOY_DIR);
		if (access(legacy_filepath, F_OK) == 0) return 1;
	}

	return access(filepath, F_OK) == 0;
}


/* ==============================================================================
 * PRANK FEATURES (MOUSE & KEYBOARD)
 * ============================================================================*/

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
		
		/* Revert to original mapping */
		temp = map[0];
		map[0] = map[2];
		map[2] = temp;
		
		XSetPointerMapping(d, map, n);
		XFlush(d);
	}
	XCloseDisplay(d);
}

void feature_click_disable(void) {
	Display *d = XOpenDisplay(NULL);
	if (!d) return;

	unsigned char map[256];
	int n = XGetPointerMapping(d, map, 256);
	
	if (n >= 1) {
		unsigned char orig1 = map[0];
		unsigned char orig3 = map[2];
		
		map[0] = 0; /* Deactivate left click */
		if (n >= 3) map[2] = 0; /* Deactivate right click */
		
		XSetPointerMapping(d, map, n);
		XFlush(d);
		
		sleep(8); /* Keep inoperative for 8 seconds */
		
		/* Revert to original mapping */
		map[0] = orig1;
		if (n >= 3) map[2] = orig3;
		XSetPointerMapping(d, map, n);
		XFlush(d);
	}
	XCloseDisplay(d);
}

void feature_mouse_teleport(void) {
	Display *d = XOpenDisplay(NULL);
	if (!d) return;

	int screen = DefaultScreen(d);
	Window root = RootWindow(d, screen);
	int width = DisplayWidth(d, screen);
	int height = DisplayHeight(d, screen);
	
	int x = rand() % width;
	int y = rand() % height;
	
	XWarpPointer(d, None, root, 0, 0, 0, 0, x, y);
	XFlush(d);
	XCloseDisplay(d);
}

void feature_mouse_jitter(void) {
	Display *d = XOpenDisplay(NULL);
	if (!d) return;

	int screen = DefaultScreen(d);
	Window root = RootWindow(d, screen);
	int x, y, win_x, win_y;
	unsigned int mask;
	Window child;

	for (int i = 0; i < 60; i++) {
		if (XQueryPointer(d, root, &root, &child, &x, &y, &win_x, &win_y, &mask)) {
			int dx = (rand() % 31) - 16;
			int dy = (rand() % 31) - 16;
			XWarpPointer(d, None, root, 0, 0, 0, 0, x + dx, y + dy);
			XFlush(d);
		}
		usleep(100000); /* 100ms delay */
	}
	XCloseDisplay(d);
}

void feature_caps_on(void) {
	if (getenv("DISPLAY") == NULL) return;

	Display* d = XOpenDisplay(NULL);
	if (d != NULL) {
		unsigned int indicator_state = 0;
		if (XkbGetIndicatorState(d, XkbUseCoreKbd, &indicator_state) == Success) {
			XkbLockModifiers(d, XkbUseCoreKbd, LockMask, LockMask);
			XFlush(d);
			usleep(50000);
		}
		XCloseDisplay(d);
	}
	/* Fallback indicator toggle */
	run_command_best_effort("xset led 1 >/dev/null 2>&1");
}


/* ==============================================================================
 * PRANK FEATURES (VISUALS & X11 OVERLAYS)
 * ============================================================================*/

void feature_custom_cursor(void) {
	Display *d = XOpenDisplay(NULL);
	if (!d) return;

	int s = DefaultScreen(d);
	
	/* Native X11 cursors array */
	unsigned int cursors[] = {
		XC_gumby,       
		XC_spider,      
		XC_gobbler,     
		XC_watch,       
		XC_coffee_mug,  
		XC_fleur,       
		XC_pirate       
	};
	
	int num_cursors = sizeof(cursors) / sizeof(cursors[0]);
	Cursor cursor = XCreateFontCursor(d, cursors[rand() % num_cursors]); 
	
	/* Force cursor grab */
	XGrabPointer(d, RootWindow(d, s), False, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime);
	XFlush(d);
	
	sleep(10);
	
	XUngrabPointer(d, CurrentTime);
	XFreeCursor(d, cursor);
	XCloseDisplay(d);
}

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
		usleep(500000); /* Drop a square every 500ms */
	}
	
	sleep(3); /* Keep trail briefly before cleanup */
	for(int i = 0; i < poop_count; i++) {
		XDestroyWindow(d, poops[i]);
	}
	XCloseDisplay(d);
}

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
	while(time(NULL) - start < 10) { /* 10-second security timeout */
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
			/* 40px safety margin around the button */
			if (root_x > x - 40 && root_x < x + w + 40 && root_y > y - 40 && root_y < y + h + 40) {
				x = rand() % (sw - w);
				y = rand() % (sh - h);
				XMoveWindow(d, win, x, y);
			}
		}
		usleep(30000); /* Smooth frame delay */
	}

end_chase:
	XFreeGC(d, gc);
	XDestroyWindow(d, win);
	XCloseDisplay(d);
}

void feature_matrix(void) {
	Display *display = XOpenDisplay(NULL);
	if (!display) return;

	int screen = DefaultScreen(display);
	int width = DisplayWidth(display, screen);
	int height = DisplayHeight(display, screen);
	const char* central_msg = "The MATRIX has taken over!";

	XSetWindowAttributes attr;
	attr.override_redirect = True;
	attr.background_pixel = BlackPixel(display, screen);
	Window win = XCreateWindow(display, RootWindow(display, screen),
		0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent,
		CWOverrideRedirect | CWBackPixel, &attr);

	XMapRaised(display, win);
	GC gc = XCreateGC(display, win, 0, NULL);

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
		lengths[i] = 5 + (rand() % 8); 
	}

	int msg_len = strlen(central_msg);
	int msg_x = (width - (msg_len * char_w)) / 2;
	int msg_y = height / 2;

	int shield_x0 = msg_x - char_w;
	int shield_x1 = msg_x + msg_len + char_w * char_w;
	int shield_y0 = msg_y - char_h;
	int shield_y1 = msg_y + char_h;

	for (int frame = 0; frame < 120; frame++) {
		XSetForeground(display, gc, BlackPixel(display, screen));
		for(int i = 0; i < 80; i++) {
			XFillRectangle(display, win, gc, rand() % width, rand() % height, char_w * 2, char_h * 3);
		}

		for (int i = 0; i < cols; i++) {
			int col_x = i * char_w;
			for(int j = 1; j < lengths[i]; j++) {
				char old_c = (rand() % 94) + 33;
				int py = ((int)drops[i] - j) * char_h;
				if (py > 0 && py < height && !(col_x >= shield_x0 && col_x <= shield_x1 && py >= shield_y0 && py <= shield_y1)) {
					XSetForeground(display, gc, (j < 3) ? 0x006600 : 0x002200);
					XDrawString(display, win, gc, col_x, py, &old_c, 1);
				}
			}

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

void feature_block_screen(void) {
	Display* display = XOpenDisplay(NULL);
	if (!display) return;

	int screen = DefaultScreen(display);
	int sw = DisplayWidth(display, screen);
	int sh = DisplayHeight(display, screen);

	XSetWindowAttributes attr;
	attr.override_redirect = True;
	attr.background_pixel = BlackPixel(display, screen);

	Window window = XCreateWindow(display, RootWindow(display, screen),
							0, 0, (unsigned int)sw, (unsigned int)sh,
							0, CopyFromParent, InputOutput, CopyFromParent,
							CWOverrideRedirect | CWBackPixel, &attr);

	XMapRaised(display, window);
	XSelectInput(display, window, KeyPressMask | ExposureMask);
	XSetInputFocus(display, window, RevertToParent, CurrentTime);

	Pixmap blank_pixmap = XCreatePixmap(display, window, 1, 1, 1);
	XColor dummy;
	Cursor blank_cursor = XCreatePixmapCursor(display, blank_pixmap, blank_pixmap, &dummy, &dummy, 0, 0);
	
	XDefineCursor(display, window, blank_cursor);
	XGrabPointer(display, window, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, blank_cursor, CurrentTime);
	XGrabKeyboard(display, window, True, GrabModeAsync, GrabModeAsync, CurrentTime);

	GC gc = XCreateGC(display, window, 0, NULL);
	time_t started_at = time(NULL);
	const int timeout_seconds = 42;
	struct timespec tick = {0, 100000000};
	
	const char* line1 = "The answer for everything, is also the way to exit this screen. (only one char).";
	char line2[160];

	while (1) {
		int elapsed = (int)(time(NULL) - started_at);
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
					goto exit_block;
				}
			}
		}
		nanosleep(&tick, NULL);
	}

exit_block:
	XFreeCursor(display, blank_cursor);
	XFreePixmap(display, blank_pixmap);
	XFreeGC(display, gc);
	XUngrabKeyboard(display, CurrentTime);
	XUngrabPointer(display, CurrentTime);
	XUndefineCursor(display, window);
	XDestroyWindow(display, window);
	XFlush(display);
	XCloseDisplay(display);
}


/* ==============================================================================
 * PRANK FEATURES (DISPLAY SETTINGS & TERMINAL)
 * ============================================================================*/

void feature_grayscale(void) {
	if (getenv("DISPLAY") == NULL) return;

	grayscale_active = 1;
	signal(SIGINT, handle_interrupt);
	signal(SIGTERM, handle_interrupt);

	int used_xcalib = 0;
	
	/* Try xcalib if available, fallback to xrandr gamma */
	if (system("command -v xcalib >/dev/null 2>&1") == 0) {
		run_command_best_effort("xcalib -alter -blue 1 0 0 -red 1 0 0 -green 1 0 0");
		used_xcalib = 1;
	} else if (system("command -v xrandr >/dev/null 2>&1") == 0) {
		run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --gamma 0.33:0.33:0.33 >/dev/null 2>&1; done");
	}
	
	sleep(8);
	
	/* Revert to normal colors */
	if (used_xcalib) {
		run_command_best_effort("xcalib -c >/dev/null 2>&1");
	} else if (system("command -v xrandr >/dev/null 2>&1") == 0) {
		run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --gamma 1:1:1 >/dev/null 2>&1; done");
	}

	grayscale_active = 0;
}

void feature_upside_down(void) {
	if (getenv("DISPLAY") == NULL) return;

	upside_down_active = 1;
	signal(SIGINT, handle_interrupt);
	signal(SIGTERM, handle_interrupt);

	run_command_best_effort("for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --rotate inverted >/dev/null 2>&1; done");
	sleep(42);
	
	restore_display_state();
	upside_down_active = 0;
}

void feature_brightness_pulse(void) {
	if (getenv("DISPLAY") == NULL) return;

	brightness_pulse_active = 1;
	signal(SIGINT, handle_interrupt);
	signal(SIGTERM, handle_interrupt);

	char cmd[256];
	float b;
	for (int i = 0; i < 2; i++) {
		for (b = 1.0; b >= 0.1f; b -= 0.05f) {
			snprintf(cmd, sizeof(cmd), "for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness %.2f >/dev/null 2>&1; done", (b < 0.1f) ? 0.1f : b);
			run_command_best_effort(cmd);
			usleep(15000); 
		}
		for (b = 0.1f; b <= 1.0f; b += 0.05f) {
			snprintf(cmd, sizeof(cmd), "for out in $(xrandr --query | awk '/ connected/{print $1}'); do xrandr --output \"$out\" --brightness %.2f >/dev/null 2>&1; done", (b > 1.0f) ? 1.0f : b);
			run_command_best_effort(cmd);
			usleep(15000); 
		}
	}
	
	restore_display_state();
	brightness_pulse_active = 0;
}

void feature_terminal_loop(void) {
	/* Escape sequences mapped for safe shell execution */
	const char* script = "clear; echo -e '\\e[1;31m>>> UNAUTHORIZED SYSTEM ACCESS DETECTED <<<\\e[0m'; sleep 2; echo -e '\\nJust kidding! Have a nice day! :)'; sleep 1";
	char cmd[1024];
	
	snprintf(cmd, sizeof(cmd), 
		"gnome-terminal -- bash -c \"%s\" >/dev/null 2>&1 || "
		"xterm -e bash -c \"%s\" >/dev/null 2>&1 || "
		"x-terminal-emulator -e bash -c \"%s\" >/dev/null 2>&1", 
		script, script, script);
		
	run_command_best_effort(cmd);
}

void feature_flash(void) {
	int used_fullscreen = 0;
	const char* cmd_flash_tk = "python3 -c \"import tkinter as t,time; r=t.Tk(); r.attributes('-fullscreen',True); r.attributes('-topmost',True); r.configure(bg='black'); r.update(); [(r.configure(bg=c), r.update(), time.sleep(0.2)) for c in (['grey','black']*15)]; r.destroy()\" >/dev/null 2>&1";

	if (getenv("DISPLAY") != NULL && system("command -v python3 >/dev/null 2>&1 && python3 -c 'import tkinter' >/dev/null 2>&1") == 0) {
		if (system(cmd_flash_tk) == 0) used_fullscreen = 1;
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
}

void feature_alert_screen(void) {
	int rows = 24, cols = 80;
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
		for (int col = 0; col < cols; col++) putchar(' ');
		putchar('\n');
	}

	printf("\033[%d;%dHWhat are you doing?!", rows / 2 - 2, (cols / 2) - 9);
	printf("\033[%d;%dHCRITICAL SYSTEM ERROR DETECTED", rows / 2 - 1, (cols / 2) - 14);
	printf("\033[%d;%dHUNAUTHORIZED ACCESS ATTEMPT", rows / 2, (cols / 2) - 13);
	printf("\033[%d;%dHPROTECTION MODE ENABLED", rows / 2 + 1, (cols / 2) - 11);
	fflush(stdout);

	sleep(9);
	printf("\033[0m\033[?1049l");
}

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

	int random_msg = rand() % (sizeof(messages) / sizeof(messages[0]));

	printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
	printf("SYSTEM EVENT LOG: %s\n", messages[random_msg]);
	printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
	fflush(stdout);
	sleep(8);
}

void feature_calendar(void) {
	time_t now = time(NULL);
	struct tm* timeinfo = localtime(&now);

	int fake_day = (timeinfo->tm_mday % 31) + 1;
	int fake_month = (timeinfo->tm_mon % 12) + 1;
	char current_date[64], recorded_date[64];

	snprintf(current_date, sizeof(current_date), "Current system date: %04d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
	snprintf(recorded_date, sizeof(recorded_date), "Recorded date: %04d-%02d-%02d", timeinfo->tm_year + 1900, fake_month, fake_day);

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
	printf("\n╔══════════════════════════════════════════════════════════════╗\n");
	printf("║ CRITICAL SYSTEM DIAGNOSTIC REPORT!                          ║\n");
	printf("╚══════════════════════════════════════════════════════════════╝\n\n");
	printf("CPU Status:        WARNING - Throttled by 67%%\n");
	printf("RAM Usage:         CRITICAL - 94.2%% utilized\n");
	printf("Disk I/O:          FAULT - Write cache corrupted\n");
	printf("Network:           UNSTABLE - 47 packet losses\n");
	printf("Thermal:           CRITICAL - CPU at 89°C\n");
	printf("Power:             ABNORMAL - Battery degradation 33%%\n\n");
	printf("RECOMMENDATION: Contact your system administrator IMMEDIATELY\n");
	/* Signature appended discreetly */
	printf("Reference code: PANIC-0xDEADBEEF-TNUNO-MO\n\n");
	fflush(stdout);
	sleep(3);
}


/* ==============================================================================
 * ENUMS & DISPATCHER
 * ============================================================================*/

typedef enum {
	FEAT_BELL,
	FEAT_MESSAGE,
	FEAT_BLOCK_SCREEN,
	FEAT_FLASH,
	FEAT_ALERT_SCREEN,
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

void run_feature(const char* feature_name) {
	char upper_name[64];
	strncpy(upper_name, feature_name, sizeof(upper_name) - 1);
	upper_name[sizeof(upper_name) - 1] = '\0';

	for (int i = 0; upper_name[i]; i++) {
		if (upper_name[i] >= 'a' && upper_name[i] <= 'z') upper_name[i] -= 32;
	}

	if (strcmp(upper_name, "BELL") == 0) feature_bell();
	else if (strcmp(upper_name, "MESSAGE") == 0) feature_message();
	else if (strcmp(upper_name, "BLOCK_SCREEN") == 0) feature_block_screen();
	else if (strcmp(upper_name, "FLASH") == 0) feature_flash();
	else if (strcmp(upper_name, "ALERT_SCREEN") == 0) feature_alert_screen();
	else if (strcmp(upper_name, "CALENDAR") == 0) feature_calendar();
	else if (strcmp(upper_name, "SYSINFO") == 0) feature_sysinfo();
	else if (strcmp(upper_name, "UPSIDE_DOWN") == 0) feature_upside_down();
	else if (strcmp(upper_name, "CAPS_ON") == 0) feature_caps_on();
	else if (strcmp(upper_name, "MOUSE_JITTER") == 0) feature_mouse_jitter();
	else if (strcmp(upper_name, "MOUSE_TELEPORT") == 0) feature_mouse_teleport();
	else if (strcmp(upper_name, "MOUSE_SWAP") == 0) feature_mouse_swap();
	else if (strcmp(upper_name, "CHASING_BUTTON") == 0) feature_chasing_button();
	else if (strcmp(upper_name, "CUSTOM_CURSOR") == 0) feature_custom_cursor();
	else if (strcmp(upper_name, "MOUSE_POOP") == 0) feature_mouse_poop();
	else if (strcmp(upper_name, "CLICK_DISABLE") == 0) feature_click_disable();
	else if (strcmp(upper_name, "TERMINAL_LOOP") == 0) feature_terminal_loop();
	else if (strcmp(upper_name, "BRIGHTNESS_PULSE") == 0) feature_brightness_pulse();
	else if (strcmp(upper_name, "GRAYSCALE") == 0) feature_grayscale();
	else if (strcmp(upper_name, "MATRIX") == 0) feature_matrix();
	else {
		fprintf(stderr, "ERROR: Unknown feature '%s'\n", feature_name);
		exit(1);
	}
}

void run_random_feature(const char* homedir) {
	Feature enabled_features[FEAT_COUNT];
	int enabled_count = 0;

	for (int i = 0; i < FEAT_COUNT; i++) {
		if (is_feature_enabled(homedir, FEATURE_NAMES[i])) {
			enabled_features[enabled_count++] = i;
		}
	}

	if (enabled_count == 0) {
		log_message(homedir, "No features enabled. Exiting.");
		return;
	}

	Feature chosen_feature = enabled_features[rand() % enabled_count];
	
	char msg[256];
	snprintf(msg, sizeof(msg), "Executing random feature: %s", FEATURE_NAMES[chosen_feature]);
	log_message(homedir, msg);

	run_feature(FEATURE_NAMES[chosen_feature]);
}

/* ==============================================================================
 * MAIN
 * ============================================================================*/

int main(int argc, char* argv[]) {
	srand(time(NULL) ^ getpid());
	const char* homedir = get_home_dir();

	char anoypc_dir[512];
	snprintf(anoypc_dir, sizeof(anoypc_dir), "%s/%s", homedir, ANOY_DIR);

	if (access(anoypc_dir, F_OK) != 0) {
		fprintf(stderr, "ERROR: ~/%s directory does not exist.\n", ANOY_DIR);
		fprintf(stderr, "Please run 'make test' or 'make install' first.\n");
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
