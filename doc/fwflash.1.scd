fwflash(1)

# NAME

fwflash - flash firmware image to a connected NXT device

# SYNOPSIS

*fwflash* [_options_]... _file_

*fwflash* (*-l*|*-h*)

# DESCRIPTION

The *fwflash* utility takes a firmware image file and sends it to a connected
NXT device through USB. It takes only one parameter which is the firmware file
to send. This file usually has the .bin or .rfw extension and a size of
262144 octets.

Connect the NXT using a USB cable. Make sure it is detected by the computer
when powered on.

If you run *fwflash* now, it should report that the NXT is found, but not
running in bootloader mode. *fwflash* can reset the NXT to bootloader for you.

*The next step will erase the current firmware image!*

If you answer "RESET", *fwflash* will erase the current firmware and will wait
for the NXT to be detected in bootloader mode. The prompt can be skipped using
the *-y* option.

Another option is to reset the NXT manually. To do so, press and hold, for at
least five seconds, the reset button which is hidden in the pin hole under the
USB port. The brick should make a clicking noise and be detected by the computer
in SAM-BA mode.

You can then use *fwflash* to send the firmware. After a few seconds, it
should announce successful flashing.

In case of problem, check your USB cable, and your device permissions.

The *fwflash* utility is part of LibNXT.

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

*fwexec*(1)

# AUTHOR

Maintained by Nicolas Schodet <nico@ni.fr.eu.org>.

Originally written by David Anderson <dave@natulte.net>.
