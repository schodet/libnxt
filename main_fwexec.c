/**
 * Main program code for the runc utility.
 *
 * Copyright 2006 Lawrie Griffiths <lawrie.griffiths@ntlworld.com>
 *           2007 David Anderson <dave@natulte.net>
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

#define NXT_HANDLE_ERR(expr, nxt, msg)         \
  do                                           \
    {                                          \
      nxt_error_t nxt__err_temp = (expr);      \
      if (nxt__err_temp)                       \
        handle_error(nxt, msg, nxt__err_temp); \
    }                                          \
  while (0)

static int
handle_error(nxt_t *nxt, const char *msg, nxt_error_t err)
{
  printf("%s: %s\n", msg, nxt_str_error(err));
  if (nxt != NULL)
    nxt_close(nxt);
  exit(err);
}

void
get_firmware(uint8_t **firmware, int *len, const char *filename)
{
  FILE *f;

  f = fopen(filename, "rb");
  if (f == NULL)
    NXT_HANDLE_ERR(NXT_FILE_ERROR, NULL, "Error opening file");

  fseek(f, 0, SEEK_END);
  *len = ftell(f);
  rewind(f);

  if (*len > 56 * 1024)
    NXT_HANDLE_ERR(NXT_INVALID_FIRMWARE, NULL,
                   "Firmware image is too big to fit in RAM.");

  *firmware = malloc(*len);
  if (*firmware == NULL)
    NXT_HANDLE_ERR(NXT_FILE_ERROR, NULL, "Error allocating memory");

  if (fread(*firmware, 1, *len, f) != (size_t)*len)
    NXT_HANDLE_ERR(NXT_FILE_ERROR, NULL, "Error reading file");

  printf("Firmware size is %d bytes\n", *len);

  fclose(f);
}

int
main(int argc, char *argv[])
{
  nxt_t *nxt;
  nxt_error_t err;
  uint8_t *firmware;
  int firmware_len;
  long load_addr;
  long jump_addr;

  if (argc < 2 || argc > 4)
    {
      printf("Syntax: %s <Firmware image to write> [load address] "
             "[jump address]\n"
             "\n"
             "Example: %s beep.bin\n"
             "         %s beep.bin 0x202000\n",
             argv[0], argv[0], argv[0]);
      exit(1);
    }
  if (argc >= 3)
    load_addr = strtol(argv[2], NULL, 16);
  else
    load_addr = 0x202000;
  if (argc == 4)
    jump_addr = strtol(argv[3], NULL, 16);
  else
    jump_addr = load_addr;

  get_firmware(&firmware, &firmware_len, argv[1]);

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
         "Uploading firmware...\n");

  // Send the C program
  NXT_HANDLE_ERR(nxt_send_file(nxt, load_addr, firmware, firmware_len), nxt,
                 "Error Sending file");

  printf("Firmware uploaded, executing...\n");
  NXT_HANDLE_ERR(nxt_jump(nxt, jump_addr), nxt, "Error jumping to C program");

  nxt_close(nxt);

  printf("Firmware started.\n");

  return 0;
}
