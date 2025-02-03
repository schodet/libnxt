/**
 * NXT interface; brick commands.
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

#ifndef __CMD_H__
#define __CMD_H__

#include <stdbool.h>
#include <stdint.h>

#include "error.h"
#include "lowlevel.h"

#define NXT_NAME_SIZE 15

/*
 * Using the same names as in the NXT firmware code, even if not consistent.
 */
typedef enum
{
  NXT_CMD_OPCODE_DIRECT_START_PROGRAM = 0x00,
  NXT_CMD_OPCODE_DIRECT_STOP_PROGRAM = 0x01,
  NXT_CMD_OPCODE_DIRECT_PLAY_SOUND_FILE = 0x02,
  NXT_CMD_OPCODE_DIRECT_PLAY_TONE = 0x03,
  NXT_CMD_OPCODE_DIRECT_SET_OUT_STATE = 0x04,
  NXT_CMD_OPCODE_DIRECT_SET_IN_MODE = 0x05,
  NXT_CMD_OPCODE_DIRECT_GET_OUT_STATE = 0x06,
  NXT_CMD_OPCODE_DIRECT_GET_IN_VALS = 0x07,
  NXT_CMD_OPCODE_DIRECT_RESET_IN_VAL = 0x08,
  NXT_CMD_OPCODE_DIRECT_MESSAGE_WRITE = 0x09,
  NXT_CMD_OPCODE_DIRECT_RESET_POSITION = 0x0a,
  NXT_CMD_OPCODE_DIRECT_GET_BATT_LVL = 0x0b,
  NXT_CMD_OPCODE_DIRECT_STOP_SOUND = 0x0c,
  NXT_CMD_OPCODE_DIRECT_KEEP_ALIVE = 0x0d,
  NXT_CMD_OPCODE_DIRECT_LS_GET_STATUS = 0x0e,
  NXT_CMD_OPCODE_DIRECT_LS_WRITE = 0x0f,
  NXT_CMD_OPCODE_DIRECT_LS_READ = 0x10,
  NXT_CMD_OPCODE_DIRECT_GET_CURR_PROGRAM = 0x11,
  NXT_CMD_OPCODE_DIRECT_GET_BUTTON_STATE = 0x12,
  NXT_CMD_OPCODE_DIRECT_MESSAGE_READ = 0x13,
  NXT_CMD_OPCODE_DIRECT_DATALOG_READ = 0x19,
  NXT_CMD_OPCODE_DIRECT_DATALOG_SET_TIMES = 0x1a,
  NXT_CMD_OPCODE_DIRECT_BT_GET_CONTACT_COUNT = 0x1b,
  NXT_CMD_OPCODE_DIRECT_BT_GET_CONTACT_NAME = 0x1c,
  NXT_CMD_OPCODE_DIRECT_BT_GET_CONN_COUNT = 0x1d,
  NXT_CMD_OPCODE_DIRECT_BT_GET_CONN_NAME = 0x1e,
  NXT_CMD_OPCODE_DIRECT_SET_PROPERTY = 0x1f,
  NXT_CMD_OPCODE_DIRECT_GET_PROPERTY = 0x20,
  NXT_CMD_OPCODE_DIRECT_UPDATE_RESET_COUNT = 0x21,
  NXT_CMD_OPCODE_SYSTEM_OPENREAD = 0x80,
  NXT_CMD_OPCODE_SYSTEM_OPENWRITE = 0x81,
  NXT_CMD_OPCODE_SYSTEM_READ = 0x82,
  NXT_CMD_OPCODE_SYSTEM_WRITE = 0x83,
  NXT_CMD_OPCODE_SYSTEM_CLOSE = 0x84,
  NXT_CMD_OPCODE_SYSTEM_DELETE = 0x85,
  NXT_CMD_OPCODE_SYSTEM_FINDFIRST = 0x86,
  NXT_CMD_OPCODE_SYSTEM_FINDNEXT = 0x87,
  NXT_CMD_OPCODE_SYSTEM_VERSIONS = 0x88,
  NXT_CMD_OPCODE_SYSTEM_OPENWRITELINEAR = 0x89,
  NXT_CMD_OPCODE_SYSTEM_OPENREADLINEAR = 0x8a,
  NXT_CMD_OPCODE_SYSTEM_OPENWRITEDATA = 0x8b,
  NXT_CMD_OPCODE_SYSTEM_OPENAPPENDDATA = 0x8c,
  NXT_CMD_OPCODE_SYSTEM_CROPDATAFILE = 0x8d,
  NXT_CMD_OPCODE_SYSTEM_FINDFIRSTMODULE = 0x90,
  NXT_CMD_OPCODE_SYSTEM_FINDNEXTMODULE = 0x91,
  NXT_CMD_OPCODE_SYSTEM_CLOSEMODHANDLE = 0x92,
  NXT_CMD_OPCODE_SYSTEM_IOMAPREAD = 0x94,
  NXT_CMD_OPCODE_SYSTEM_IOMAPWRITE = 0x95,
  NXT_CMD_OPCODE_SYSTEM_BOOTCMD = 0x97,
  NXT_CMD_OPCODE_SYSTEM_SETBRICKNAME = 0x98,
  NXT_CMD_OPCODE_SYSTEM_BTGETADR = 0x9a,
  NXT_CMD_OPCODE_SYSTEM_DEVICEINFO = 0x9b,
  NXT_CMD_OPCODE_SYSTEM_DELETEUSERFLASH = 0xa0,
  NXT_CMD_OPCODE_SYSTEM_POLLCMDLEN = 0xa1,
  NXT_CMD_OPCODE_SYSTEM_POLLCMD = 0xa2,
  NXT_CMD_OPCODE_SYSTEM_RENAMEFILE = 0xa3,
  NXT_CMD_OPCODE_SYSTEM_BTFACTORYRESET = 0xa4,
} nxt_cmd_opcode_t;

