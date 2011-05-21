
/* interface.c
 *
 * This file is part of fizmo.
 *
 * Copyright (c) 2011 Christoph Ender and Andrew Plotkin.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <interpreter/fizmo.h>
#include <interpreter/text.h>
#include <interpreter/streams.h>
#include <tools/unused.h>
#include <tools/types.h>
#include <tools/i18n.h>
#include <tools/tracelog.h>

#include "glk.h"
#include "glkint.h"

static char* interface_name = "fizmo-glk";
static char* interface_version = "0.7.0-b5";

static winid_t mainwin = NULL;
static winid_t statuswin = NULL;
static bool instatuswin = false;

static int inputbuffer_size = 0;
static glui32 *inputbuffer = NULL;


void glkint_open_interface()
{
    mainwin = glk_window_open(NULL, 0, 0, wintype_TextBuffer, 1);
    glk_set_window(mainwin);
    instatuswin = false;
}

/* The following functions all go into the screen interface structure. */


char *glkint_get_interface_name()
{ return interface_name; }

bool glkint_return_false()
{ return false; }

bool glkint_return_true()
{ return true; }

uint8_t glkint_return_0()
{ return 0; }

uint8_t glkint_return_1()
{ return 1; }

uint8_t glkint_get_screen_height()
{ 
    return 24; //###
}

uint8_t glkint_get_screen_width()
{ 
    return 80; //###
}

z_colour glkint_get_default_foreground_colour()
{ return Z_COLOUR_BLACK; }

z_colour glkint_get_default_background_colour()
{ return Z_COLOUR_WHITE; }

int glkint_parse_config_parameter(char *UNUSED(key), char *UNUSED(value))
{ return 1; }

void glkint_link_interface_to_story(struct z_story *UNUSED(story))
{ }

/* Called at @restart time. */
void glkint_reset_interface()
{
    if (statuswin) {
        glk_window_close(statuswin, NULL);
        statuswin = NULL;
    }

    instatuswin = false;
    glk_set_window(mainwin);    
    glk_set_style(style_Normal);
    glk_window_clear(mainwin);
}

/* Called at @quit time, or if the interpreter hits a fatal error. */
int glkint_close_interface(z_ucs *error_message)
{ 
    if (error_message)
        fatal_error_handler(NULL, error_message, NULL, 0, 0);
    return 0;
}

void glkint_set_buffer_mode(uint8_t UNUSED(new_buffer_mode))
{ }

void glkint_interface_output_z_ucs(z_ucs *z_ucs_output)
{
    /* Conveniently, z_ucs is the same as glui32. */
    glk_put_string_uni(z_ucs_output);
}

int16_t glkint_interface_read_line(zscii *dest, uint16_t maximum_length,
    uint16_t UNUSED(tenth_seconds), uint32_t UNUSED(verification_routine),
    uint8_t UNUSED(preloaded_input), int *UNUSED(tenth_seconds_elapsed),
    bool UNUSED(disable_command_history), bool UNUSED(return_on_escape))
{
    int ix;

    if (!inputbuffer) {
        inputbuffer_size = maximum_length+16;
        inputbuffer = malloc(inputbuffer_size * sizeof(glui32));
    }
    else if (maximum_length > inputbuffer_size) {
        inputbuffer_size = maximum_length+16;
        inputbuffer = realloc(inputbuffer, inputbuffer_size * sizeof(glui32));
    }

    event_t event;
    winid_t win = (instatuswin ? statuswin : mainwin);

    if (win) {
        //### use preloaded_input and the timeout params
        glk_request_line_event_uni(win, inputbuffer, maximum_length, 0);
    }
    while (true) {
        glk_select(&event);
        if (event.type == evtype_LineInput)
            break;
    }

    int count = event.val1;
    for (ix=0; ix<count; ix++) {
        zscii zch = unicode_char_to_zscii_input_char(inputbuffer[ix]);
        dest[ix] = zch;
    }

    return count;
}

