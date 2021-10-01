/**
 * NXT bootstrap interface; low-level USB functions.
 *
 * Copyright 2006 David Anderson <david.anderson@calixo.net>
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
#include <string.h>
#include <unistd.h>
#include <usb.h>

#include "lowlevel.h"

const struct
{
  int vendor_id;
  int product_id;
} nxt_usb_ids[N_FIRMWARES] = {
  { 0x03EB, 0x6124 }, /* SAM-BA */
  { 0x0694, 0x0002 }, /* LEGO   */
  { 0x0694, 0xFF00 }  /* NXTOS  */
};

struct nxt_t
{
  struct usb_device *dev;
  struct usb_dev_handle *hdl;
  nxt_firmware firmware;
  int interface;
};

nxt_error_t
nxt_init(nxt_t **nxt)
{
  usb_init();
  *nxt = calloc(1, sizeof(**nxt));

  return NXT_OK;
}

nxt_error_t
nxt_find(nxt_t *nxt)
{
  struct usb_bus *busses, *bus;

  usb_find_busses();
  usb_find_devices();

  busses = usb_get_busses();

  for (bus = busses; bus != NULL; bus = bus->next)
    {
      struct usb_device *dev;

      for (dev = bus->devices; dev != NULL; dev = dev->next)
        {
          int i;

          for (i = 0; i < N_FIRMWARES; i++)
            if (dev->descriptor.idVendor == nxt_usb_ids[i].vendor_id &&
                dev->descriptor.idProduct == nxt_usb_ids[i].product_id)
              {
                nxt->dev = dev;
                nxt->firmware = i;
                return NXT_OK;
              }
        }
    }

  return NXT_NOT_PRESENT;
}

nxt_error_t
nxt_open(nxt_t *nxt, int interface)
{
  int ret;

  nxt->hdl = usb_open(nxt->dev);

  // Try to detach driver, but ignore errors, only implemented on Linux
  usb_detach_kernel_driver_np(nxt->hdl, interface);

  ret = usb_set_configuration(nxt->hdl, 1);
  if (ret < 0)
    {
      usb_close(nxt->hdl);
      return NXT_CONFIGURATION_ERROR;
    }

  ret = usb_claim_interface(nxt->hdl, interface);
  if (ret < 0)
    {
      usb_close(nxt->hdl);
      return NXT_IN_USE;
    }

  nxt->interface = interface;

  return NXT_OK;
}

nxt_error_t
nxt_close(nxt_t *nxt)
{
  usb_release_interface(nxt->hdl, nxt->interface);
  usb_close(nxt->hdl);
  free(nxt);

  return NXT_OK;
}

int
nxt_is_firmware(nxt_t *nxt, nxt_firmware fw)
{
  return (nxt->firmware == fw);
}

nxt_error_t
nxt_send_buf(nxt_t *nxt, char *buf, int len)
{
  int ret = usb_bulk_write(nxt->hdl, 0x1, buf, len, 0);
  if (ret < 0)
    return NXT_USB_WRITE_ERROR;

  return NXT_OK;
}

nxt_error_t
nxt_send_str(nxt_t *nxt, char *str)
{
  return nxt_send_buf(nxt, str, strlen(str));
}

nxt_error_t
nxt_recv_buf(nxt_t *nxt, char *buf, int len)
{
  int ret = usb_bulk_read(nxt->hdl, 0x82, buf, len, 0);
  if (ret < 0)
    return NXT_USB_READ_ERROR;

  return NXT_OK;
}
