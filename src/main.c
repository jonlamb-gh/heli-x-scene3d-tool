/**
 * @file main.c
 * @brief Main.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <popt.h>

#include "config.h"
#include "gui_types.h"
#include "gui.h"

static volatile sig_atomic_t g_exit_signaled;

static void sig_handler(
        int sig)
{
    if(sig == SIGINT)
    {
        g_exit_signaled = 1;
    }
}

int main(
        int argc,
        char **argv)
{
    int ret = 0;
    config_s config;
    struct sigaction sigact;

    g_exit_signaled = 0;
    (void) memset(&config, 0, sizeof(config));
    (void) memset(&sigact, 0, sizeof(sigact));

    sigact.sa_flags = SA_RESTART;
    sigact.sa_handler = sig_handler;

    config.win_width = (int) CONFIG_WINDOW_WIDTH;
    config.win_height = (int) CONFIG_WINDOW_HEIGHT;

    if(ret == 0)
    {
        ret = sigaction(SIGINT, &sigact, 0);
    }

    if(ret == 0)
    {
        ret = gui_init(
                &config,
                CONFIG_WINDOW_TITLE);
    }

    // don't start if we've encountered an error
    if(ret != 0)
    {
        g_exit_signaled = 1;

        (void) fprintf(
                stderr,
                "error during initialization\n");
    }

    // TODO - should I control redraws or just let GL do it?
    // could move the signal handler into the GUI module
    while((g_exit_signaled == 0) && (ret == 0))
    {
        gui_display();

        if(gui_is_window_closed() != 0)
        {
            g_exit_signaled = 1;
        }
    }

    gui_fini();

    if(ret == 0)
    {
        ret = EXIT_SUCCESS;
    }
    else
    {
        ret = EXIT_FAILURE;
    }

    return ret;
}
