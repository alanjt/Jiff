cmake_minimum_required (VERSION 2.6.4)

include (CheckFunctionExists)
include (CheckCSourceCompiles)
include (CheckCXXSourceCompiles)
include (CheckIncludeFile)
include (CPack)

project(jiff)

file(READ version versionFile)
string(STRIP ${versionFile} JIFF_VERSION)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

if(${CMAKE_VERSION} VERSION_GREATER 2.8.4)
    # use official include provided by latest CMake
    include(GNUInstallDirs)
else(${CMAKE_VERSION} VERSION_GREATER 2.8.4)
    # backward compatibility: use our own module for older cmake versions
    include(OldGNUInstallDirs)
endif(${CMAKE_VERSION} VERSION_GREATER 2.8.4)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")


#packaging
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
SET(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "JSBSim Table plotter")
SET(CPACK_PACKAGE_VENDOR "The FlightGear project")
SET(CPACK_GENERATOR "TBZ2")
SET(CPACK_INSTALL_CMAKE_PROJECTS  ${CMAKE_CURRENT_BINARY_DIR};JIFF;ALL;/)


# split version string into components, note CMAKE_MATCH_0 is the entire regexp match
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" CPACK_PACKAGE_VERSION ${JIFF_VERSION} )
set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1})
set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2})

message(STATUS "version is ${CPACK_PACKAGE_VERSION_MAJOR} dot ${CPACK_PACKAGE_VERSION_MINOR}")

set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "jiff-${JIFF_VERSION}" CACHE INTERNAL "tarball basename")
set(CPACK_SOURCE_IGNORE_FILES
  "\\\\.svn;Makefile.am;~$;${CPACK_SOURCE_IGNORE_FILES}")

message(STATUS "ignoring: ${CPACK_SOURCE_IGNORE_FILES}")

option(SIMGEAR_SHARED "Set to ON to dynamically link SimGear" OFF)

if (MSVC)
    GET_FILENAME_COMPONENT(PARENT_DIR ${PROJECT_SOURCE_DIR} PATH)
    if (CMAKE_CL_64)
        SET(TEST_3RDPARTY_DIR "${PARENT_DIR}/3rdparty.x64")
    else (CMAKE_CL_64)
        SET(TEST_3RDPARTY_DIR "${PARENT_DIR}/3rdparty")
    endif (CMAKE_CL_64)
    if (EXISTS ${TEST_3RDPARTY_DIR})
        set(MSVC_3RDPARTY_ROOT ${PARENT_DIR} CACHE PATH "Location where the third-party dependencies are extracted")
    else (EXISTS ${TEST_3RDPARTY_DIR})
        set(MSVC_3RDPARTY_ROOT NOT_FOUND CACHE PATH "Location where the third-party dependencies are extracted")
    endif (EXISTS ${TEST_3RDPARTY_DIR})
    list(APPEND PLATFORM_LIBS "winmm.lib" "Shlwapi.lib")
else (MSVC)
    set(MSVC_3RDPARTY_ROOT NOT_FOUND CACHE PATH "Location where the third-party dependencies are extracted")
endif (MSVC)

if (MSVC AND MSVC_3RDPARTY_ROOT)

  message(STATUS "3rdparty files located in ${MSVC_3RDPARTY_ROOT}")


  if (CMAKE_CL_64)
      set( MSVC_3RDPARTY_DIR 3rdParty.x64 )
  else (CMAKE_CL_64)
      set( MSVC_3RDPARTY_DIR 3rdParty )
  endif (CMAKE_CL_64)

  set (CMAKE_LIBRARY_PATH ${MSVC_3RDPARTY_ROOT}/${MSVC_3RDPARTY_DIR}/lib ${MSVC_3RDPARTY_ROOT}/install/SimGear/lib )
  set (CMAKE_INCLUDE_PATH  ${MSVC_3RDPARTY_ROOT}/${MSVC_3RDPARTY_DIR}/include include ${MSVC_3RDPARTY_ROOT}/install/SimGear/include)

  GET_FILENAME_COMPONENT(MSVC_ROOT_PARENT_DIR ${MSVC_3RDPARTY_ROOT} PATH)
  
  find_path(BOOST_ROOT boost/version.hpp
      ${MSVC_ROOT_PARENT_DIR}
			${MSVC_3RDPARTY_ROOT}/boost
			${MSVC_3RDPARTY_ROOT}/boost_1_51_0
			${MSVC_3RDPARTY_ROOT}/boost_1_50_0
			${MSVC_3RDPARTY_ROOT}/boost_1_49_0
			${MSVC_3RDPARTY_ROOT}/boost_1_48_0
			${MSVC_3RDPARTY_ROOT}/boost_1_47_0
			${MSVC_3RDPARTY_ROOT}/boost_1_46_1
			${MSVC_3RDPARTY_ROOT}/boost_1_46_0
			${MSVC_3RDPARTY_ROOT}/boost_1_45_0
			${MSVC_3RDPARTY_ROOT}/boost_1_44_0
			)
   message(STATUS "BOOST_ROOT is ${BOOST_ROOT}")
  if (CMAKE_CL_64)
	  set( BOOST_LIBRARYDIR "${BOOST_ROOT}/lib64" )
  else (CMAKE_CL_64)
	  set( BOOST_LIBRARYDIR "${BOOST_ROOT}/lib" )
  endif (CMAKE_CL_64)
  set (Boost_USE_STATIC_LIBS 1)

endif (MSVC AND MSVC_3RDPARTY_ROOT)

if(WIN32)

    if(MSVC)
        set(MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS")
        if (${MSVC_VERSION} GREATER 1599)
          set( MSVC_LD_FLAGS "/FORCE:MULTIPLE /SUBSYSTEM:CONSOLE")
        endif (${MSVC_VERSION} GREATER 1599)
    endif(MSVC)

    set(NOMINMAX 1)

endif(WIN32)

##set(Boost_NO_SYSTEM_PATHS true)
## set (Boost_USE_STATIC_LIBS OFF CACHE BOOL "use static libraries from Boost")
##set (Boost_USE_MULTITHREADED ON)
##set (BOOST_CXX_FLAGS "-DBOOST_BIMAP_DISABLE_SERIALIZATION")
find_package(Boost COMPONENTS program_options REQUIRED)
##include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})
if (WIN32)
  # disable autolinking in boost
  ##add_definitions( -DBOOST_ALL_NO_LIB )

  # force all boost libraries to dynamic link (we already disabled
  # autolinking, so I don't know why we need this, but we do!)
  ##add_definitions( -DBOOST_ALL_DYN_LINK )
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT ${BOOST_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}")

find_package(ZLIB REQUIRED)
find_package(SimGear ${JIFF_VERSION} REQUIRED)

find_package(JSBSim REQUIRED)
link_libraries(${JSBSIM_LIBRARIES})


check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file(termios.h HAVE_TERMIOS_H)
check_include_file(pty.h HAVE_PTY_H)
check_include_file(utmp.h HAVE_UTMP_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(string.h HAVE_STRING_H)

add_definitions(-DHAVE_CONFIG_H)

include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${CMAKE_HOME_DIRECTORY}/src)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config_cmake.h.in"
  "${PROJECT_BINARY_DIR}/src/config.h"
  )


include_directories(${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
    ${SIMGEAR_INCLUDE_DIR}
	${JSBSIM_INCLUDE_DIRS})

add_subdirectory( src )