int glkint_interface_read_char(uint16_t UNUSED(tenth_seconds),
    uint32_t UNUSED(verification_routine), int *UNUSED(tenth_seconds_elapsed))
{
    event_t event;
    winid_t win = (instatuswin ? statuswin : mainwin);

    if (win) {
        //### use the timeout params
        glk_request_char_event_uni(win);
    }

    while (true) {
        glk_select(&event);
        if (event.type == evtype_CharInput)
            break;
    }

    glui32 ch = event.val1;
    if (ch >= (0x100000000 - keycode_MAXVAL)) {
        switch (ch) {
        case keycode_Left: return 131;
        case keycode_Right: return 132;
        case keycode_Up: return 129;
        case keycode_Down: return 130;
        case keycode_Delete: return 8;
        case keycode_Return: return 13;
        case keycode_Func1: return 133;
        case keycode_Func2: return 134;
        case keycode_Func3: return 135;
        case keycode_Func4: return 136;
        case keycode_Func5: return 137;
        case keycode_Func6: return 138;
        case keycode_Func7: return 139;
        case keycode_Func8: return 140;
        case keycode_Func9: return 141;
        case keycode_Func10: return 142;
        case keycode_Func11: return 143;
        case keycode_Func12: return 144;
        case keycode_Escape: return 27;
        default: return -1;
        }
    }

    return unicode_char_to_zscii_input_char(ch);
}

void glkint_show_status(z_ucs *UNUSED(room_description),
    int UNUSED(status_line_mode), int16_t UNUSED(parameter1),
    int16_t UNUSED(parameter2))
{ }

void glkint_set_text_style(z_style text_style)
{ 
    if (text_style & Z_STYLE_FIXED_PITCH) {
        glk_set_style(style_Preformatted);
    }
    else if (text_style & Z_STYLE_ITALIC) {
        glk_set_style(style_Emphasized);
    }
    else if (text_style & Z_STYLE_BOLD) {
        glk_set_style(style_Subheader);
    }
    else {
        glk_set_style(style_Normal);
    }
}

void glkint_set_colour(z_colour UNUSED(foreground),
    z_colour UNUSED(background), int16_t UNUSED(window))
{ }

void glkint_set_font(z_font font_type)
{ 
    if (instatuswin)
        return;
    if (font_type == Z_FONT_COURIER_FIXED_PITCH)
        glk_set_style(style_Preformatted);
    else
        glk_set_style(style_Normal);
}

void glkint_split_window(int16_t nof_lines)
{ 
    if (!nof_lines) {
        if (statuswin) {
            glk_window_close(statuswin, NULL);
            statuswin = NULL;
        }
    }
    else {
        if (!statuswin) {
            statuswin = glk_window_open(mainwin, winmethod_Above | winmethod_Fixed, nof_lines, wintype_TextGrid, 2);
        }
        else {
            glk_window_set_arrangement(glk_window_get_parent(statuswin), winmethod_Above | winmethod_Fixed, nof_lines, statuswin);
        }
    }
}

/* 1 is the status window; 0 is the story window. */
void glkint_set_window(int16_t window_number)
{ 
    if (!window_number) {
        glk_set_window(mainwin);
        instatuswin = false;
    }
    else {
        if (statuswin) 
            glk_set_window(statuswin);
        else
            glk_set_window(NULL);
        instatuswin = true;
    }    
}

void glkint_erase_window(int16_t window_number)
{
    if (!window_number) {
        glk_window_clear(mainwin);
    }
    else {
        if (statuswin)
            glk_window_clear(statuswin);
    }
}

void glkint_set_cursor(int16_t line, int16_t column,
    int16_t window)
{ 
    if (window && statuswin)
        glk_window_move_cursor(statuswin, column-1, line-1);
}

/* Glk doesn't support this. */
uint16_t glkint_get_cursor_row()
{ return 0; }

/* Glk doesn't support this. */
uint16_t glkint_get_cursor_column()
{ return 0;}

void glkint_erase_line_value(uint16_t UNUSED(start_position))
{ }

void glkint_erase_line_pixels(uint16_t UNUSED(start_position))
{ }

void glkint_output_interface_info()
{
    (void)streams_latin1_output(interface_name);
    (void)streams_latin1_output(" interface version ");
    (void)streams_latin1_output(interface_version);
    (void)streams_latin1_output("\n");
}

void glkint_game_was_restored_and_history_modified()
{ }

