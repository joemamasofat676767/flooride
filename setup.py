from setuptools import setup, Extension
import pybind11
import os

ext_module = Extension(
    "flooride",
    ["main.cpp"],
    include_dirs=[pybind11.get_include(), os.path.abspath(".")],
    extra_compile_args=["-DJSON_IMPLEMENTATION"]
)
setup(
    name="flooride",
    version="0.1.12",
    ext_modules=[ext_module],
    package_data={"":["*.h"]},
    include_package_data=True
)
