cc = find_program('arm-none-eabi-gcc')
as = find_program('arm-none-eabi-as')
ld = find_program('arm-none-eabi-ld')
objcopy = find_program('arm-none-eabi-objcopy')

flash_o = custom_target(
  'flash.o',
  output : 'flash.o',
  input : 'flash.c',
  command : [cc, '-mcpu=arm7tdmi', '-msoft-float', '-mapcs', '-W', '-Wall',
    '-O3', '-c', '-o', '@OUTPUT@', '@INPUT@'],
)

crt0_o = custom_target(
  'crt0.o',
  output : 'crt0.o',
  input : 'crt0.s',
  command : [as, '-mcpu=arm7tdmi', '-mfpu=softfpa', '-mapcs-32', '--warn',
    '-o', '@OUTPUT@', '@INPUT@'],
)

flash_elf = custom_target(
  'flash.elf',
  output : 'flash.elf',
  input : [crt0_o, flash_o],
  command : [ld, '--gc-sections', '-o', '@OUTPUT@', '@INPUT@'],
)

flash_bin = custom_target(
  'flash.bin',
  output : 'flash.bin',
  input : flash_elf,
  command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
)
