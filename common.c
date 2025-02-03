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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

int
handle_error(nxt_t *nxt, const char *msg, nxt_error_t err)
{
  fprintf(stderr, "%s: %s\n", msg, nxt_str_error(err));
  if (nxt != NULL)
    {
      nxt_close(nxt);
      nxt_exit(nxt);
    }
  exit(err);
}

static void
list_cb(void *user, const char *connection, nxt_firmware fw, const char *serial,
        const char *name)
{
  bool *seen = user;
  const char *fws[] = { "SAMBA", "LEGO", "NXTOS" };

  if (!*seen)
    {
      printf("%-11s  %-8s  %-17s  %s\n", "Connection", "Firmware",
             "Serial number", "Brick name");
      *seen = true;
    }
  printf("%-11s  %-8s  %-17s  %s\n", connection, fws[fw], serial ? serial : "-",
         name ? name : "-");
}

static void
common_list()
{
  nxt_t *nxt;
  bool seen = false;

  NXT_HANDLE_ERR(nxt_init(&nxt), NULL, "Error during library initialization");

  NXT_HANDLE_ERR(nxt_list(nxt, list_cb, &seen), nxt,
                 "Error while scanning for bricks");
  if (!seen)
    fputs("No brick found\n", stdout);

  nxt_exit(nxt);
}

int
common_getopt(int argc, char *const *argv, const char *optstring,
              common_options_t *common_options,
              void (*usage)(const char *, int))
{
  int c;

  while ((c = getopt(argc, argv, optstring ? optstring : COMMON_OPTSTRING)) !=
         -1)
    {
      switch (c)
        {
        case 'l':
          common_options->list = true;
          break;
        case 'h':
          usage(argv[0], 0);
          break;
        default:
          return c;
        }
    }
  if (common_options->list)
    {
      if (optind != argc)
        usage(argv[0], 1);
      common_list();
      exit(0);
    }
  return -1;
}
