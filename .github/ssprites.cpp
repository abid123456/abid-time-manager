/* ssprites.cpp - support for sprites */

#include <SFML/Graphics.hpp>
#include "iconfig.h"

using namespace sf;

IntRect ss_vert(int i) { return IntRect(0,ICON_SIZE*i,ICON_SIZE,ICON_SIZE); } /* sprite select vertical */
IntRect ss_vert(int i, int n) { return IntRect(0,ICON_SIZE*i,ICON_SIZE,ICON_SIZE*n); } /* sprite select vertical (multiple) */
IntRect ss_hv(int x0, int y0, int w, int h) { return IntRect(ICON_SIZE*x0,ICON_SIZE*y0,ICON_SIZE*w,ICON_SIZE*h); }
