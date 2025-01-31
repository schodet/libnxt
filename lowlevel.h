/**
 * NXT bootstrap interface; low-level USB functions.
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

#ifndef __LOWLEVEL_H__
#define __LOWLEVEL_H__

#include <stdint.h>

#include "error.h"

typedef struct nxt_t nxt_t;

typedef enum
{
  SAMBA = 0, /* SAM7 Boot Assistant    */
  LEGO,      /* Official LEGO firmware */
  NXTOS,
  N_FIRMWARES,
} nxt_firmware;

nxt_error_t nxt_init(nxt_t **nxt);
void nxt_exit(nxt_t *nxt);
nxt_error_t nxt_find(nxt_t *nxt);
nxt_error_t nxt_open(nxt_t *nxt);
void nxt_close(nxt_t *nxt);
int nxt_is_firmware(nxt_t *nxt, nxt_firmware fw);
nxt_error_t nxt_send_buf(nxt_t *nxt, const uint8_t *buf, int len);
nxt_error_t nxt_send_str(nxt_t *nxt, const char *str);
nxt_error_t nxt_recv_buf(nxt_t *nxt, uint8_t *buf, int len);

#endif /* __LOWLEVEL_H__ */