/*
 * Using the same names as in the NXT firmware code, even if not consistent.
 */
typedef enum
{
  NXT_CMD_STATUS_NO_ERR = 0x00,
  NXT_CMD_STATUS_STAT_COMM_PENDING = 0x20,
  NXT_CMD_STATUS_STAT_MSG_EMPTY_MAILBOX = 0x40,
  NXT_CMD_STATUS_NOMOREHANDLES = 0x81,
  NXT_CMD_STATUS_NOSPACE = 0x82,
  NXT_CMD_STATUS_NOMOREFILES = 0x83,
  NXT_CMD_STATUS_EOFEXSPECTED = 0x84,
  NXT_CMD_STATUS_ENDOFFILE = 0x85,
  NXT_CMD_STATUS_NOTLINEARFILE = 0x86,
  NXT_CMD_STATUS_FILENOTFOUND = 0x87,
  NXT_CMD_STATUS_HANDLEALREADYCLOSED = 0x88,
  NXT_CMD_STATUS_NOLINEARSPACE = 0x89,
  NXT_CMD_STATUS_UNDEFINEDERROR = 0x8a,
  NXT_CMD_STATUS_FILEISBUSY = 0x8b,
  NXT_CMD_STATUS_NOWRITEBUFFERS = 0x8c,
  NXT_CMD_STATUS_APPENDNOTPOSSIBLE = 0x8d,
  NXT_CMD_STATUS_FILEISFULL = 0x8e,
  NXT_CMD_STATUS_FILEEXISTS = 0x8f,
  NXT_CMD_STATUS_MODULENOTFOUND = 0x90,
  NXT_CMD_STATUS_OUTOFBOUNDERY = 0x91,
  NXT_CMD_STATUS_ILLEGALFILENAME = 0x92,
  NXT_CMD_STATUS_ILLEGALHANDLE = 0x93,
  NXT_CMD_STATUS_ERR_RC_FAILED = 0xbd,
  NXT_CMD_STATUS_ERR_RC_UNKNOWN_CMD = 0xbe,
  NXT_CMD_STATUS_ERR_RC_BAD_PACKET = 0xbf,
  NXT_CMD_STATUS_ERR_RC_ILLEGAL_VAL = 0xc0,
  NXT_CMD_STATUS_ERR_COMM_BUS_ERR = 0xdd,
  NXT_CMD_STATUS_ERR_COMM_BUFFER_FULL = 0xde,
  NXT_CMD_STATUS_ERR_COMM_CHAN_INVALID = 0xdf,
  NXT_CMD_STATUS_ERR_COMM_CHAN_NOT_READY = 0xe0,
  NXT_CMD_STATUS_ERR_NO_PROG = 0xec,
  NXT_CMD_STATUS_ERR_INVALID_SIZE = 0xed,
  NXT_CMD_STATUS_ERR_INVALID_QUEUE = 0xee,
  NXT_CMD_STATUS_ERR_INVALID_FIELD = 0xef,
  NXT_CMD_STATUS_ERR_INVALID_PORT = 0xf0,
  NXT_CMD_STATUS_ERR_MEM = 0xfb,
  NXT_CMD_STATUS_ERR_ARG = 0xff,
} nxt_cmd_status_t;

typedef enum
{
  NXT_CMD_TYPE_DIRECT = 0x00,
  NXT_CMD_TYPE_SYSTEM = 0x01,
  NXT_CMD_TYPE_REPLY = 0x02,
  NXT_CMD_TYPE_REPLY_NOT_REQUIRED = 0x80,
  NXT_CMD_TYPE_DIRECT_NO_REPLY =
      NXT_CMD_TYPE_DIRECT | NXT_CMD_TYPE_REPLY_NOT_REQUIRED,
  NXT_CMD_TYPE_SYSTEM_NO_REPLY =
      NXT_CMD_TYPE_SYSTEM | NXT_CMD_TYPE_REPLY_NOT_REQUIRED,
} nxt_cmd_type_t;

typedef struct
{
  char name[NXT_NAME_SIZE];
  char serial[NXT_SERIAL_SIZE];
  uint8_t signal_strengths[4];
  int user_flash;
} nxt_device_info_t;

const char *nxt_cmd_str_error(nxt_cmd_status_t status);
nxt_error_t nxt_cmd_boot(nxt_t *nxt, bool sure);
nxt_error_t nxt_cmd_get_device_info(nxt_t *nxt, nxt_device_info_t *device_info);

#endif /* __CMD_H__ */
