from setuptools import setup, Extension
import pybind11
import os

ext_module = Extension(
    "flooride",
    ["main.cpp"],
    include_dirs=[pybind11.get_include()],
    extra_compile_args=["-O2"]
)
setup(
    name="flooride",
    version="0.1.17",
    description="light weight & low level ai framework",
    author="joemamasofat676767",
    ext_modules=[ext_module],
)
