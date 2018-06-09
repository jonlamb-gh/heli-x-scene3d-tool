/**
 * @file terrain.h
 * @brief TODO.
 *
 */

#ifndef TERRAIN_H
#define TERRAIN_H

#include "gui_types.h"

typedef struct
{
    double width;
    double center_x;
    double center_z;
    double height_min;
    double height_max;
    // determines the space between terrain vertices in world units for both the X and Z axes
    double block_scale;
    // TODO
    // heightmap
    // alphamap
    // texture_r
    // texture_g
    // texture_b
} terrain_s;

int terrain_init(
        const config_s * const config,
        terrain_s * const terrain);

void terrain_fini(
        terrain_s * const terrain);

void terrain_render(
        const terrain_s * const terrain);

#endif /* TERRAIN_H */
