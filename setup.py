from setuptools import setup, Extension
import numpy as np
from setuptools.command.build_ext import build_ext
import sys


class BuildExt(build_ext):
    """Custom build extension for handling compiler flags."""

    def build_extensions(self):
        if sys.platform == 'darwin':  # macOS
            for ext in self.extensions:
                ext.extra_compile_args += ['-O3', '-march=native', '-ffast-math']
                ext.extra_link_args += ['-Wl,-undefined,dynamic_lookup']
        elif sys.platform.startswith('linux'):  # Linux
            for ext in self.extensions:
                ext.extra_compile_args += ['-O3', '-march=native', '-ffast-math']
                ext.extra_link_args += ['-Wl,--allow-multiple-definition']
        elif sys.platform == 'win32':  # Windows
            for ext in self.extensions:
                ext.extra_compile_args += ['/O2', '/arch:AVX2']

        # Call the original build_extensions
        build_ext.build_extensions(self)


# Source files for the extension
sources = [
    'visvalingam.c',
    'min_heap.c',
    'geometry.c'
]

# Define the extension module
visvalingam_module = Extension(
    'visvalingam_c',
    sources=sources,
    include_dirs=[
        np.get_include(),
        'include'  # Header files directory
    ],
    define_macros=[
        ('NPY_NO_DEPRECATED_API', 'NPY_1_7_API_VERSION'),
        ('PY_SSIZE_T_CLEAN', None),
    ],
    # Platform-specific compiler flags will be added by BuildExt
)

setup(
    name='visvalingam_c',
    version='0.0.4',
    description='C implementation of Visvalingam-Whyatt polygon simplification',
    author='Lukas Sz',
    author_email='nahhh@bro.com.on',
    ext_modules=[visvalingam_module],
    cmdclass={'build_ext': BuildExt},
    setup_requires=['numpy'],
    install_requires=['numpy>=1.17.0'],
    python_requires='>=3.7',
    classifiers=[
        'Programming Language :: Python :: 3.10',
        'Programming Language :: C',
    ],
)