project('raysweep', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

raylib = dependency('raylib')

exe = executable('raysweep', 'raysweep.c',
  install : true,
  dependencies : [raylib]
)

test('basic', exe)
