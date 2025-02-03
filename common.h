/**
 * Common code for programs.
 *
 * Copyright 2006 David Anderson <dave@natulte.net>
 * Copyright 2025 Nicolas Schodet
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>

#include "error.h"
#include "lowlevel.h"

#define COMMON_OPTSTRING "lyhbs:n:"
#define COMMON_OPTIONS                                                     \
  "  -l         list detected devices\n"                                   \
  "  -y         allow reset to bootloader mode (erase device memory)\n"    \
  "  -h         print this help message\n"                                 \
  "Device selection option:\n"                                             \
  "  -b         select device in bootloader mode only\n"                   \
  "Device selection options (not in bootloader mode):\n"                   \
  "  -s SERIAL  select device with this serial (e.g. 00:16:53:01:02:03)\n" \
  "  -n NAME    select device with this name (e.g. NXT)\n"

#define NXT_HANDLE_ERR(expr, nxt, msg)         \
  do                                           \
    {                                          \
      nxt_error_t nxt__err_temp = (expr);      \
      if (nxt__err_temp)                       \
        handle_error(nxt, msg, nxt__err_temp); \
    }                                          \
  while (0)

typedef struct
{
  bool list;
  bool yes;
  bool bootloader_mode_only;
  const char *match_serial;
  const char *match_name;
} common_options_t;

int handle_error(nxt_t *nxt, const char *msg, nxt_error_t err);
int common_getopt(int argc, char *const *argv, const char *optstring,
                  common_options_t *common_options,
                  void (*usage)(const char *, int));
void common_find_bootloader(nxt_t *nxt, const common_options_t *common_options);

#endif /* __COMMON_H__ */
