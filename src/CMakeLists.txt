cmake_minimum_required(VERSION 3.15)

project(miata-gate)

set(PLATFORMIO_CMD ~/.platformio/penv/bin/platformio)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib/Helpers)

include_directories(${CMAKE_SOURCE_DIR}/.pio/libdeps/nanoatmega328)
include_directories(${CMAKE_SOURCE_DIR}/.pio/libdeps/ArduinoBLE)
include_directories(${CMAKE_SOURCE_DIR}/.pio/libdeps/ArxContainer)
include_directories(${CMAKE_SOURCE_DIR}/.pio/libdeps/ArxTypeTraits)
include_directories(${CMAKE_SOURCE_DIR}/.pio/libdeps/DebugLog)

include_directories("${PROJECT_SOURCE_DIR}/inc/")
add_executable(${EXECUTABLE_NAME} main.cpp ${SOURCE})

add_custom_target(upload ALL
        COMMAND ${PLATFORMIO_CMD} run -t upload
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Uploading firmware to Arduino..."
)
