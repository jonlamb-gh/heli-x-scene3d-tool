/**
 * @file terrain.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "config.h"
#include "lodepng.h"
#include "gl_headers.h"
#include "gui_types.h"
#include "gui_util.h"
#include "terrain.h"

static int load_heightmap_image(
        const char * const filename,
        terrain_s * const terrain)
{
    // TODO - assertions
    unsigned int err = 0;
    unsigned int width = 0;
    unsigned int height = 0;

    (void) strncpy(
            terrain->heightmap_filename,
            filename,
            sizeof(terrain->heightmap_filename));

    err = lodepng_decode_file(
            &terrain->heightmap_data,
            &width,
            &height,
            filename,
            LCT_GREY,
            8);

    if(err == 0)
    {
        assert(terrain->heightmap_data != NULL);

        terrain->heightmap_width = (GLuint) width;
        terrain->heightmap_height = (GLuint) height;
        terrain->heightmap_size = (GLuint) (width * height * 1);
    }

    if(err != 0)
    {
        (void) fprintf(
                stderr,
                "failed to read heightmap file '%s' - %s\n",
                filename,
                lodepng_error_text(err));
    }

    return (int) err;
}

static int allocate_buffers(
        terrain_s * const terrain)
{
    int ret = 0;

    terrain->num_vertices = terrain->heightmap_size;

    if(ret == 0)
    {
        terrain->vertex_buffer = calloc(
                terrain->num_vertices,
                sizeof(*terrain->vertex_buffer));

        if(terrain->vertex_buffer == NULL)
        {
            (void) fprintf(
                    stderr,
                    "failed to allocate TODO bytes\n");
            ret = 1;
        }
    }

    if(ret == 0)
    {
        glGenBuffers(1, &terrain->vertex_buffer_id);
    }

    return ret;
}

static int generate_heightmap_vbo(
        terrain_s * const terrain)
{
    int ret = 0;

    // TODO
    //const GLdouble height_scale = ABS(terrain->min - terrain->max) / 255.0;
    const GLdouble height_scale = -50.0;

    const GLdouble terrain_width =
            (GLdouble) (terrain->heightmap_width - 1) * terrain->block_scale;

    const GLdouble terrain_height =
            (GLdouble) (terrain->heightmap_height - 1) * terrain->block_scale;

    const double half_terrain_width = terrain_width / 2.0;
    const double half_terrain_height = terrain_height / 2.0;

    GLuint x;
    GLuint y;
    for(y = 0; y < terrain->heightmap_height; y += 1)
    {
        for(x = 0; x < terrain->heightmap_width; x += 1)
        {
            const GLuint index = (y * terrain->heightmap_width) + x;

            const GLdouble height_value =
                    ((GLdouble) terrain->heightmap_data[index]) / 255.0;

            const GLdouble s =
                    ((GLdouble) x / (GLdouble) (terrain->heightmap_width - 1));
            const GLdouble t =
                    ((GLdouble) y / (GLdouble) (terrain->heightmap_height - 1));

            terrain->vertex_buffer[index].xyz[0] =
                    (s * terrain_width) - half_terrain_width;
            terrain->vertex_buffer[index].xyz[1] =
                    (height_value * height_scale);
            terrain->vertex_buffer[index].xyz[2] =
                    (t * terrain_height) - half_terrain_height;
        }
    }

    if(ret == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, terrain->vertex_buffer_id);

        glBufferData(
                GL_ARRAY_BUFFER,
                terrain->num_vertices * sizeof(*terrain->vertex_buffer),
                &terrain->vertex_buffer[0],
                GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return ret;
}

int terrain_init(
        const config_s * const config,
        terrain_s * const terrain)
{
    int ret = 0;

    // TODO
    terrain->width = 400.0;
    terrain->center_x = 0.0;
    terrain->center_z = 0.0;

    terrain->height_min = -50.0;
    terrain->height_max = 0.0;

    terrain->block_scale = 1.0;

    if(ret == 0)
    {
        ret = load_heightmap_image("heightmap.png", terrain);
    }

    if(ret == 0)
    {
        ret = allocate_buffers(terrain);
    }

    if(ret == 0)
    {
        ret = generate_heightmap_vbo(terrain);
    }

    return ret;
}

void terrain_fini(
        terrain_s * const terrain)
{
    if(terrain->vertex_buffer_id != 0)
    {
        glDeleteBuffers(1, &terrain->vertex_buffer_id);
    }

    if(terrain->heightmap_data != NULL)
    {
        free(terrain->heightmap_data);
        terrain->heightmap_data = NULL;
    }

    if(terrain->vertex_buffer != NULL)
    {
        free(terrain->vertex_buffer);
        terrain->vertex_buffer = NULL;
    }
}

void terrain_render(
        const terrain_s * const terrain)
{
    // TODO
    glColor4d(0.0, 1.0, 0.0, 1.0);

    glBindBuffer(GL_ARRAY_BUFFER, terrain->vertex_buffer_id);

    glVertexPointer(3, GL_DOUBLE, 0, NULL);

    glEnableClientState(GL_VERTEX_ARRAY);

    glDrawArrays(GL_POINTS, 0, terrain->num_vertices);
    //glDrawArrays(GL_TRIANGLES, 0, terrain->num_vertices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
