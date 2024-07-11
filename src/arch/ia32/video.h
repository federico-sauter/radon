/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#if !defined(_RADON_SYS_VIDEO__H_)
#define _RADON_SYS_VIDEO__H_

/*  screen size */
#define ia32_video_get_line_len()   80
#define ia32_video_get_screen_len() 25

/*  clears the screen   */
void ia32_video_clear(unsigned char bg);

/*  scrolls one line up the screen; moves the current position of the buffer to
   the beginning of the new line at the bottom of the screen and clears the
   newline in the specified color */
void ia32_video_scroll(unsigned char bg);

/*  moves the current position in video memory pointer to the specified
   character position, affecting the position for the next write operation. */
void ia32_video_move_ppos(unsigned short x, unsigned short y);

/*  updates the cursor position */
void ia32_video_upd_cursor(unsigned short x, unsigned short y);

/*  combines the foreground and background colors into an int for the write
 * calls   */
unsigned int ia32_video_getcolor(unsigned char fg, unsigned char bg);

/*  writes the specified character to the current position of the screen buffer
   and updates the screen buffer position and cursor position  */
void ia32_video_printch(unsigned char c, int color);

#endif
