project(
    'sysload',
    ['c'],
    default_options: ['c_std=c99'],
    license: 'GPL-2.0',
    version: '1.0',
)

c_args = [
    '-Wall',
    '-Wextra',
    '-O2',
    '-D_GNU_SOURCE',
]

app_deps = [
    #dependency('tinyc'),
    #dependency('gtk+-3.0'),
    #dependency('glib-2.0'),
]

app_sources = [
    'cpuload.c',
    'gpuload.c',
    'main.c',
    'gettemp.c',
]

executable(
    meson.project_name(),
    c_args: c_args,
    dependencies: app_deps,
    sources: app_sources,
    install: true,
)


