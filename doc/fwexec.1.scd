fwexec(1)

# NAME

fwexec - upload firmware image to a connected NXT device and run it from RAM

# SYNOPSIS

*fwexec* _file_ [_load_address_ [_jump_address_]]

*fwexec* (*-l*|*-h*)

# DESCRIPTION

The *fwexec* utility is similar to *fwflash*(1), but the firmware image is
uploaded to NXT RAM and executed from there. The image must have been compiled
specially to handle this.

The *fwexec* utility is part of LibNXT.

# OPTIONS

*-l*
	List detected devices and exit.
*-h*
	Show help message and exit.

# SEE ALSO

*fwflash*(1)

# AUTHOR

Maintained by Nicolas Schodet <nico@ni.fr.eu.org>.

Originally written by David Anderson <dave@natulte.net> based on first initial
version by Lawrie Griffiths.
