#!/usr/bin/env python3
#
"""Embed flash routine as an array of bytes in a header file."""
#
# Copyright 2006 David Anderson <dave@natulte.net>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

import argparse
import sys

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('binary',
               help='flash routine binary file')
p.add_argument('template',
               help='header template')
p.add_argument('-o', '--output', metavar='output', required=True,
               help='output header')
options = p.parse_args()

# Read the binary
with open(options.binary, 'rb') as f:
    fwbin = f.read()

if len(fwbin) > 1024:
    print("The flash driver looks too big, refusing to embed.",
          file=sys.stderr)
    sys.exit(1)

data = [f'0x{c:02x}' for c in fwbin]

for i in range(0, len(data), 12):
    data[i] = "\n  " + data[i]

data_str = ', '.join(data)
len_data = "%d" % len(data)

# Read in the template
with open(options.template, 'rt') as tplfile:
    template = tplfile.read()

# Replace the values in the template
template = template.replace('___FLASH_BIN___', data_str + '\n')
template = template.replace('___FLASH_LEN___', len_data)

# Output the done header
with open(options.output, 'wt') as out:
    out.write(template)
