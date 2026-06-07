# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/ZeebrooPosDesktop_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ZeebrooPosDesktop_autogen.dir/ParseCache.txt"
  "ZeebrooPosDesktop_autogen"
  )
endif()
