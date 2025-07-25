# genchk.cmake.in
# Generate .chk from .out with awk (generic), based upon the automake logic.

# Copyright (c) 2022-2024 Cosmin Truta
# Copyright (c) 2016 Glenn Randers-Pehrson
# Written by Roger Leigh, 2016
#
# Use, modification and distribution are subject to
# the same licensing terms and conditions as libpng.
# Please see the copyright notice in png.h or visit
# http://libpng.org/pub/png/src/libpng-LICENSE.txt
#
# SPDX-License-Identifier: libpng-2.0

# Variables substituted from CMakeLists.txt
set(SRCDIR "/home/xuncheng/game/c++game/libpng")
set(BINDIR "/home/xuncheng/game/c++game/cmake-build-debug/libpng")

set(AWK "/usr/bin/gawk")

get_filename_component(INPUTEXT "${INPUT}" EXT)
get_filename_component(OUTPUTEXT "${OUTPUT}" EXT)
get_filename_component(INPUTBASE "${INPUT}" NAME_WE)
get_filename_component(OUTPUTBASE "${OUTPUT}" NAME_WE)
get_filename_component(INPUTDIR "${INPUT}" PATH)
get_filename_component(OUTPUTDIR "${OUTPUT}" PATH)

if(INPUTEXT STREQUAL ".out" AND OUTPUTEXT STREQUAL ".chk")
  # Generate .chk from .out with awk (generic)
  file(REMOVE "${OUTPUT}" "${OUTPUTDIR}/${OUTPUTBASE}.new")
  execute_process(COMMAND "${AWK}" -f "${BINDIR}/scripts/checksym.awk"
                          "${SRCDIR}/scripts/${INPUTBASE}.def"
                          "of=${OUTPUTDIR}/${OUTPUTBASE}.new"
                          "${INPUT}"
                  RESULT_VARIABLE AWK_FAIL)
  if(AWK_FAIL)
    message(FATAL_ERROR "Failed to generate ${OUTPUTDIR}/${OUTPUTBASE}.new")
  endif()
  file(RENAME "${OUTPUTDIR}/${OUTPUTBASE}.new" "${OUTPUT}")
else()
  message(FATAL_ERROR "Unsupported conversion: ${INPUTEXT} to ${OUTPUTEXT}")
endif()
