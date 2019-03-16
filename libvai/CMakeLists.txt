## Copyright(c) 2017, Intel Corporation
##
## Redistribution  and  use  in source  and  binary  forms,  with  or  without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of  source code  must retain the  above copyright notice,
##   this list of conditions and the following disclaimer.
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and/or other materials provided with the distribution.
## * Neither the name  of Intel Corporation  nor the names of its contributors
##   may be used to  endorse or promote  products derived  from this  software
##   without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
## IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT  SHALL THE COPYRIGHT OWNER  OR CONTRIBUTORS BE
## LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
## CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT LIMITED  TO,  PROCUREMENT  OF
## SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS
## INTERRUPTION)  HOWEVER CAUSED  AND ON ANY THEORY  OF LIABILITY,  WHETHER IN
## CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE  OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.

project(libvai-c)

############################################################################
## Add 'coverage' ##########################################################
############################################################################

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  include(coverage)
endif(CMAKE_BUILD_TYPE STREQUAL "Coverage")

############################################################################
## Add 'libopae-c' library ###################################################
############################################################################
set(SRC
  src/malloc.c
  src/wrapper.c
  src/vai.c)

# Define target
add_library(vai-c SHARED ${SRC})
add_dependencies(vai-c copy-common-opae-header-files)
target_link_libraries(vai-c
  m
  safestr
  ${libjson-c_LIBRARIES}
  ${libuuid_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths. Keep current directory private.
target_include_directories(vai-c PUBLIC
  $<BUILD_INTERFACE:${OPAE_INCLUDE_DIR}>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE src)

# Add coverage flags
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  set_property(SOURCE ${SRC} APPEND PROPERTY COMPILE_FLAGS ${GCOV_COMPILE_FLAGS})
endif(CMAKE_BUILD_TYPE STREQUAL "Coverage")

# Target properties
set_property(TARGET vai-c PROPERTY C_STANDARD 99)
set_target_properties(vai-c PROPERTIES
  VERSION ${INTEL_FPGA_API_VERSION}
  SOVERSION ${INTEL_FPGA_API_VER_MAJOR})

# Add coverage flags
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  target_link_libraries(vai-c ${GCOV_LINK_FLAGS})
endif(CMAKE_BUILD_TYPE STREQUAL "Coverage")

# Set debug flags, if required
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DLIBVAI_DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Hide non-public symbols when building release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("!! Building for release, may break internal tests")
  set_property(TARGET vai-c PROPERTY C_VISIBILITY_PRESET hidden)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

install(TARGETS vai-c
  LIBRARY DESTINATION ${OPAE_LIB_INSTALL_DIR}
  COMPONENT vaiclib)


############################################################################
## Create config_int.h #########################################################
############################################################################

configure_file ("${CMAKE_SOURCE_DIR}/cmake/config/config.h.in"
                "${PROJECT_BINARY_DIR}/include/config_int.h" )
