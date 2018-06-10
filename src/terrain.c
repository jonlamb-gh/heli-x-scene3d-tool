/**
 * @file terrain.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
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

    // two triangles for every quad in the terrain mesh
    terrain->num_triangles =
            (terrain->heightmap_width - 1) * (terrain->heightmap_height - 1) * 2;

    // 3 indices for each triangle
    terrain->num_indices = terrain->num_triangles * 3;

    if(ret == 0)
    {
        terrain->vertex_buffer = calloc(
                terrain->num_vertices,
                sizeof(*terrain->vertex_buffer));

        if(terrain->vertex_buffer == NULL)
        {
            (void) fprintf(
                    stderr,
                    "failed to allocate TODO bytes to vertex buffer\n");
            ret = 1;
        }
    }

    if(ret == 0)
    {
        terrain->normal_buffer = calloc(
                terrain->num_vertices,
                sizeof(*terrain->normal_buffer));

        if(terrain->normal_buffer == NULL)
        {
            (void) fprintf(
                    stderr,
                    "failed to allocate TODO bytes to normal buffer\n");
            ret = 1;
        }
    }

    if(ret == 0)
    {
        terrain->index_buffer = calloc(
                terrain->num_indices,
                sizeof(*terrain->index_buffer));

        if(terrain->index_buffer == NULL)
        {
            (void) fprintf(
                    stderr,
                    "failed to allocate TODO bytes to index buffer\n");
            ret = 1;
        }
    }

    if(ret == 0)
    {
        glGenBuffers(1, &terrain->vertex_buffer_id);
        glGenBuffers(1, &terrain->normal_buffer_id);
        glGenBuffers(1, &terrain->index_buffer_id);

        assert(terrain->vertex_buffer_id > 0);
        assert(terrain->normal_buffer_id > 0);
        assert(terrain->index_buffer_id > 0);
    }

    return ret;
}

static int generate_heightmap_vbo(
        terrain_s * const terrain)
{
    int ret = 0;

    // TODO
    //const GLdouble height_scale = ABS(terrain->min - terrain->max) / 255.0;
    const GLdouble height_scale = 50.0;
    const GLdouble height_offset = -50.0;

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
                    (height_value * height_scale) + height_offset;
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

static int generate_heightmap_ibo(
        terrain_s * const terrain)
{
    int ret = 0;
    GLuint buffer_index = 0;

    GLuint x;
    GLuint y;
    for(y = 0; y < (terrain->heightmap_height - 1); y += 1)
    {
        for(x = 0; x < (terrain->heightmap_width - 1); x += 1)
        {
            const GLuint vertex_index = (y * terrain->heightmap_width) + x;

            // top triangle T0 v0->v1->V2
            terrain->index_buffer[buffer_index++] =
                    vertex_index;
            terrain->index_buffer[buffer_index++] =
                    vertex_index + terrain->heightmap_width + 1;
            terrain->index_buffer[buffer_index++] =
                    vertex_index + 1;

            // bottom triangle T1 v0->v1->V2
            terrain->index_buffer[buffer_index++] =
                    vertex_index;
            terrain->index_buffer[buffer_index++] =
                    vertex_index + terrain->heightmap_width;
            terrain->index_buffer[buffer_index++] =
                    vertex_index + terrain->heightmap_width + 1;
        }
    }

    if(ret == 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->index_buffer_id);

        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                terrain->num_indices * sizeof(*terrain->index_buffer),
                &terrain->index_buffer[0],
                GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    return ret;
}

static int generate_heightmap_nbo(
        terrain_s * const terrain)
{
    // TODO - some math functions would be nice
    int ret = 0;

    GLuint idx;
    for(idx = 0; idx < terrain->num_indices; idx += 3)
    {
        point_3d_s * const v0 =
                &terrain->vertex_buffer[terrain->index_buffer[idx + 0]];
        point_3d_s * const v1 =
                &terrain->vertex_buffer[terrain->index_buffer[idx + 1]];
        point_3d_s * const v2 =
                &terrain->vertex_buffer[terrain->index_buffer[idx + 2]];

        const point_3d_s a =
        {{
            v1->xyz[0] - v0->xyz[0],
            v1->xyz[1] - v0->xyz[1],
            v1->xyz[2] - v0->xyz[2]
        }};

        const point_3d_s b =
        {{
            v2->xyz[0] - v0->xyz[0],
            v2->xyz[1] - v0->xyz[1],
            v2->xyz[2] - v0->xyz[2]
        }};

        const point_3d_s cross_product =
        {{
            a.xyz[1] * b.xyz[2] - a.xyz[2] * b.xyz[1],
            a.xyz[2] * b.xyz[0] - a.xyz[0] * b.xyz[2],
            a.xyz[0] * b.xyz[1] - a.xyz[1] * b.xyz[0]
        }};

        const GLdouble len = sqrt(
                SQ(cross_product.xyz[0]) +
                SQ(cross_product.xyz[1]) +
                SQ(cross_product.xyz[2]));

        const point_3d_s normal =
        {{
             cross_product.xyz[0] / len,
             cross_product.xyz[1] / len,
             cross_product.xyz[2] / len
        }};

        point_3d_s * const n0 =
                &terrain->normal_buffer[terrain->index_buffer[idx + 0]];
        point_3d_s * const n1 =
                &terrain->normal_buffer[terrain->index_buffer[idx + 1]];
        point_3d_s * const n2 =
                &terrain->normal_buffer[terrain->index_buffer[idx + 2]];

        n0->xyz[0] += normal.xyz[0];
        n0->xyz[1] += normal.xyz[1];
        n0->xyz[2] += normal.xyz[2];

        n1->xyz[0] += normal.xyz[0];
        n1->xyz[1] += normal.xyz[1];
        n1->xyz[2] += normal.xyz[2];

        n2->xyz[0] += normal.xyz[0];
        n2->xyz[1] += normal.xyz[1];
        n2->xyz[2] += normal.xyz[2];
    }

    for(idx = 0; idx < terrain->num_vertices; idx += 1)
    {
        point_3d_s * const vert =
                &terrain->normal_buffer[idx];

        const GLdouble len = sqrt(
                SQ(vert->xyz[0]) +
                SQ(vert->xyz[1]) +
                SQ(vert->xyz[2]));

        const point_3d_s normal =
        {{
             vert->xyz[0] / len,
             vert->xyz[1] / len,
             vert->xyz[2] / len
        }};

        vert->xyz[0] = normal.xyz[0];
        vert->xyz[1] = normal.xyz[1];
        vert->xyz[2] = normal.xyz[2];
    }

    if(ret == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, terrain->normal_buffer_id);

        glBufferData(
                GL_ARRAY_BUFFER,
                terrain->num_vertices * sizeof(*terrain->normal_buffer),
                &terrain->normal_buffer[0],
                GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return ret;
}

static void render_points(
        const terrain_s * const terrain)
{
    // TODO - color/size
    glColor4d(0.0, 1.0, 0.0, 1.0);

    glBindBuffer(GL_ARRAY_BUFFER, terrain->vertex_buffer_id);
    glVertexPointer(3, GL_DOUBLE, 0, NULL);

    glEnableClientState(GL_VERTEX_ARRAY);

    glDrawArrays(
            GL_POINTS,
            0,
            terrain->num_vertices);

    glDisableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void render_triangles(
        const terrain_s * const terrain)
{
    // TODO - color/texture buffers
    glColor4d(0.0, 1.0, 0.0, 1.0);

    glBindBuffer(GL_ARRAY_BUFFER, terrain->vertex_buffer_id);
    glVertexPointer(3, GL_DOUBLE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, terrain->normal_buffer_id);
    glNormalPointer(GL_DOUBLE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->index_buffer_id);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glDrawElements(
            GL_TRIANGLES,
            terrain->num_indices,
            GL_UNSIGNED_INT,
            0);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    if(ret == 0)
    {
        ret = generate_heightmap_ibo(terrain);
    }

    if(ret == 0)
    {
        ret = generate_heightmap_nbo(terrain);
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

    if(terrain->normal_buffer_id != 0)
    {
        glDeleteBuffers(1, &terrain->normal_buffer_id);
    }

    if(terrain->index_buffer_id != 0)
    {
        glDeleteBuffers(1, &terrain->index_buffer_id);
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

     if(terrain->normal_buffer != NULL)
    {
        free(terrain->normal_buffer);
        terrain->normal_buffer = NULL;
    }

    if(terrain->index_buffer != NULL)
    {
        free(terrain->index_buffer);
        terrain->index_buffer = NULL;
    }
}

void terrain_render(
        const terrain_primitive_kind kind,
        const terrain_s * const terrain)
{
    assert(kind >= TERRAIN_PRIMITIVE_POINTS);
    assert(kind <= TERRAIN_PRIMITIVE_TRIANGLES);

    if(kind == TERRAIN_PRIMITIVE_POINTS)
    {
        render_points(terrain);
    }
    else if(kind == TERRAIN_PRIMITIVE_TRIANGLES)
    {
        render_triangles(terrain);
    }
}
