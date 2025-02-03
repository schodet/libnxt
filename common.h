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

#define COMMON_OPTSTRING "lh"
#define COMMON_OPTIONS             \
  "  -l   list detected devices\n" \
  "  -h   print this help message\n"

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
} common_options_t;

int handle_error(nxt_t *nxt, const char *msg, nxt_error_t err);
int common_getopt(int argc, char *const *argv, const char *optstring,
                  common_options_t *common_options,
                  void (*usage)(const char *, int));

#endif /* __COMMON_H__ */
