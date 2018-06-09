/**
 * @file view.h
 * @brief TODO.
 *
 */

#ifndef VIEW_H
#define VIEW_H

#include "gui_types.h"

typedef struct
{
    double radius;
    double theta;
    double phi;
    double fov; // degrees
    double z_near;
    double z_far;
    double eye_pos[3];
    double center_pos[3];
} view_s;

void view_init(
        const config_s * const config,
        view_s * const view);

void view_update_eye_pos(
        view_s * const view);

void view_adj_eye_pos(
        const double dr,
        const double dt,
        const double dp,
        view_s * const view);

void view_adj_center_pos(
        const double dx,
        const double dz,
        view_s * const view);

#endif /* VIEW_H */
