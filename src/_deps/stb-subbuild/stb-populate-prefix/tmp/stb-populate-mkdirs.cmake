# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-src"
  "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-build"
  "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-subbuild/stb-populate-prefix"
  "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-subbuild/stb-populate-prefix/tmp"
  "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp"
  "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-subbuild/stb-populate-prefix/src"
  "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/timnogga/CLionProjects/cg-animation/src/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
