scdoc = find_program('scdoc', required : get_option('build_doc'))
if scdoc.found()
  man_files = [
    'fwflash.1',
    'fwexec.1',
  ]
  foreach filename : man_files
    man = custom_target(
      filename,
      output : filename,
      input : filename + '.scd',
      command : scdoc,
      feed : true,
      capture : true,
      install : true,
      install_dir : get_option('mandir') / 'man1',
      install_tag : 'man',
    )
  endforeach
endif
