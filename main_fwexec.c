/**
 * Main program code for the runc utility.
 *
 * Copyright 2006 Lawrie Griffiths <lawrie.griffiths@ntlworld.com>
 * Copyright 2007 David Anderson <dave@natulte.net>
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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "error.h"
#include "lowlevel.h"
#include "samba.h"

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

static void
fwexec(const char *filename, long load_addr, long jump_addr)
{
  nxt_t *nxt;
  nxt_error_t err;
  uint8_t *firmware;
  int firmware_len;

  NXT_HANDLE_ERR(nxt_init(&nxt), NULL, "Error during library initialization");

  get_firmware(&firmware, &firmware_len, filename);

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
         "Uploading firmware...\n");

  // Send the C program
  NXT_HANDLE_ERR(nxt_send_file(nxt, load_addr, firmware, firmware_len), nxt,
                 "Error Sending file");

  printf("Firmware uploaded, executing...\n");
  NXT_HANDLE_ERR(nxt_jump(nxt, jump_addr), nxt, "Error jumping to C program");
  printf("Firmware started.\n");

  nxt_close(nxt);
  nxt_exit(nxt);
}

static void
usage(const char *progname, int exit_code)
{
  fprintf(
      exit_code ? stderr : stdout,
      "Usage: %s <firmware image to write> [load address [jump address]]\n"
      "       %s (-l|-h)\n"
      "Upload firmware image to a connected NXT device and run it from RAM.\n"
      "\n"
      "Options:\n" COMMON_OPTIONS "\n"
      "Example:\n"
      "  %s -l\n"
      "       print detected NXT bricks\n"
      "  %s beep.bin\n"
      "       locate a NXT brick and run beep.bin file\n"
      "  %s beep.bin 0x202000\n"
      "       locate a NXT brick and run beep.bin file at address 0x202000\n",
      progname, progname, progname, progname, progname);
  exit(exit_code);
}

static long
get_addr(const char *progname, const char *s)
{
  char *end;
  long r;

  errno = 0;
  r = strtol(s, &end, 16);
  if (end == s || *end != '\0' || errno)
    {
      fprintf(stderr, "Expect a hexadecimal address.\n");
      usage(progname, 1);
    }
  return r;
}

int
main(int argc, char *const *argv)
{
  common_options_t common_options = { 0 };
  const char *filename = NULL;
  long load_addr;
  long jump_addr;
  int c;

  while ((c = common_getopt(argc, argv, NULL, &common_options, usage)) != -1)
    {
      usage(argv[0], 1);
    }
  if (optind == argc)
    usage(argv[0], 1);
  filename = argv[optind++];
  load_addr = 0x202000;
  if (optind < argc)
    load_addr = get_addr(argv[0], argv[optind++]);
  jump_addr = load_addr;
  if (optind < argc)
    jump_addr = get_addr(argv[0], argv[optind++]);
  if (optind < argc)
    usage(argv[0], 1);

  fwexec(filename, load_addr, jump_addr);

  return 0;
}
