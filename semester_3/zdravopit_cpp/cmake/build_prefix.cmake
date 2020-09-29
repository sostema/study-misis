# Determine the platform
# -----------------------------------------------------------------------
if (WIN32)
    set(PLATFORM_NAME "win")
elseif (UNIX)
    set(PLATFORM_NAME "unix")
else ()
    message(fatal_error "Unsupported platform")
endif ()

# Determine platform bits
# -----------------------------------------------------------------------
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PLATFORM_BITNESS "32")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM_BITNESS "64")
else ()
    message(fatal_error "Unsupported bitness")
endif ()

# Platform is the pair of system and its bitness
# -----------------------------------------------------------------------
set(PLATFORM "${PLATFORM_NAME}${PLATFORM_BITNESS}")
message("Platform is ${PLATFORM}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(COMPILER_ID "icl")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_ID "gcc")
elseif (MSVC14)
    set(COMPILER_ID "vc2015")
elseif (MSVC15)
    set(COMPILER_ID "vc2017")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_ID "Clang")
else ()
    set(COMPILER_ID "unknown")
endif ()
message("Compiler id is ${COMPILER_ID}")

set(PLATFORM "${PLATFORM}.${COMPILER_ID}")
message("Build suffix is ${PLATFORM}")

# Runtime output directory.
# -----------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin.${PLATFORM}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin.${PLATFORM}.dbg")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin.${PLATFORM}.rel")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/bin.${PLATFORM}.inforelease")