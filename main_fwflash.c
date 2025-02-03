/**
 * Main program code for the fwflash utility.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "error.h"
#include "firmware.h"
#include "lowlevel.h"
#include "samba.h"

static void
fwflash(const char *fw_file)
{
  nxt_t *nxt;
  nxt_error_t err;

  NXT_HANDLE_ERR(nxt_init(&nxt), NULL, "Error during library initialization");

  printf("Checking firmware... ");
  NXT_HANDLE_ERR(nxt_firmware_validate(fw_file), NULL, "Error");
  printf("OK.\n");

  err = nxt_find(nxt);
  if (err)
    {
      if (err == NXT_NOT_PRESENT)
        fprintf(stderr, "NXT not found. Is it properly plugged in via USB?\n");
      else
        NXT_HANDLE_ERR(err, nxt, "Error while scanning for NXT");
      exit(1);
    }

  if (!nxt_is_firmware(nxt, SAMBA))
    {
      fprintf(stderr, "NXT found, but not running in reset mode.\n");
      fprintf(stderr,
              "Please reset your NXT manually and restart this program.\n");
      exit(2);
    }

  NXT_HANDLE_ERR(nxt_open(nxt), nxt, "Error while connecting to NXT");
  NXT_HANDLE_ERR(nxt_handshake(nxt), nxt, "Error during initial handshake");

  printf("NXT device in reset mode located and opened.\n"
         "Starting firmware flash procedure now...\n");

  NXT_HANDLE_ERR(nxt_firmware_flash(nxt, fw_file), nxt,
                 "Error flashing firmware");
  printf("Firmware flash complete.\n");
  NXT_HANDLE_ERR(nxt_jump(nxt, 0x00100000), nxt, "Error booting new firmware");
  printf("New firmware started!\n");

  nxt_close(nxt);
  nxt_exit(nxt);
}

static void
usage(const char *progname, int exit_code)
{
  fprintf(exit_code ? stderr : stdout,
          "Usage: %s <firmware image to write>\n"
          "       %s (-l|-h)\n"
          "Flash firmware image to a connected NXT device.\n"
          "\n"
          "Options:\n" COMMON_OPTIONS "\n"
          "Example:\n"
          "  %s -l\n"
          "       print detected NXT bricks\n"
          "  %s nxt_firmware.bin\n"
          "       locate a NXT brick and flash nxt_firmware.bin file\n",
          progname, progname, progname, progname);
  exit(exit_code);
}

int
main(int argc, char *const *argv)
{
  common_options_t common_options = { 0 };
  const char *fw_file = NULL;
  int c;

  while ((c = common_getopt(argc, argv, NULL, &common_options, usage)) != -1)
    {
      usage(argv[0], 1);
    }
  if (optind + 1 != argc)
    usage(argv[0], 1);
  fw_file = argv[optind];

  fwflash(fw_file);

  return 0;
}
