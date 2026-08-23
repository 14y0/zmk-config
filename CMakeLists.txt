cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(threekey_custom)

zephyr_library()
zephyr_library_sources(src/i2c_probe.c)
