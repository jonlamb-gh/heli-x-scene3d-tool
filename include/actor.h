/**
 * @file actor.h
 * @brief TODO.
 *
 */

#ifndef ACTOR_H
#define ACTOR_H

#include "gui_types.h"

typedef struct
{
    double size[3];
    double pos[3];
} actor_s;

void actor_init(
        const config_s * const config,
        actor_s * const actor);

void actor_render(
        const actor_s * const actor);

#endif /* ACTOR_H */
