/**
 * @file gui.h
 * @brief TODO.
 *
 */

#ifndef GUI_H
#define GUI_H

#include "gui_types.h"

int gui_init(
        const config_s * const config,
        const char * const win_title);

void gui_fini(void);

int gui_is_window_closed(void);

void gui_display(void);

#endif /* GUI_H */
