/* awindows.h - header for awindows.cpp */

#ifndef AWINDOWS_H_INCLUDED
#define AWINDOWS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <cstring>
#include "iconfig.h"
#include "ssprites.h"
#include "svectors.h"

/* window IDs */
#define WIN_GENERIC  1
#define WIN_SAVE_BOX 2
#define WIN_OPEN_BOX 3
#define WIN_ACTIVITY 4

/* window component IDs */
#define TEXT_FIELD 1
#define TEXT_BOX   2

using namespace sf;

typedef struct {
    int comp_id;
    IntRect r_relative;
    char text[MAX_TEXT_LENGTH+1];
    bool visible;
    void *parent;
} win_comp; /* window component */

typedef struct {
    int window_id;
    IntRect r; /* 3rd and 4th members multiples of ICON_SIZE */
    char title[MAX_WINDOW_TITLE_LENGTH+1];
    win_comp* pcomp[MAX_COMP_IN_WINDOW];
    int comp_count;
} window;

void draw_window(RenderWindow *rw, window *wp);
window *init_window(int window_id, IntRect r, const char *title);
win_comp *init_window_comp(window *wp, int comp_id, IntRect r_relative);
win_comp *init_window_comp(window *wp, int comp_id, IntRect r_relative, const char *text);
void destroy_window(window *wp);
void focus_window(window *wp);
void focus_component(win_comp *wcp);

#endif // AWINDOWS_H_INCLUDED
