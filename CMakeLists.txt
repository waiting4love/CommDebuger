cmake_minimum_required(VERSION 3.20)

project(WTLCommDebuger LANGUAGES CXX RC)

# This project requires MSVC (Visual Studio)
if(NOT MSVC)
    message(FATAL_ERROR "This project requires MSVC (Visual Studio).")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
set(SOURCES
    WTLCommDebuger/MainDlg.cpp
    WTLCommDebuger/SerialPortInfo.cpp
    WTLCommDebuger/WTLCommDebuger.cpp
    WTLCommDebuger/stdafx.cpp
)

set(RC_SOURCES
    WTLCommDebuger/WTLCOMM.rc
)

# ---------------------------------------------------------------------------
# Target — WIN32 = subsystem windows (no console)
# ---------------------------------------------------------------------------
add_executable(WTLCommDebuger WIN32 ${SOURCES} ${RC_SOURCES})

# ---------------------------------------------------------------------------
# Include directories
# ---------------------------------------------------------------------------
target_include_directories(WTLCommDebuger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/WTLCommDebuger
    ${CMAKE_CURRENT_SOURCE_DIR}/WTLCommDebuger/Include
)

# ---------------------------------------------------------------------------
# Precompiled header (stdafx.h)
# ---------------------------------------------------------------------------
target_precompile_headers(WTLCommDebuger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/WTLCommDebuger/stdafx.h
)

# ---------------------------------------------------------------------------
# Compile definitions
# ---------------------------------------------------------------------------
target_compile_definitions(WTLCommDebuger PRIVATE
    UNICODE
    _UNICODE
    WIN32
    _WINDOWS
    STRICT
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# ---------------------------------------------------------------------------
# MSVC: use UTF-8 for source and execution character sets
# ---------------------------------------------------------------------------
target_compile_options(WTLCommDebuger PRIVATE /utf-8)

# ---------------------------------------------------------------------------
# Static runtime library: /MT for Release, /MTd for Debug
# ---------------------------------------------------------------------------
set_property(TARGET WTLCommDebuger PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# ---------------------------------------------------------------------------
# Link libraries
# ---------------------------------------------------------------------------
target_link_libraries(WTLCommDebuger PRIVATE shlwapi)

# ---------------------------------------------------------------------------
# Disable linker manifest generation — the manifest is embedded via the
# .rc file (1 RT_MANIFEST "WTLCommDebuger.manifest").
# ---------------------------------------------------------------------------
set_target_properties(WTLCommDebuger PROPERTIES
    LINK_FLAGS "/MANIFEST:NO"
)

# ---------------------------------------------------------------------------
# RC compiler: the .rc file is encoded in UTF-8 with BOM (auto-detected)
# ---------------------------------------------------------------------------
# No /c flag needed — the RC compiler auto-detects UTF-8 with BOM.

# ---------------------------------------------------------------------------
# Output directory: place binaries in <arch>/<config>/ for clarity
# ---------------------------------------------------------------------------
set_target_properties(WTLCommDebuger PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${CMAKE_VS_PLATFORM_NAME}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${CMAKE_VS_PLATFORM_NAME}/Release"
)
