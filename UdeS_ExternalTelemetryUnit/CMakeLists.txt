#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ExternalTelemetryUnit)

set(WPS_RADIO_COUNT 1)
set(APP "Cortical Implant")
set(HARDWARE "NRF")
set(MYPL "COORD") #Change between COORD or NODE for appropriate UWB device
set(NRF_HW_CONGIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src CACHE PATH "Path to the configuration file for the nrf hardware")

message("Generating build : ${MYPL}")

# NORDIC SDK APP START
target_sources(app 
  PRIVATE
    src/main.c
  PUBLIC
    src/hw_cfg.h
)

add_subdirectory(lib/spark_sdk_v1.3.0)
add_subdirectory(lib/usb_console)
add_subdirectory(lib/INA231)

# Include application events and configuration headers
zephyr_library_include_directories(
  lib/spark_sdk_v1.3.0
  lib/usb_console
  lib/INA231
  ${NRF_HW_CONGIG_PATH}
  ${CORTICAL_IMPLANT_PATH}
)


target_link_libraries(app 
  PRIVATE
    cortical_implant
#  PUBLIC
#    hardware
)

# NORDIC SDK APP END
zephyr_library_include_directories(.)




