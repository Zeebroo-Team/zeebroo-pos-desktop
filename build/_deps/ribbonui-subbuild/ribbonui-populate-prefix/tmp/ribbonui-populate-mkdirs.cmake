# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-src")
  file(MAKE_DIRECTORY "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-src")
endif()
file(MAKE_DIRECTORY
  "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-build"
  "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-subbuild/ribbonui-populate-prefix"
  "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-subbuild/ribbonui-populate-prefix/tmp"
  "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-subbuild/ribbonui-populate-prefix/src/ribbonui-populate-stamp"
  "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-subbuild/ribbonui-populate-prefix/src"
  "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-subbuild/ribbonui-populate-prefix/src/ribbonui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-subbuild/ribbonui-populate-prefix/src/ribbonui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/mac/Documents/Projects/socibiz/pos-desktop/build/_deps/ribbonui-subbuild/ribbonui-populate-prefix/src/ribbonui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
