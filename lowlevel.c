/**
 * NXT bootstrap interface; low-level USB functions.
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
#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lowlevel.h"

const struct
{
  int vendor_id;
  int product_id;
  int interface;
} nxt_usb_ids[N_FIRMWARES] = {
  { 0x03EB, 0x6124, 1 }, /* SAM-BA */
  { 0x0694, 0x0002, 0 }, /* LEGO   */
  { 0x0694, 0xFF00, 0 }  /* NXTOS  */
};

struct nxt_t
{
  libusb_context *usb;
  libusb_device *dev;
  nxt_firmware firmware;
  int interface;
  libusb_device_handle *hdl;
};

nxt_error_t
nxt_init(nxt_t **nxt)
{
  int ret;
  nxt_t *lnxt;

  lnxt = calloc(1, sizeof(*lnxt));
  if (!lnxt)
    return NXT_ERROR_NO_MEM;

  ret = libusb_init(&lnxt->usb);
  if (ret < 0)
    {
      free(lnxt);
      return NXT_ERROR_USB(ret);
    }

  *nxt = lnxt;
  return NXT_OK;
}

void
nxt_exit(nxt_t *nxt)
{
  nxt_close(nxt);
  libusb_exit(nxt->usb);
  free(nxt);
}

static nxt_firmware
nxt_get_firmware(const struct libusb_device_descriptor *desc)
{
  for (nxt_firmware fw = 0; fw < N_FIRMWARES; fw++)
    {
      if (desc->idVendor == nxt_usb_ids[fw].vendor_id &&
          desc->idProduct == nxt_usb_ids[fw].product_id)
        {
          return fw;
        }
    }
  return N_FIRMWARES;
}

static nxt_error_t
nxt_get_serial(libusb_device *dev, const struct libusb_device_descriptor *desc,
               char *serial, size_t serial_size)
{
  int ret;
  libusb_device_handle *hdl;

  ret = libusb_open(dev, &hdl);
  if (ret != 0)
    {
      return NXT_ERROR_USB(ret);
    }
  else
    {
      ret = libusb_get_string_descriptor_ascii(hdl, desc->iSerialNumber,
                                               (uint8_t *)serial, serial_size);
      libusb_close(hdl);
      if (ret < 0)
        return NXT_ERROR_USB(ret);
      else
        return NXT_OK;
    }
}

nxt_error_t
nxt_list(nxt_t *nxt, nxt_list_cb_t cb, void *user)
{
  libusb_device **list;

  assert(!nxt->dev);

  ssize_t cnt = libusb_get_device_list(nxt->usb, &list);
  if (cnt < 0)
    return NXT_ERROR_USB(cnt);
  for (ssize_t i = 0; i < cnt; i++)
    {
      libusb_device *dev = list[i];
      struct libusb_device_descriptor desc;
      int ret = libusb_get_device_descriptor(dev, &desc);
      if (ret == 0)
        {
          nxt_firmware fw = nxt_get_firmware(&desc);
          if (fw != N_FIRMWARES)
            {
              char connection[NXT_CONNECTION_SIZE];
              ret = snprintf(connection, sizeof(connection), "usb.%d.%d",
                             libusb_get_bus_number(dev),
                             libusb_get_device_address(dev));
              assert(ret < (int)sizeof(connection));
              char serial_tab[20];
              char *serial = NULL;
              if (fw == LEGO)
                {
                  nxt_error_t nret = nxt_get_serial(dev, &desc, serial_tab,
                                                    sizeof(serial_tab));
                  if (nret == NXT_OK)
                    serial = serial_tab;
                }
              cb(user, connection, fw, serial, NULL);
            }
        }
    }

  libusb_free_device_list(list, 1);
  return NXT_OK;
}

nxt_error_t
nxt_find(nxt_t *nxt)
{
  libusb_device **list;

  assert(!nxt->dev);

  ssize_t cnt = libusb_get_device_list(nxt->usb, &list);
  if (cnt < 0)
    {
      return NXT_ERROR_USB(cnt);
    }
  for (ssize_t i = 0; i < cnt; i++)
    {
      libusb_device *dev = list[i];
      struct libusb_device_descriptor desc;
      int ret = libusb_get_device_descriptor(dev, &desc);
      if (ret == 0)
        {
          nxt_firmware fw = nxt_get_firmware(&desc);
          if (fw != N_FIRMWARES)
            {
              libusb_ref_device(dev);
              libusb_free_device_list(list, 1);
              nxt->dev = dev;
              nxt->firmware = fw;
              nxt->interface = nxt_usb_ids[fw].interface;
              return NXT_OK;
            }
        }
    }

  libusb_free_device_list(list, 1);
  return NXT_NOT_PRESENT;
}

nxt_error_t
nxt_open(nxt_t *nxt)
{
  int ret;
  libusb_device_handle *hdl;

  assert(nxt->dev);
  assert(!nxt->hdl);

  ret = libusb_open(nxt->dev, &hdl);
  if (ret < 0)
    return NXT_ERROR_USB(ret);

  // Try to detach driver, but ignore errors when not implemented, or kernel
  // driver not attached.
  ret = libusb_detach_kernel_driver(hdl, nxt->interface);
  if (ret < 0 && ret != LIBUSB_ERROR_NOT_SUPPORTED &&
      ret != LIBUSB_ERROR_NOT_FOUND)
    {
      libusb_close(hdl);
      return NXT_ERROR_USB(ret);
    }

  ret = libusb_set_configuration(hdl, 1);
  if (ret < 0 && ret != LIBUSB_ERROR_NOT_SUPPORTED)
    {
      libusb_close(hdl);
      return NXT_ERROR_USB(ret);
    }

  ret = libusb_claim_interface(hdl, nxt->interface);
  if (ret < 0)
    {
      libusb_close(hdl);
      return NXT_ERROR_USB(ret);
    }

  nxt->hdl = hdl;
  return NXT_OK;
}

void
nxt_close(nxt_t *nxt)
{
  if (nxt->hdl)
    {
      libusb_release_interface(nxt->hdl, nxt->interface);
      libusb_close(nxt->hdl);
      nxt->hdl = NULL;
    }
  if (nxt->dev)
    {
      libusb_unref_device(nxt->dev);
      nxt->dev = NULL;
    }
}

int
nxt_is_firmware(nxt_t *nxt, nxt_firmware fw)
{
  return (nxt->firmware == fw);
}

static nxt_error_t
nxt_transfer_buf(nxt_t *nxt, uint8_t endpoint, uint8_t *buf, int len)
{
  int ret;
  int transfered;

  do
    {
      ret = libusb_bulk_transfer(nxt->hdl, endpoint, buf, len, &transfered, 0);
      if (ret < 0)
        return NXT_ERROR_USB(ret);
      buf += transfered;
      len -= transfered;
    }
  while (len);

  return NXT_OK;
}

nxt_error_t
nxt_send_buf(nxt_t *nxt, const uint8_t *buf, int len)
{
  return nxt_transfer_buf(nxt, 0x01, (uint8_t *)buf, len);
}

nxt_error_t
nxt_send_str(nxt_t *nxt, const char *str)
{
  return nxt_send_buf(nxt, (const uint8_t *)str, strlen(str));
}

nxt_error_t
nxt_recv_buf(nxt_t *nxt, uint8_t *buf, int len)
{
  return nxt_transfer_buf(nxt, 0x82, buf, len);
}
