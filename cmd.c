/**
 * NXT interface; commands code.
 *
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

#include "cmd.h"
#include "lowlevel.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>

static nxt_error_t
parse_reply(uint8_t **p, nxt_cmd_opcode_t opcode)
{
  if (**p != NXT_CMD_TYPE_REPLY)
    return NXT_ERROR_PROTO;
  (*p)++;
  if (**p != opcode)
    return NXT_ERROR_PROTO;
  (*p)++;
  if (**p != NXT_CMD_STATUS_NO_ERR)
    return NXT_ERROR_CMD(**p);
  (*p)++;
  return NXT_OK;
}

static nxt_error_t
parse_string(uint8_t **p, char *s, size_t size)
{
  uint8_t *pp;
  pp = *p;
  *p += size;
  for (; pp != *p; pp++)
    {
      *s++ = *pp;
      if (!*pp)
        return NXT_OK;
      if (!isprint(*pp))
        return NXT_ERROR_PROTO;
    }
  return NXT_ERROR_PROTO;
}

static nxt_error_t
parse_serial(uint8_t **p, char *s, size_t s_size)
{
  int ret = snprintf(s, s_size, "%02x:%02x:%02x:%02x:%02x:%02x", (*p)[0],
                     (*p)[1], (*p)[2], (*p)[3], (*p)[4], (*p)[5]);
  assert(ret < (int)s_size);
  // Last one is always 0.
  *p += 7;
  return NXT_OK;
}

const char *
nxt_cmd_str_error(nxt_cmd_status_t status)
{
  switch (status)
    {
    case NXT_CMD_STATUS_NO_ERR:
      return "Command status: no error";
    case NXT_CMD_STATUS_STAT_COMM_PENDING:
      return "Command status: pending setup operation in progress";
    case NXT_CMD_STATUS_STAT_MSG_EMPTY_MAILBOX:
      return "Command status: specified mailbox contains no new messages";
    case NXT_CMD_STATUS_NOMOREHANDLES:
      return "Command status: no more handles";
    case NXT_CMD_STATUS_NOSPACE:
      return "Command status: no space";
    case NXT_CMD_STATUS_NOMOREFILES:
      return "Command status: no more files";
    case NXT_CMD_STATUS_EOFEXSPECTED:
      return "Command status: end of file expected";
    case NXT_CMD_STATUS_ENDOFFILE:
      return "Command status: end of file";
    case NXT_CMD_STATUS_NOTLINEARFILE:
      return "Command status: not a linear file";
    case NXT_CMD_STATUS_FILENOTFOUND:
      return "Command status: file not found";
    case NXT_CMD_STATUS_HANDLEALREADYCLOSED:
      return "Command status: handle already closed";
    case NXT_CMD_STATUS_NOLINEARSPACE:
      return "Command status: no linear space available";
    case NXT_CMD_STATUS_UNDEFINEDERROR:
      return "Command status: undefined error";
    case NXT_CMD_STATUS_FILEISBUSY:
      return "Command status: file is busy";
    case NXT_CMD_STATUS_NOWRITEBUFFERS:
      return "Command status: no write buffers";
    case NXT_CMD_STATUS_APPENDNOTPOSSIBLE:
      return "Command status: append not possible";
    case NXT_CMD_STATUS_FILEISFULL:
      return "Command status: file is full";
    case NXT_CMD_STATUS_FILEEXISTS:
      return "Command status: file exists";
    case NXT_CMD_STATUS_MODULENOTFOUND:
      return "Command status: module not found";
    case NXT_CMD_STATUS_OUTOFBOUNDERY:
      return "Command status: out of boundary";
    case NXT_CMD_STATUS_ILLEGALFILENAME:
      return "Command status: illegal file name";
    case NXT_CMD_STATUS_ILLEGALHANDLE:
      return "Command status: illegal handle";
    case NXT_CMD_STATUS_ERR_RC_FAILED:
      return "Command status: request failed (i.e. specified file not found)";
    case NXT_CMD_STATUS_ERR_RC_UNKNOWN_CMD:
      return "Command status: unknown command opcode";
    case NXT_CMD_STATUS_ERR_RC_BAD_PACKET:
      return "Command status: clearly insane packet";
    case NXT_CMD_STATUS_ERR_RC_ILLEGAL_VAL:
      return "Command status: data contains out-of-range values";
    case NXT_CMD_STATUS_ERR_COMM_BUS_ERR:
      return "Command status: something went wrong on the communications bus";
    case NXT_CMD_STATUS_ERR_COMM_BUFFER_FULL:
      return "Command status: no room in comm buffer";
    case NXT_CMD_STATUS_ERR_COMM_CHAN_INVALID:
      return "Command status: specified channel/connection is not valid";
    case NXT_CMD_STATUS_ERR_COMM_CHAN_NOT_READY:
      return "Command status: specified channel/connection not configured or "
             "busy";
    case NXT_CMD_STATUS_ERR_NO_PROG:
      return "Command status: no active program";
    case NXT_CMD_STATUS_ERR_INVALID_SIZE:
      return "Command status: illegal size specified";
    case NXT_CMD_STATUS_ERR_INVALID_QUEUE:
      return "Command status: illegal queue ID specified";
    case NXT_CMD_STATUS_ERR_INVALID_FIELD:
      return "Command status: attempted to access invalid field of a structure";
    case NXT_CMD_STATUS_ERR_INVALID_PORT:
      return "Command status: bad input or output port specified";
    case NXT_CMD_STATUS_ERR_MEM:
      return "Command status: insufficient memory available";
    case NXT_CMD_STATUS_ERR_ARG:
      return "Command status: bad arguments";
    default:
      return "Command status: unknown error";
    }
}

nxt_error_t
nxt_cmd_get_device_info(nxt_t *nxt, nxt_device_info_t *device_info)
{
  static const uint8_t cmd[] = { NXT_CMD_TYPE_SYSTEM,
                                 NXT_CMD_OPCODE_SYSTEM_DEVICEINFO };
  uint8_t rsp[3 + NXT_NAME_SIZE + 7 + 4 + 4], *p;

  NXT_ERR(nxt_send_buf(nxt, cmd, sizeof(cmd)));
  NXT_ERR(nxt_recv_buf(nxt, rsp, sizeof(rsp)));

  p = rsp;
  NXT_ERR(parse_reply(&p, NXT_CMD_OPCODE_SYSTEM_DEVICEINFO));
  NXT_ERR(parse_string(&p, device_info->name, sizeof(device_info->name)));
  NXT_ERR(parse_serial(&p, device_info->serial, sizeof(device_info->serial)));
  device_info->signal_strengths[0] = *p++;
  device_info->signal_strengths[1] = *p++;
  device_info->signal_strengths[2] = *p++;
  device_info->signal_strengths[3] = *p++;
  device_info->user_flash = p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
  p += 4;
  assert(p == rsp + sizeof(rsp));

  return NXT_OK;
}
