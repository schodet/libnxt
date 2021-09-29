from glob import glob
from sys import byteorder


#
# Commandline options
#
opts = Options()
opts.Add(
    'prefix', 'Installation prefix.', '/usr')
opts.Add(
    'staging', 'Staging directory to install to. Useful for packaging.', '')
opts.Add(BoolOption(
    'debug', 'Build with debugging symbols and optimization turned off.', 0))


#
# Core environment
#
env = Environment(options = opts, CCFLAGS=['-Wall', '-Werror', '-std=gnu99'])
Help(opts.GenerateHelpText(env))

#
# "Autoconf" configuration
#
if not env.GetOption('clean'):
    conf = Configure(env)
    if not conf.CheckLibWithHeader('usb', 'usb.h', 'C'):
        print 'Could not find libusb, which is required by libnxt.'
        Exit(1)
    env = conf.Finish()

    # Set debugging or optimization flags
    if env['debug']:
        env.Append(CCFLAGS = ['-g', '-ggdb', '-O0'])
    else:
        env.Append(CCFLAGS = ['-O3'])


#
# Actual build rules.
#
env.Command('flash_routine.h',
            'flash_routine.h.base',
            './make_flash_header.py')

libnxt_sources = [x for x in glob('*.c') if not x.startswith('main_')]

libnxt_a = env.StaticLibrary('nxt', libnxt_sources, LIBS='usb')
libnxt_so = env.SharedLibrary('nxt', libnxt_sources, LIBS='usb')

prog_libs = ['usb', libnxt_so]

fwflash = env.Program('fwflash', 'main_fwflash.c', LIBS=prog_libs)
fwexec = env.Program('fwexec', 'main_fwexec.c', LIBS=prog_libs)

env.Default(libnxt_a, libnxt_so, fwflash, fwexec)

#
# Installation rules
#
install_root = env['staging'] + env['prefix']

install_libs = env.Install(install_root + '/lib', [libnxt_a, libnxt_so])
install_bins = env.Install(install_root + '/bin', [fwflash, fwexec])
env.Alias('install', [install_libs, install_bins])
