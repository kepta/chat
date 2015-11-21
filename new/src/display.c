#include <display.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define min(a,b) ((a)>(b)?(b):(a))

WINDOW *main_screen;
static int intensity_threshold = 25;
static double saturation = 2.0;
static char *ascii_values = " ..::--==+++***###%%%%%%%%@@@@@@@";
// static char *ascii_values = " ..::--==░░░░░░░▒▒▒▒▒▒▒▓▓▓▓▓█████";

static int monochrome = 0;
char gr; char gg; char gb;

/* private functions */
void init_colors(void);

void init_screen(display_options_t *dopt){
  main_screen = initscr();
  keypad(stdscr, TRUE);           // keypad enabled
  (void) nodelay(main_screen, 1); // no blocking
  (void) noecho();                // don't print to screen
  (void) curs_set(FALSE);         // don't show the cursor
  (void) nonl();                  // no new lines
  init_colors();
  gr = dopt->r;
  gg = dopt->g;
  gb = dopt->b;
  monochrome = dopt->monochrome;
  if (dopt->ascii_values) {
    ascii_values = dopt->ascii_values;
  }
  if (dopt->saturation >= 0.0) {
    saturation = dopt->saturation;
  }
  if (dopt->intensity_threshold) {
    intensity_threshold = dopt->intensity_threshold;
  }
}

/*
 * crazy shifting is to set up every color
 */
void init_colors(void) {
  int i;
  start_color();
  if (COLORS < 255) {
    for (i = 0; i < (1 << 8); i ++) {
      init_pair(i, 255, 0); // 0 --> i if you want pure blocks, otherwise ascii
    }
  } else {
    for (i = 0; i < (1 << 8); i ++) {
      init_pair(i, i, 0); // 0 --> i if you want pure blocks, otherwise ascii
    }
  }
  return;
}

void end_screen(void) {
  endwin();
}

#define  PR  .299
#define  PG  .587
#define  PB  .114

void saturate(int *r, int *g, int *b, double change) {
  double p = sqrt((*r)*(*r)*PR + (*g)*(*g)*PG + (*b)*(*b)*PB);

  *r = abs(p + ((*r) - p) * change);
  *g = abs(p + ((*g) - p) * change);
  *b = abs(p + ((*b) - p) * change);
}

/* allow us to directly map to the 216 colors ncurses makes available */
static inline unsigned int get_color(int r, int g, int b) {
  unsigned int f;
  if (monochrome) {
    f = (16+gr/48*36+gg/48*6+gb/48);
  } else {
    saturate(&r, &g, &b, saturation);
    f = (16+r/48*36+g/48*6+b/48);
  }
  return f;
}

chtype to_braille(unsigned char byte) {
  return 10240 + (
  ((byte >> 7) & (1 << 0)) |
  ((byte >> 3) & (1 << 3)) |
  ((byte >> 4) & (1 << 1)) |
  ((byte >> 0) & (1 << 4)) |
  ((byte >> 1) & (1 << 2)) |
  ((byte << 3) & (1 << 5)) |
  ((byte << 5) & (1 << 6)) |
  ((byte << 7) & (1 << 7))
  );
}

unsigned char from_braille(chtype c) {
  char byte = (c - 10240) & 0xFF;
  return
  ((byte << 7) & (1 << 7)) |
  ((byte << 3) & (1 << 6)) |
  ((byte << 4) & (1 << 5)) |
  ((byte << 0) & (1 << 4)) |
  ((byte << 1) & (1 << 3)) |
  ((byte >> 3) & (1 << 2)) |
  ((byte >> 5) & (1 << 1)) |
  ((byte >> 7) & (1 << 0));
}

/* if on is nonzero it will turn the pixel on, else off */
chtype add_pixel(chtype c, int row, int col, int on) {
  unsigned char byte = from_braille(c);
  if (on) {
    return to_braille(byte | (1 << (2 * row + col)));
  } else {
    return to_braille(byte & (~(1 << (2 * row + col))));
  }
}

int draw_braille(char *data, int width, int y, int channels) {
  int j;
  int row = y/4;
  unsigned char b, g, r;
  int intensity;
  for (j = 0; j < width; j++) {
    b = data[j * channels];
    g = data[j * channels + 1];
    r = data[j * channels + 2];
    intensity = 100 * ((r/255.0 + g/255.0 + b/255.0) / 3);
    int color = get_color(r, g, b);

    char braille[2];
    attron(COLOR_PAIR(color));
    if (y % 4 == 0) {
      sprintf(braille, "%C", to_braille(0));
    }
    if (intensity > intensity_threshold) {
      sprintf(braille, "%C", add_pixel(mvinch(row, j / 2), 3 - (y % 4), 1 - (j % 2), 1));
    } else {
      sprintf(braille, "%C", add_pixel(mvinch(row, j / 2), 3 - (y % 4), 1 - (j % 2), 0));
    }
    mvaddstr(row, j / 2, braille);
  }
  if (y == 0) {
    refresh();
  }
  return 0;
}

int draw_line(char *data, int width, int y, int channels) {
  int j;
  unsigned char b, g, r;
  int intensity;
  for (j = 0; j < width; j++){
    b = data[j * channels];
    g = data[j * channels + 1];
    r = data[j * channels + 2];
    intensity = (strlen(ascii_values) - 1) * ((r/255.0 + g/255.0 + b/255.0) / 3);
    char val = ascii_values[intensity];
    int color = get_color(r, g, b);
    if (COLORS < 255) {
      color = 0;
    }
    mvaddch(y, j, val|COLOR_PAIR(color));
  }

  if (y == 0) {
    refresh();
  }
  return 0;
}

int write_bandwidth(char *bandstr, int bandlen, int width, int height) {
  int i;

  if (width < bandlen) {
    /* can't write it to the screen */
    return -1;
  }

  for (i = width - bandlen; i < width; i++) {
    mvaddch(0, i, bandstr[i - width + bandlen]);
  }

  refresh();
  return 0;
}
