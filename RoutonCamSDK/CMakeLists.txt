cmake_minimum_required(VERSION 3.5)

project(RoutonCamSDK LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# c++11编译
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(PREJECT_ROOT_PATH ${PROJECT_SOURCE_DIR})

set(RUN_ENV env_x86_64_linux)

if( "${RUN_ENV}" STREQUAL "env_x86_64_linux")

    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)

elseif("${RUN_ENV}" STREQUAL "env_x86_64_linux_rk1109")

    SET(CMAKE_SYSROOT ${PREJECT_ROOT_PATH}/../rk/sysroot)

    set(SYSROOT_INC_PATH ${PREJECT_ROOT_PATH}/../rk/sysroot/usr/include)
    set(SYSROOT_LIB_PATH ${PREJECT_ROOT_PATH}/../rk/sysroot)
    set(SYSROOT_USR_LIB_PATH ${PREJECT_ROOT_PATH}/../rk/sysroot/usr/lib)

    include_directories(${SYSROOT_INC_PATH})
    link_directories(${SYSROOT_LIB_PATH})
    link_directories(${SYSROOT_USR_LIB_PATH})
    
    set(CMAKE_C_COMPILER ${PREJECT_ROOT_PATH}/../rk/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER ${PREJECT_ROOT_PATH}/../rk/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)

    set(COMPILER_LIB_PATH ${PREJECT_ROOT_PATH}/../rk/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/arm-linux-gnueabihf/libc/lib/)
    link_directories(${COMPILER_LIB_PATH})

elseif("${RUN_ENV}" STREQUAL "env_x86_64_window")
    
endif()

set(HIKVISION_SDK_INC_PATH ${PREJECT_ROOT_PATH}/include/)
set(HIKVISION_SDK_LIB_PATH ${PREJECT_ROOT_PATH}/lib/)
include_directories(${HIKVISION_SDK_INC_PATH})
link_directories(${HIKVISION_SDK_LIB_PATH})

link_libraries(hcnetsdk PlayCtrl AudioRender SuperRender)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)

aux_source_directory(. DIR_SRCS)

# 创建共享库
add_library(RoutonCamSDK SHARED ${DIR_SRCS})



#target_compile_definitions(RoutonCamSDK PRIVATE ROUTONCAMSDK_LIBRARY)
