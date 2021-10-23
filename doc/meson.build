scdoc = find_program('scdoc', required : get_option('build_doc'))
if scdoc.found()
  # Need to feed the input on standard input, so use the shell.
  sh = find_program('sh')
  custom_target(
    'fwflash.1',
    output : 'fwflash.1',
    input : 'fwflash.1.scd',
    command : [sh, '-c', scdoc.full_path() + ' < @INPUT@'],
    capture : true,
    install : true,
    install_dir : join_paths(get_option('mandir'), 'man1'),
  )
  custom_target(
    'fwexec.1',
    output : 'fwexec.1',
    input : 'fwexec.1.scd',
    command : [sh, '-c', scdoc.full_path() + ' < @INPUT@'],
    capture : true,
    install : true,
    install_dir : join_paths(get_option('mandir'), 'man1'),
  )
endif
