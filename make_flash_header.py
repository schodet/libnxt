#!/usr/bin/env python3
#
# Take the flash_routine.bin file, and embed it as an array of bytes
# in a flash_routine.h, ready for packaging with the C firmware
# flasher.
#
# If a file name is provided on the commandline, load that file as the
# firmware flashing routine instead.
#

import sys
import os
import os.path
import urllib.request, urllib.error, urllib.parse
import hashlib

FLASH_DIR = 'flash_write'
FLASH_BIN = 'flash.bin'
FLASH_PATH = os.path.join(FLASH_DIR, FLASH_BIN)

DOWNLOAD_FLASH_CHECKSUM = '589501072d76be483f873a787080adcab20841f4'
DOWNLOAD_FLASH_URL = 'http://libnxt.googlecode.com/files/flash.bin'

def check_flash_size():
    statinfo = os.stat(FLASH_PATH)
    if statinfo.st_size > 1024:
        print("The flash driver looks too big, refusing to embed.")
        return False
    return True

def ensure_flash_bin():
    # If the flash binary does exist, just check its size and return.
    if os.path.isfile(FLASH_PATH):
        return check_flash_size()

    # If the binary doesn't exist, offer to download a binary build
    # from Google Code.
    print("""
Embedded flash driver not found. This is required to build LibNXT.

If you have an ARM7 cross-compiler toolchain available, you can build
the flash driver by interrupting (ctrl-C) this build and running
'make' in the 'flash_write' subdirectory. Then rerun this build again,
and everything should work great.

If you do not have a cross-compiler, do not despair! I can also
download a copy of the compiled driver (built by the libnxt developer)
from the project website and use that.
""")
    reply = input("Is that okay? (y/n) ")
    if reply not in ('y', 'Y', 'yes'):
        print ("Okay, you're the boss. But that does mean I can't build "
               "LibNXT. Sorry.")
        return False
    f = urllib.request.urlopen(DOWNLOAD_FLASH_URL)
    data = f.read()
    f.close()

    # Verify the SHA-1 checksum
    checksum = hashlib.sha1(data).hexdigest()
    if checksum != DOWNLOAD_FLASH_CHECKSUM:
        print("Oops, the flash binary I downloaded has the wrong checksum!")
        print("Aborting :(")
        return False

    with open(FLASH_PATH, 'wb') as f:
        f.write(data)

    if os.path.isfile(FLASH_PATH):
        return check_flash_size()

    # Dude, you're really not lucky, are you.
    return False


def main():
    if not ensure_flash_bin():
        sys.exit(1)

    with open(FLASH_PATH, 'rb') as f:
        fwbin = f.read()

    data = [f'0x{c:02x}' for c in fwbin]

    for i in range(0, len(data), 12):
        data[i] = "\n  " + data[i]

    data_str = ', '.join(data)
    len_data = "%d" % len(data)

    # Read in the template
    with open('flash_routine.h.base', 'rt') as tplfile:
        template = tplfile.read()

    # Replace the values in the template
    template = template.replace('___FLASH_BIN___', data_str + '\n')
    template = template.replace('___FLASH_LEN___', len_data)

    # Output the done header
    with open('flash_routine.h', 'wt') as out:
        out.write(template)

if __name__ == '__main__':
    main()

