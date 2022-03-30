cmake_minimum_required(VERSION 3.10.2)

project(RoutonCamTest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)

# set preject path
SET(PREJECT_ROOT_PATH ${PROJECT_SOURCE_DIR})
message("PREJECT_ROOT_PATH: " ${PREJECT_ROOT_PATH})

# set include path
set(CAMSDK_INC_PATH ${PREJECT_ROOT_PATH}/include)
set(CAMSDK_LIB_PATH ${PREJECT_ROOT_PATH}/lib)

set(OPENCV_INC_PATH ${PREJECT_ROOT_PATH}/include/opencv)
set(OPENCV_LIB_PATH ${PREJECT_ROOT_PATH}/lib/opencv)

SET(HIKVISION_LIB_PATH ${PREJECT_ROOT_PATH}/lib/hikvision)

include_directories(${CAMSDK_INC_PATH} ${OPENCV_INC_PATH})
link_directories(${CAMSDK_LIB_PATH} ${OPENCV_LIB_PATH} ${HIKVISION_LIB_PATH})

link_libraries(opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui RoutonCamSDK)

aux_source_directory(. SRC_FILE)

set(EXECUTABLE_OUTPUT_PATH ${PREJECT_ROOT_PATH}/bin)

add_executable(RoutonCamTest
    ${SRC_FILE}
)
