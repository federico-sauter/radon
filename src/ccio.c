/* Copyright(C) 2024 by Federico Sauter<federicosauter@pm.me>. All rights reserved. */

#include <arch/ia32/video.h>
#include <radon.h>
#include <string.h>
#include <sv/svsched.h>
#include <sys/ccio.h>

typedef char* va_list;

#define _va_sizeof(t) (((sizeof(t) + sizeof(unsigned) - 1) / sizeof(unsigned)) * sizeof(unsigned))

#define va_start(va, last_arg) va = ((char*)&(last_arg)) + _va_sizeof(last_arg)

#define va_arg(va, t) (va += _va_sizeof(t), *((t*)(va - _va_sizeof(t))))

#define va_end(va)

#define NEWLINE '\n'

/*  current coordinates in screen ZERO-based */
static unsigned short _curr_x = 0;
static unsigned short _curr_y = 0;
static unsigned int   _color  = 0 | (LIGHT_GREY & 0x0f);

/*  keyboard buffer */
#define KEYBD_BUF_LEN 512
static char             keybd_buf[KEYBD_BUF_LEN];
static unsigned short   keybd_buf_len   = 0;
volatile unsigned short keybd_last_key  = 0;
volatile unsigned char  keybd_last_scan = 0;
#define is_key_pressed(x) (!(x & 0x80))

/*  utilities   */
void cc_print_hexdigit(unsigned char d); /*  print out: hex number digit */
void cc_printb(unsigned char d);         /*  print out: byte as number               */
void cc_printhw(unsigned short d);       /*  print out: halfword as hex number */

void cc_clear()
{
    ia32_video_clear(_color);
}

void cc_cpos_get(unsigned short* x, unsigned short* y)
{
    *x = _curr_x;
    *y = _curr_y;
}

void cc_cpos_set(unsigned short x, unsigned short y)
{
    _curr_x = (x > ia32_video_get_line_len() - 1) ? ia32_video_get_line_len() - 1 : x;
    _curr_y = (y > ia32_video_get_screen_len() - 1) ? ia32_video_get_screen_len() - 1 : y;

    ia32_video_move_ppos(_curr_x, _curr_y);
}

int cc_printf(const char* format, ...)
{
    int      b = 0;
    unsigned i, j;
    unsigned uintArg;
    va_list  vargs;
    char*    strArg;

    va_start(vargs, format);

    for (b = 0; *format != 0; ++format, ++b)
    {
        if (*format == '%')
        {
            ++format;
            if (*format == 'x')
            {
                /* unsigned hex number */
                cc_printch('0');
                cc_printch('x');
                ++b;
                uintArg = va_arg(vargs, unsigned);

                for (i = sizeof(unsigned); i > 0; --i, b += 2)
                {
                    j = (i - 1) * 8;
                    cc_print_hexdigit((uintArg >> (j + 4)) & 0xF);
                    cc_print_hexdigit((uintArg >> j) & 0xF);
                }
            }
            else if (*format == 'c')
            {
                /* character */
                cc_printch(va_arg(vargs, char));
            }
            else if (*format == 's')
            {
                /* string */
                strArg = va_arg(vargs, char*);
                while (*strArg)
                {
                    cc_printch(*strArg++);
                    ++b;
                }
            }
            /* skip unrecognized sequences */
        }
        else
            cc_printch(*format);
    }

    va_end(vargs);
    return b;
}

void cc_printch(char c)
{
    /*  olverflow to newline or scroll as necessary */
    if (++_curr_x == ia32_video_get_line_len() || c == NEWLINE)
    {
        /* last char on line, overflow to next line */
        _curr_x = 0;
        if (++_curr_y == ia32_video_get_screen_len())
        {
            ia32_video_scroll(_color);
            --_curr_y;
        }
        else
            ia32_video_move_ppos(_curr_x, _curr_y);
    }

    if (c != NEWLINE)
        ia32_video_printch(c, _color);

    /*  TODO fix bug: what happens when it reaches out of screen? scroll! */
    ia32_video_upd_cursor(_curr_x, _curr_y);
}

void cc_print_abs(unsigned short x, unsigned short y, const char* s)
{
    ia32_video_move_ppos(x, y);
    while (*s != 0)
        ia32_video_printch(*s++, _color);
    ia32_video_move_ppos(_curr_x, _curr_y);
}

void cc_keybd_addch(unsigned short c)
{
    keybd_last_key = c;
    if (keybd_last_key != CC_KEY_LF)
    {
        keybd_buf_len = (keybd_buf_len == KEYBD_BUF_LEN - 1) ? KEYBD_BUF_LEN - 2 : keybd_buf_len;
        keybd_buf[keybd_buf_len++] = c;
    }
    keybd_buf[keybd_buf_len] = 0;
    cc_printch(c);
}

void cc_keybd_dobs()
{
    keybd_last_key = CC_KEY_BS;
    if (keybd_buf_len > 0)
    {
        keybd_buf[--keybd_buf_len] = 0;
        ia32_video_move_ppos(--_curr_x, _curr_y);
        cc_printf(" ");
        ia32_video_move_ppos(--_curr_x, _curr_y);
    }
}

void cc_print_hexdigit(unsigned char d)
{
    d = (d > 15) ? 15 : d;
    if (d < 10)
        cc_printch(d + 0x30);
    else
        cc_printch(d + 0x37);
}

void cc_print_decdigit(unsigned char d)
{
    d = (d > 9) ? 9 : d;
    cc_printch(d + 0x30);
}

void cc_color_set(unsigned char fg, unsigned char bg)
{
    _color = ia32_video_getcolor(fg, bg);
}
