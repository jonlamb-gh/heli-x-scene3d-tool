/**
 * @file gui_types.h
 * @brief TODO.
 *
 */

#ifndef GUI_TYPES_H
#define GUI_TYPES_H

#include "gl_headers.h"

typedef struct
{
    int win_width;
    int win_height;
} config_s;

typedef struct
{
    GLdouble rgba[4];
} color_4d_s;

typedef struct
{
    GLdouble xyz[3];
} point_3d_s;

typedef struct
{
    int id;
    unsigned long width;
    unsigned long height;
    char title[256];
} window_s;

// TODO - move to a module view.h, init with config/etc
typedef struct
{
    double zoom_factor;
    // double radius;
    double theta;
    double phi;

    // TODO - eye pos, etc here
} view_s;

#endif /* GUI_TYPES_H */
