#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PNG::png_shared" for configuration "Debug"
set_property(TARGET PNG::png_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PNG::png_shared PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpng16d.so.16.51.git"
  IMPORTED_SONAME_DEBUG "libpng16d.so.16"
  )

list(APPEND _cmake_import_check_targets PNG::png_shared )
list(APPEND _cmake_import_check_files_for_PNG::png_shared "${_IMPORT_PREFIX}/lib/libpng16d.so.16.51.git" )

# Import target "PNG::png_static" for configuration "Debug"
set_property(TARGET PNG::png_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PNG::png_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpng16d.a"
  )

list(APPEND _cmake_import_check_targets PNG::png_static )
list(APPEND _cmake_import_check_files_for_PNG::png_static "${_IMPORT_PREFIX}/lib/libpng16d.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
