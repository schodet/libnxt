fwflash(1)

# NAME

fwflash - flash firmware image to a connected NXT device

# SYNOPSIS

*fwflash* _file_

*fwflash* (*-l*|*-h*)

# DESCRIPTION

The *fwflash* utility takes a firmware image file and sends it to a connected
NXT device through USB. It takes only one parameter which is the firmware file
to send. This file usually has the .bin or .rfw extension and a size of
262144 octets.

Connect the NXT using a USB cable. Make sure it is detected by the computer
when powered on. If you run *fwflash* now, it should report that the NXT is
found, but not running in reset mode.

*The next step will erase the current firmware image!*

Press and hold, for at least five seconds, the reset button which is hidden in
the pin hole under the USB port. The brick should make a clicking noise and be
detected by the computer in SAM-BA mode.

You can then use *fwflash* to send the firmware. After a few seconds, it
should announce successful flashing.

In case of problem, check your USB cable, and your device permissions.

The *fwflash* utility is part of LibNXT.

# OPTIONS

*-l*
	List detected devices and exit.
*-h*
	Show help message and exit.

# SEE ALSO

*fwexec*(1)

# AUTHOR

Maintained by Nicolas Schodet <nico@ni.fr.eu.org>.

Originally written by David Anderson <dave@natulte.net>.