void *glkint_request_savegame_fileref(char *filename, bool tosave) 
{
    frefid_t fileref = NULL;
    strid_t str = NULL;

    if (filename) {
        fileref = glk_fileref_create_by_name(
            fileusage_SavedGame|fileusage_BinaryMode, 
            filename, 0);
    }
    else {
        fileref = glk_fileref_create_by_prompt(
            fileusage_SavedGame|fileusage_BinaryMode, 
            (tosave ? filemode_Write : filemode_Read), 0);
    }

    if (!fileref)
        return NULL;

    str = glk_stream_open_file(fileref, (tosave ? filemode_Write : filemode_Read), 0);
    /* Dispose of the fileref, whether the stream opened successfully or not. */
    glk_fileref_destroy(fileref);

    return str;
}

int glkint_closefile(void *file)
{
    glk_stream_close(file, NULL);
    return 0;
}
int glkint_getchar(void *file)
{
    int ch = glk_get_char_stream(file);
    if (ch < 0)
        return EOF;
    return ch;
}
size_t glkint_getchars(char *ptr, size_t len, void *file)
{
    return glk_get_buffer_stream(file, ptr, len);
}
int glkint_putchar(int ch, void *file)
{
    glk_put_char_stream(file, ch);
    return ch;
}
size_t glkint_putchars(char *ptr, size_t len, void *file)
{
    glk_put_buffer_stream(file, ptr, len);
    return len;
}
off_t glkint_getfilepos(void *file)
{
    return glk_stream_get_position(file);
}
int glkint_setfilepos(void *file, off_t seek, int whence)
{
    if (whence == SEEK_SET)
        glk_stream_set_position(file, seek, seekmode_Start);
    else if (whence == SEEK_CUR)
        glk_stream_set_position(file, seek, seekmode_Current);
    else if (whence == SEEK_END)
        glk_stream_set_position(file, seek, seekmode_End);
    return 0;
}

/* The two top-level data structures. These are installed by code
   in unixstrt.c.
*/

struct z_screen_interface glkint_screen_interface =
{
    &glkint_get_interface_name,
    &glkint_return_true, /* is_status_line_available */
    &glkint_return_true, /* is_split_screen_available */
    &glkint_return_true, /* is_variable_pitch_font_default */
    &glkint_return_false, /* is_colour_available */
    &glkint_return_false, /* is_picture_displaying_available */
    &glkint_return_true, /* is_bold_face_available */
    &glkint_return_true, /* is_italic_available */
    &glkint_return_true, /* is_fixed_space_font_available */
    &glkint_return_true, /* is_timed_keyboard_input_available */
    &glkint_return_true, /* is_preloaded_input_available */
    &glkint_return_false, /* is_character_graphics_font_availiable */
    &glkint_return_false, /* is_picture_font_availiable */
    &glkint_get_screen_height,
    &glkint_get_screen_width,
    &glkint_get_screen_height,
    &glkint_get_screen_width,
    &glkint_return_1, /* get_font_width_in_units */
    &glkint_return_1, /* get_font_height_in_units */
    &glkint_get_default_foreground_colour,
    &glkint_get_default_background_colour,
    &glkint_return_0, /* get_total_width_in_pixels_of_text_sent_to_output_stream_3 */
    &glkint_parse_config_parameter,
    &glkint_link_interface_to_story,
    &glkint_reset_interface,
    &glkint_close_interface,
    &glkint_set_buffer_mode,
    &glkint_interface_output_z_ucs,
    &glkint_interface_read_line,
    &glkint_interface_read_char,
    &glkint_show_status,
    &glkint_set_text_style,
    &glkint_set_colour,
    &glkint_set_font,
    &glkint_split_window,
    &glkint_set_window,
    &glkint_erase_window,
    &glkint_set_cursor,
    &glkint_get_cursor_row,
    &glkint_get_cursor_column,
    &glkint_erase_line_value,
    &glkint_erase_line_pixels,
    &glkint_output_interface_info,
    &glkint_return_false, /* input_must_be_repeated_by_story */
    &glkint_game_was_restored_and_history_modified
};

struct z_filesys_interface glkint_filesys_interface =
{
    &glkint_request_savegame_fileref,
    &glkint_closefile,
    &glkint_getchar,
    &glkint_getchars,
    &glkint_putchar,
    &glkint_putchars,
    &glkint_getfilepos,
    &glkint_setfilepos,
};
