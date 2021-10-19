project('decimal128', 'c', 'cpp', version: '0.0.0', default_options: ['cpp_std=c++17', 'werror=true', 'optimization=2'])

include = [include_directories('src')]

gtest = dependency('gtest')

python = import('python').find_installation('python3')

generated = custom_target('generated.h',
    output : 'generated.h',
    input : 'src/decimal128.yaml',
    command : [python, meson.current_source_dir() / 'src/generate.py', '@INPUT@', '@OUTPUT@'],
)

test('test-decimal', executable('test-decimal',
	sources: ['src/test.cc', generated],
	include_directories: include,
	dependencies: [gtest],
	)
)
