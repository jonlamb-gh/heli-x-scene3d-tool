/**
 * @file actor.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "gl_headers.h"
#include "gui_types.h"
#include "gui_util.h"
#include "actor.h"

void actor_init(
        const config_s * const config,
        actor_s * const actor)
{
    actor->pos[0] = 0.0;
    actor->pos[1] = 0.0;
    actor->pos[2] = 0.0;

    actor->size[0] = 1.0;
    actor->size[1] = 1.0;
    actor->size[2] = 1.0;
}

void actor_render(
        const actor_s * const actor)
{
    glColor4d(1.0, 0.0, 1.0, 1.0);

    glPushMatrix();

    glScaled(
            actor->size[0],
            actor->size[1],
            actor->size[2]);

    glTranslated(
            actor->pos[0],
            actor->pos[1],
            actor->pos[2]);

    glutWireCube(1.0);
    glutSolidCube(0.25);

    glPopMatrix();
}
