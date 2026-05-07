# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/opt/esp5/esp-idf/components/bootloader/subproject"
  "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader"
  "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader-prefix"
  "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader-prefix/tmp"
  "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader-prefix/src/bootloader-stamp"
  "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader-prefix/src"
  "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/auto/fse/gelzinga/IOTSHOWERREMOTE/IOTSHOWERREMOTE/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
