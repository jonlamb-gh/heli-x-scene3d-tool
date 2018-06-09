/**
 * @file grid.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "config.h"
#include "gl_headers.h"
#include "gui_types.h"
#include "gui_util.h"
#include "grid.h"

static void render_axis_lines(
        const grid_s * const grid)
{
    glLineWidth(grid->line_width);

    // draw XYZ axis lines
    glColor4d(0.0, 0.0, 1.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(grid->width/2, 0.0, 0.0);

    glColor4d(1.0, 0.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, grid->width/2);

    glColor4d(0.0, 1.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, grid->width/2, 0.0);
}

static void render_cartesian_grid(
        const grid_s * const grid)
{
    glLineWidth(grid->line_width);
    glColor4dv(grid->line_color.rgba);

    double pos;
    for(pos = -grid->width/2; pos <= grid->width/2; pos += grid->line_spacing)
    {
        glVertex3d(grid->width/2, 0.0, pos);
        glVertex3d(-grid->width/2, 0.0, pos);

        glVertex3d(pos, 0.0, grid->width/2);
        glVertex3d(pos, 0.0, -grid->width/2);
    }
}

void grid_init(
        const config_s * const config,
        grid_s * const grid)
{
    // TODO - configs
    grid->width = 400.0;
    grid->line_spacing = 1.0;
    grid->line_width = CONFIG_LINE_WIDTH;
    grid->line_color.rgba[0] = 0.3;
    grid->line_color.rgba[1] = 0.3;
    grid->line_color.rgba[2] = 0.3;
    grid->line_color.rgba[3] = 0.8;

    assert(grid->width > 0.0);
    assert(grid->line_spacing > 0.0);
}

void grid_render(
        const grid_s * const grid)
{
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_LINES);

    render_cartesian_grid(grid);

    render_axis_lines(grid);

    glEnd();

    glEnable(GL_DEPTH_TEST);
}
