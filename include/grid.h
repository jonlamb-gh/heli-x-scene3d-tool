/**
 * @file grid.h
 * @brief TODO.
 *
 */

#ifndef GRID_H
#define GRID_H

#include "gui_types.h"

// simple 2D grid for now
typedef struct
{
    double width;
    double line_spacing;
    double line_width;
    color_4d_s line_color;
} grid_s;

void grid_init(
        const config_s * const config,
        grid_s * const grid);

void grid_render(
        const grid_s * const grid);

#endif /* GRID_H */
