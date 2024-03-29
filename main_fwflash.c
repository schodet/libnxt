/**
 * Main program code for the fwflash utility.
 *
 * Copyright 2006 David Anderson <dave@natulte.net>
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

#include "error.h"
#include "firmware.h"
#include "lowlevel.h"
#include "samba.h"

#define NXT_HANDLE_ERR(expr, nxt, msg)                \
  do                                                  \
    {                                                 \
      nxt_error_t nxt__err_temp = (expr);             \
      if (nxt__err_temp)                              \
        return handle_error(nxt, msg, nxt__err_temp); \
    }                                                 \
  while (0)

static int
handle_error(nxt_t *nxt, const char *msg, nxt_error_t err)
{
  printf("%s: %s\n", msg, nxt_str_error(err));
  if (nxt != NULL)
    nxt_close(nxt);
  exit(err);
}

int
main(int argc, const char *argv[])
{
  nxt_t *nxt;
  nxt_error_t err;
  const char *fw_file;

  if (argc != 2)
    {
      printf("Syntax: %s <firmware image to write>\n"
             "\n"
             "Example: %s nxtos.bin\n",
             argv[0], argv[0]);
      exit(1);
    }

  fw_file = argv[1];

  printf("Checking firmware... ");
  NXT_HANDLE_ERR(nxt_firmware_validate(fw_file), NULL, "Error");
  printf("OK.\n");

  NXT_HANDLE_ERR(nxt_init(&nxt), NULL, "Error during library initialization");

  err = nxt_find(nxt);
  if (err)
    {
      if (err == NXT_NOT_PRESENT)
        printf("NXT not found. Is it properly plugged in via USB?\n");
      else
        NXT_HANDLE_ERR(err, nxt, "Error while scanning for NXT");
      exit(1);
    }

  if (!nxt_is_firmware(nxt, SAMBA))
    {
      printf("NXT found, but not running in reset mode.\n");
      printf("Please reset your NXT manually and restart this program.\n");
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
  return 0;
}
