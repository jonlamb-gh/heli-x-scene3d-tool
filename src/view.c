/**
 * @file view.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "config.h"
#include "gl_headers.h"
#include "gui_types.h"
#include "gui_util.h"
#include "view.h"

void view_init(
        const config_s * const config,
        view_s * const view)
{
    // TODO
    view->radius = 20.0;
    view->theta = RAD(10.0);
    view->phi = RAD(10.0);

    view->fov = CONFIG_VIEW_FOV_DEGREES;
    view->z_near = CONFIG_VIEW_FOV_Z_NEAR;
    view->z_far = CONFIG_VIEW_FOV_Z_FAR;

    view->center_pos[0] = 0;
    view->center_pos[1] = 0;
    view->center_pos[2] = 0;

    view_update_eye_pos(view);
}

void view_update_eye_pos(
        view_s * const view)
{
    // TODO
    view->eye_pos[0] = view->radius * sin(view->theta) * cos(view->phi);
    view->eye_pos[1] = view->radius * cos(view->theta);
    view->eye_pos[2] = view->radius * sin(view->theta) * sin(view->phi);

    printf("(%.4f, %0.4f, %0.4f)\n", view->radius, DEG(view->theta), DEG(view->phi));
    printf("(%.4f, %0.4f, %0.4f)\n", view->eye_pos[0], view->eye_pos[1], view->eye_pos[2]);
}

void view_adj_eye_pos(
        const double dr,
        const double dt,
        const double dp,
        view_s * const view)
{
    // TODO - constraints
    view->radius += dr;
    view->theta += dt;
    view->phi += dp;

    view_update_eye_pos(view);
}
