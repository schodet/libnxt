project('libnxt', 'c',
  default_options : 'warning_level=3',
  meson_version : '>=0.60.0',
  version : '0.4.2')

usbdep = dependency('libusb-1.0')

subdir('flash_write')
subdir('doc')

prog_python = import('python').find_installation()
flash_routine_h = custom_target(
  'flash_routine.h',
  output : 'flash_routine.h',
  input : ['make_flash_header.py', flash_bin, 'flash_routine.h.base'],
  command : [prog_python, '@INPUT0@', '-o', '@OUTPUT@', '@INPUT1@', '@INPUT2@'],
)

lib = static_library('nxt',
  'cmd.c',
  'error.c',
  'firmware.c',
  'flash.c',
  'lowlevel.c',
  'samba.c',
  flash_routine_h,
  dependencies : usbdep,
)

executable('fwflash',
  'main_fwflash.c', 'common.c',
  link_with : lib,
  install : true,
)
executable('fwexec',
  'main_fwexec.c', 'common.c',
  link_with : lib,
  install : true,
)
