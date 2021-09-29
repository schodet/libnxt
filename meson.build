project('libnxt', 'c',
  version : '0.3')

usbdep = dependency('libusb', version : ['>=0.1', '<1.0'])

subdir('flash_write')

prog_python = import('python').find_installation('python3')
flash_routine_h = custom_target(
  'flash_routine.h',
  output : 'flash_routine.h',
  input : ['make_flash_header.py', flash_bin, 'flash_routine.h.base'],
  command : [prog_python, '@INPUT0@', '-o', '@OUTPUT@', '@INPUT1@', '@INPUT2@'],
)

lib = static_library('libnxt',
  'error.c',
  'firmware.c',
  'flash.c',
  'lowlevel.c',
  'samba.c',
  flash_routine_h,
)

executable('fwflash',
  'main_fwflash.c',
  link_with : lib, dependencies : usbdep,
  install : true,
)
executable('fwexec',
  'main_fwexec.c',
  link_with : lib, dependencies : usbdep,
  install : true,
)
