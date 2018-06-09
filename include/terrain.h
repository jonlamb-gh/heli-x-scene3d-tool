/**
 * @file terrain.h
 * @brief TODO.
 *
 */

#ifndef TERRAIN_H
#define TERRAIN_H

#include "gl_headers.h"
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

    // TODO - make a container struct? likely used with texture/alphamap
    char heightmap_filename[256];
    GLuint heightmap_width;
    GLuint heightmap_height;
    GLuint heightmap_size;
    unsigned char *heightmap_data;

    GLuint num_vertices;
    GLuint num_triangles;
    GLuint num_indices;
    point_3d_s *vertex_buffer;
    GLuint *index_buffer;

    // TODO
    // heightmap
    // alphamap
    // texture_r
    // texture_g
    // texture_b
    GLuint vertex_buffer_id;
    GLuint normal_buffer_id;
    GLuint index_buffer_id;
    //GLuint color_buffer_id;
    //GLuint texture0_buffer_id;
    //GLuint texture1_buffer_id;
    //GLuint texture2_buffer_id;

    GLuint texture0;
    GLuint texture1;
    GLuint texture2;
} terrain_s;

int terrain_init(
        const config_s * const config,
        terrain_s * const terrain);

void terrain_fini(
        terrain_s * const terrain);

void terrain_render(
        const terrain_s * const terrain);

#endif /* TERRAIN_H */
