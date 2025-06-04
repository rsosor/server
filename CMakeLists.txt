cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0167 NEW) # Use the new method for finding Boost

project(game VERSION 0.1)

# vscode 整合 IntelliSense 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 指定 C++ 編譯器
set(CMAKE_CXX_COMPILER "C:/ProgramData/mingw64/bin/g++.exe")
# 指定 C 編譯器
set(CMAKE_C_COMPILER "C:/ProgramData/mingw64/bin/gcc.exe")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(BUILD_SHARED_LIBS OFF)

# 設置 vcpkg 的工具鏈文件路徑
set(CMAKE_TOOLCHAIN_FILE "C://Users/godpk/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
# include("C://Users/godpk/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 通知 vcpkg 使用靜態 mingw 版本
# set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")

set(Boost_ROOT "C://Users/godpk/vcpkg/installed/x64-windows")

# 設定 jsoncppConfig.cmake 的路徑
# llvm 版本
# set(jsoncpp_DIR "C://Users/godpk/vcpkg/installed/x64-windows/share/jsoncpp" CACHE PATH "Path to jsoncppConfig.cmake")
# mingw 版本
# set(jsoncpp_DIR "C://Users/godpk/vcpkg/installed/x64-mingw-static/share/jsoncpp" CACHE PATH "Path to jsoncppConfig.cmake")

# 設置根目錄，這是 vcpkg 的安裝目錄
# find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system)
# find_package(jsoncpp REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# 獲取 include 目錄
# get_target_property(JSONCPP_INCLUDE_DIRS JsonCpp::JsonCpp INTERFACE_INCLUDE_DIRECTORIES)

# 確保目標存在
# if(NOT JSONCPP_INCLUDE_DIRS)
#     message(FATAL_ERROR "JsonCpp::JsonCpp target not found. Check jsoncpp installation.")
# endif()

# Linux 上，jsoncpp 可能已透過 pkg-config 註冊，因此可以這樣尋找：
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(JSONCPP jsoncpp)
# 這樣 pkg_check_modules(JSONCPP jsoncpp) 會自動填入 JSONCPP_INCLUDE_DIRS 和 JSONCPP_LIBRARIES，讓 CMake 使用。
# link_libraries(${JSONCPP_LIBRARIES})

# Find Boost headers and some static (release only) libraries:
# set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
# set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
# set(Boost_USE_MULTITHREADED      ON)
# set(Boost_USE_STATIC_RUNTIME    OFF)
# set(CMAKE_VERBOSE_MAKEFILE       ON)  # 編譯時顯示更多的詳細訊息

if(Boost_FOUND)
  message(STATUS "Boost include path: ${Boost_INCLUDE_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(STATUS "Boost NOT Found !")
endif(Boost_FOUND)

if(nlohmann_json_FOUND)
  message(STATUS "nlohmann_json include path: ${nlohmann_json_INCLUDE_DIRS}")
  # include_directories(${nlohmann_json_INCLUDE_DIRS})
else()
  message(STATUS "nlohmann_json NOT Found !")
endif(nlohmann_json_FOUND)

# x64-windows：MSVC 編譯器使用的 Windows 靜態或動態庫
# link_directories("C:/Users/godpk/vcpkg/installed/x64-windows/lib" "C:/Users/godpk/vcpkg/installed/x64-windows/bin")
# x64-mingw-static：MinGW 編譯器使用的靜態版本
# link_directories("C:/Users/godpk/vcpkg/installed/x64-mingw-static/lib")


# file(GLOB SOURCES "src/*/*.cpp")

# 收集 source 檔案
file(GLOB GAME_SERVER_SOURCES "src/*/*.cpp")

# 建立可執行檔
add_executable(${PROJECT_NAME} 
src/main.cpp
${GAME_SERVER_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
# header-only 使用：#include <boost/algorithm/string.hpp>
# target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} Boost::Boost)
# target_link_libraries(${PROJECT_NAME} Boost::)
# 靜態連結與動態連結版本，靜態則需要到 vcpkg 下載靜態版本：vcpkg install jsoncpp:x64-windows-static
# target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp_static)
# target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)
# target_link_libraries(${PROJECT_NAME} PRIVATE Boost::Boost JsonCpp::JsonCpp)
# Boost components
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::filesystem Boost::system)
# JSON support
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
