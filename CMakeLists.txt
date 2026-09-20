# =============================================================================
# tcxQr - TrussC QR Code Generator Addon
# =============================================================================
# Wraps Nayuki's qrcodegen (MIT), vendored unchanged in src/.
# Not header-only: qrcodegen.cpp has to be compiled in.

cmake_minimum_required(VERSION 3.16)

set(ADDON_NAME tcxQr)

file(GLOB ADDON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB ADDON_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_library(${ADDON_NAME} STATIC ${ADDON_SOURCES})

target_include_directories(${ADDON_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(${ADDON_NAME} PUBLIC cxx_std_17)

target_link_libraries(${ADDON_NAME} PUBLIC TrussC)
