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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmd.h"
#include "common.h"
#include "lowlevel.h"

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
common_list(void)
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
        case 'y':
          common_options->yes = true;
          break;
        case 'h':
          usage(argv[0], 0);
          break;
        case 'b':
          common_options->bootloader_mode_only = true;
          break;
        case 's':
          common_options->match_serial = optarg;
          break;
        case 'n':
          common_options->match_name = optarg;
          break;
        default:
          return c;
        }
    }
  if (common_options->list)
    {
      if (optind != argc)
        {
          fprintf(stderr, "Too many arguments with -l option\n");
          usage(argv[0], 1);
        }
      common_list();
      exit(0);
    }
  return -1;
}

static nxt_error_t
common_reset_bootloader(nxt_t *nxt, bool yes)
{
  char rep[80];
  nxt_error_t err;

  if (!yes)
    {
      nxt_device_info_t device_info;

      if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
        {
          fprintf(stderr, "No TTY, can not ask for permission to continue.\n");
          return NXT_NOT_PRESENT;
        }

      NXT_HANDLE_ERR(nxt_open(nxt), nxt, "Error while connecting to NXT");
      NXT_HANDLE_ERR(nxt_cmd_get_device_info(nxt, &device_info), nxt,
                     "Error while getting device information");

      printf("Found brick:\n Serial: %s\n Name: %s\n", device_info.serial,
             device_info.name);
      printf("Reset to bootloader (this erases the brick memory)?\n"
             "Please type RESET and press enter to continue:\n");
      fgets(rep, sizeof(rep), stdin);
      if (strcmp(rep, "RESET\n") != 0)
        {
          printf("Not resetting, bye!\n");
          return NXT_NOT_PRESENT;
        }
    }

  printf("Resetting brick, it should make a clicking noise...\n");
  NXT_HANDLE_ERR(nxt_cmd_boot(nxt, true), nxt, "Error while resetting");
  nxt_close(nxt);

  for (int i = 0; i < 20; i++)
    {
      putchar('.');
      fflush(stdout);
      sleep(1);
      err = nxt_find(nxt, SAMBA, NULL, NULL);
      if (err != NXT_NOT_PRESENT)
        NXT_HANDLE_ERR(err, nxt, "Error while scanning for NXT bootloader");
      if (!err)
        break;
    }
  putchar('\n');

  return err;
}

void
common_find_bootloader(nxt_t *nxt, const common_options_t *common_options)
{
  nxt_error_t err;

  err = nxt_find(nxt, SAMBA, NULL, NULL);
  if (err != NXT_NOT_PRESENT)
    NXT_HANDLE_ERR(err, nxt, "Error while scanning for NXT bootloader");

  if (err == NXT_NOT_PRESENT && !common_options->bootloader_mode_only)
    {
      // Try to find a device not in bootloader mode.
      err = nxt_find(nxt, LEGO, common_options->match_serial,
                     common_options->match_name);
      if (err != NXT_NOT_PRESENT)
        NXT_HANDLE_ERR(err, nxt, "Error while scanning for NXT");
      if (!err)
        err = common_reset_bootloader(nxt, common_options->yes);
    }

  if (err == NXT_NOT_PRESENT)
    {
      fprintf(stderr, "NXT not found. Is it properly plugged in via USB?\n");
      exit(1);
    }

  assert(err == NXT_OK);
}
