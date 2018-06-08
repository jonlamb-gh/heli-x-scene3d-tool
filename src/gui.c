/**
 * @file gui.c
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
#include "grid.h"
#include "terrain.h"
#include "gui.h"

typedef struct
{
    int gl_argc;
    char **gl_argv;
    GLenum gl_err;
    window_s window;
    config_s config;
    grid_s grid;
    terrain_s terrain;
} gui_s;

static gui_s g_gui;

static void gl_close_func(void)
{
    g_gui.window.id = -1;
}

static void gl_key_func(
        unsigned char key,
        int x,
        int y)
{
    if((key == '\e') || (key == 'q'))
    {
        gl_close_func();
    }
}

static void gl_reshape_func(
        int w,
        int h)
{
    // TODO - zoom
    const double zoom_factor = 1.0;
    const double fov_degrees = 50.0;
    //const double aspect_ratio = 1.0;
    const double aspect_ratio = (double) w / (double) h;
    const double z_near = 1.0;
    const double z_far = 50.0;

    const double eye_pos[3] = {15.0, 15.0, 5.0};
    const double center_pos[3] = {0.0, 0.0, 0.0};
    const double up_vector[3] = {0.0, 1.0, 0.0};

    g_gui.window.width = (unsigned long) w;
    g_gui.window.height = (unsigned long) h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
            fov_degrees * zoom_factor,
            aspect_ratio,
            z_near,
            z_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
            eye_pos[0], eye_pos[1], eye_pos[2],
            center_pos[0], center_pos[1], center_pos[2],
            up_vector[0], up_vector[1], up_vector[2]);

    glutPostRedisplay();
}

static void gl_display_func(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(CONFIG_BACKGROUND_RGBA);

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glLineWidth((GLfloat) CONFIG_LINE_WIDTH);

    glPointSize(1.0f);

    glEnable(GL_BLEND);

    // TODO - testing depth/3D bits
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glutWireCube(200.0);
    glutWireCube(100.0);
    glutWireCube(10.0);
    glutWireCube(1.0);

    grid_render(&g_gui.grid);

    glutSwapBuffers();
}

int gui_init(
        const config_s * const config,
        const char * const win_title)
{
    int ret = 0;

    (void) memset(&g_gui, 0, sizeof(g_gui));

    (void) strncpy(
            g_gui.window.title,
            win_title,
            sizeof(g_gui.window.title));

    g_gui.window.width = (unsigned long) config->win_width;
    g_gui.window.height = (unsigned long) config->win_height;

    glutInit(&g_gui.gl_argc, g_gui.gl_argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutInitWindowSize(g_gui.window.width, g_gui.window.height);

    g_gui.window.id = glutCreateWindow(g_gui.window.title);

    if(g_gui.window.id < 0)
    {
        ret = 1;
    }
    
    if(ret == 0)
    {
        grid_init(config, &g_gui.grid);
    }

    if(ret == 0)
    {
        glutCloseFunc(gl_close_func);
        glutReshapeFunc(gl_reshape_func);
        glutDisplayFunc(gl_display_func);
        glutKeyboardFunc(gl_key_func);

        glEnable(GL_DEPTH);
        glDisable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glutSetOption(
                GLUT_ACTION_ON_WINDOW_CLOSE,
                GLUT_ACTION_GLUTMAINLOOP_RETURNS);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glutPostRedisplay();

        glutMainLoopEvent();
    }

    return ret;
}

void gui_fini(void)
{
    if(g_gui.window.id >= 0)
    {
        glutExit();
    }
}

int gui_is_window_closed(void)
{
    int ret;

    if(g_gui.window.id >= 0)
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }

    return ret;
}

void gui_display(void)
{
    if(g_gui.window.id >= 0)
    {
        glutPostRedisplay();

        glutMainLoopEvent();
    }
}
