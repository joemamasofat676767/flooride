from setuptools import setup, Extension
import pybind11
import os

ext_module = Extension(
    "flooride",
    ["main.cpp"],
    include_dirs=[pybind11.get_include()],
    extra_compile_args=["-O2", "-std=c++20", "-D__ANDROID_API__=30"]
)
setup(
    name="flooride",
    version="1.1.0",
    description="light weight & low level ai framework",
    author="joemamasofat676767",
    ext_modules=[ext_module],
)
