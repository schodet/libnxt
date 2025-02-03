fwexec(1)

# NAME

fwexec - upload firmware image to a connected NXT device and run it from RAM

# SYNOPSIS

*fwexec* [_options_]... _file_ [_load_address_ [_jump_address_]]

*fwexec* (*-l*|*-h*)

# DESCRIPTION

The *fwexec* utility is similar to *fwflash*(1), but the firmware image is
uploaded to NXT RAM and executed from there. The image must have been compiled
specially to handle this.

The NXT must be in bootloader mode, see *fwflash*(1) for more details.

The *fwexec* utility is part of LibNXT.

# OPTIONS

*-l*
	List detected devices and exit.
*-y*
	Allow reset to bootloader mode without prompt (this erases the NXT
	memory).
*-h*
	Show help message and exit.
*-b*
	Select device already in bootloader mode only, ignore other NXT devices.
*-s* _SERIAL_
	Select device with this serial (e.g. 00:16:53:01:02:03). This does not
	work in bootloader mode, devices in bootloader mode are always selected.
*-n* _NAME_
	Select device with this name (e.g. NXT). This does not work in
	bootloader mode, devices in bootloader mode are always selected.

# SEE ALSO

*fwflash*(1)

# AUTHOR

Maintained by Nicolas Schodet <nico@ni.fr.eu.org>.

Originally written by David Anderson <dave@natulte.net> based on first initial
version by Lawrie Griffiths.
