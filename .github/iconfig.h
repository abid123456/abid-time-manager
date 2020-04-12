/* iconfig.h - configurations with abbreviating macros, to be included */

#ifndef ICONFIG_H_INCLUDED
#define ICONFIG_H_INCLUDED

#include "config.h"

#define PART_WIDTH (MA_WIDTH/2)
#define PART_DURATION (60/PART_WIDTH)

#define MARGIN_HRZT (MARGIN_LEFT+MARGIN_RIGHT)
#define MARGIN_VERT (MARGIN_TOP+MARGIN_BOTTOM)

#define WINDOW_WIDTH  ICON_SIZE*(MARGIN_HRZT+MA_WIDTH+MID_SEPARATOR)
#define WINDOW_HEIGHT ICON_SIZE*(MARGIN_VERT+MA_HEIGHT)

#define CA_ZERO NUM_ICONS_MAIN

#endif // ICONFIG_H_INCLUDED
