/**
 * @file gui.c
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
    view_s view;
    grid_s grid;
    terrain_s terrain;
} gui_s;

static gui_s g_gui;

static void gl_close_func(void)
{
    g_gui.window.id = -1;
}

static void gl_reshape_func(
        int w,
        int h)
{
    //const double aspect_ratio = 1.0;
    const double aspect_ratio = (double) w / (double) h;
    const double up_vector[3] = CONFIG_VIEW_UP_VECTOR_INITIALIZER;

    g_gui.window.width = (unsigned long) w;
    g_gui.window.height = (unsigned long) h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
            g_gui.view.fov,
            aspect_ratio,
            g_gui.view.z_near,
            g_gui.view.z_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
            g_gui.view.eye_pos[0], g_gui.view.eye_pos[1], g_gui.view.eye_pos[2],
            g_gui.view.center_pos[0], g_gui.view.center_pos[1], g_gui.view.center_pos[2],
            up_vector[0], up_vector[1], up_vector[2]);

    glutPostRedisplay();
}

static void gl_key_func(
        unsigned char key,
        int x,
        int y)
{
    const double center_dx = 0.5;
    const double center_dz = 0.5;

    if((key == '\e') || (key == 'q'))
    {
        gl_close_func();
    }
    else if(key == 'r')
    {
        view_init(&g_gui.config, &g_gui.view);
    }
    else if(key == 'w')
    {
        view_adj_center_pos(-center_dx, 0.0, &g_gui.view);
    }
    else if(key == 's')
    {
        view_adj_center_pos(center_dx, 0.0, &g_gui.view);
    }
    else if(key == 'a')
    {
        view_adj_center_pos(0.0, center_dz, &g_gui.view);
    }
    else if(key == 'd')
    {
        view_adj_center_pos(0.0, -center_dz, &g_gui.view);
    }

    gl_reshape_func(
            (int) g_gui.window.width,
            (int) g_gui.window.height);
}

static void gl_special_key_func(
        int key,
        int x,
        int y)
{
    double d_radius = 0.0;
    double d_theta = 0.0;
    double d_phi = 0.0;
    const double d_angle = RAD(5.0);
    const double d_r = 0.5;

    if(key == GLUT_KEY_PAGE_UP)
    {
        d_radius = -d_r;
    }
    else if(key == GLUT_KEY_PAGE_DOWN)
    {
        d_radius = d_r;
    }
    else if(key == GLUT_KEY_LEFT)
    {
        d_phi = d_angle;
    }
    else if(key == GLUT_KEY_RIGHT)
    {
        d_phi = -d_angle;
    }
    else if(key == GLUT_KEY_UP)
    {
        d_theta = -d_angle;
    }
    else if(key == GLUT_KEY_DOWN)
    {
        d_theta = d_angle;
    }

    view_adj_eye_pos(
            d_radius,
            d_theta,
            d_phi,
            &g_gui.view);

    gl_reshape_func(
            (int) g_gui.window.width,
            (int) g_gui.window.height);
}

static void gl_display_func(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(CONFIG_BACKGROUND_RGBA);

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glLineWidth((GLfloat) CONFIG_LINE_WIDTH);

    glPointSize(1.0f);

    glEnable(GL_BLEND);

    grid_render(&g_gui.grid);

    // TODO - testing depth/3D bits
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glutWireCube(200.0);
    glutWireCube(100.0);
    glutWireCube(10.0);

    glutSolidCube(1.0);


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

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(g_gui.window.width, g_gui.window.height);

    g_gui.window.id = glutCreateWindow(g_gui.window.title);

    if(g_gui.window.id < 0)
    {
        ret = 1;
    }

    if(ret == 0)
    {
        view_init(config, &g_gui.view);

        grid_init(config, &g_gui.grid);

        ret = terrain_init(config, &g_gui.terrain);
    }

    if(ret == 0)
    {
        glutCloseFunc(gl_close_func);
        glutReshapeFunc(gl_reshape_func);
        glutDisplayFunc(gl_display_func);
        glutKeyboardFunc(gl_key_func);
        glutSpecialFunc(gl_special_key_func);

        glEnable(GL_DEPTH);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);

        // ambient lighting
        const double amb = 0.3;
        const double spec = 1.0;
        const double diff = 1.0;
        const GLfloat global_ambient[] = {amb, amb, amb, 0.1};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

        const GLfloat light_pos[] = {0.0, 100.0, 0.0, 0.0};
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

        const GLfloat light_ambient[] = {amb, amb, amb, 1.0};
        const GLfloat light_diffuse[] = {diff, diff, diff, 1.0};
        const GLfloat light_specular[] = {spec, spec, spec, 1.0};

        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

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
