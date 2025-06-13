cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0167 NEW) # Use the new method for finding Boost

project(game VERSION 0.1)

# include(cmake/common.cmake)

#
# ---------- vscode ----------
#
# vscode 整合 IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#
# ---------- vscode ----------
#

#
# ---------- Compiler Options Start ----------
#
# 指定 C++ 編譯器
# set(CMAKE_CXX_COMPILER "C:/ProgramData/mingw64/bin/g++.exe")
# 指定 C 編譯器
# set(CMAKE_C_COMPILER "C:/ProgramData/mingw64/bin/gcc.exe")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 開啟編譯器參數 -source-charset:utf-8
# add_compile_options(/source-charset:utf-8 /execution-charset:utf-8)
add_compile_definitions(_WIN32_WINNT=0x0601)
#
# ---------- Compiler Options End ----------
#

# set(BUILD_SHARED_LIBS OFF)

#
# ---------- vcpkg start ----------
#
# 設置 vcpkg 的工具鏈文件路徑
set(CMAKE_TOOLCHAIN_FILE "C://Users/godpk/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
# include("C://Users/godpk/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 通知 vcpkg 使用靜態 mingw 版本
# set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")
#
# ---------- vcpkg end ----------
#

set(Boost_ROOT "C://Users/godpk/vcpkg/installed/x64-windows")
set(nlohmann_json_DIR "C://Users/godpk/vcpkg/installed/x64-windows/share/nlohmann_json")
set(protobuf_DIR "C://Users/godpk/vcpkg/installed/x64-windows/share/protobuf")
set(gRPC_DIR "C://Users/godpk/vcpkg/installed/x64-windows/share/grpc")

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
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

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
  include_directories(${nlohmann_json_INCLUDE_DIRS})
else()
  message(STATUS "nlohmann_json NOT Found !")
endif(nlohmann_json_FOUND)

#
# ---------- gRPC Start ----------
#
# set(PROTO_FILES 
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/proto/game.proto
# )

# set(GENERATED_DIR "${DCMAKE_SOURCE_DIR}/src/generated")

# add_library(myproto ${PROTO_FILES})
# target_link_libraries(myproto
#   PUBLIC
#     protobuf::libprotobuf
#     gRPC::grpc
#     gRPC::grpc++
# )
# myproto 這個 target 跟使用 myproto 的 target 都會用到這個 include 路徑（PUBLIC）。
# target_include_directories(myproto PUBLIC ${GENERATED_DIR})

# 產生 protobuf C++ code
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# 產生 gRPC C++ code
# grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_FILES})
#
# ---------- gRPC End ----------
#

# x64-windows：MSVC 編譯器使用的 Windows 靜態或動態庫
# link_directories("C:/Users/godpk/vcpkg/installed/x64-windows/lib" "C:/Users/godpk/vcpkg/installed/x64-windows/bin")
# x64-mingw-static：MinGW 編譯器使用的靜態版本
# link_directories("C:/Users/godpk/vcpkg/installed/x64-mingw-static/lib")

# 收集 source 檔案
# file(GLOB_RECURSE GAME_SERVER_SOURCES "src/*/*.cpp")
file(GLOB GAME_SERVER_SOURCES_CPP "src/*/*.cpp")
file(GLOB GAME_SERVER_SOURCES_CC "src/*/*.cc")
# 合併兩個列表
set(GAME_SERVER_SOURCES ${GAME_SERVER_SOURCES_CPP} ${GAME_SERVER_SOURCES_CC})

# 過濾掉任何路徑中包含 test
list(FILTER GAME_SERVER_SOURCES EXCLUDE REGEX ".*/test/.*")
# list(FILTER GAME_SERVER_SOURCES EXCLUDE REGEX ".*/(test|grpc)/.*")

message(STATUS "GAME_SERVER_SOURCES files: ${GAME_SERVER_SOURCES}")
message(STATUS "Proto generated files: ${GAME_SERVER_SOURCES_CC}")

# 建立可執行檔
add_executable(${PROJECT_NAME} 
  src/main.cpp
  ${GAME_SERVER_SOURCES}
  # ${PROTO_SRCS}
  # ${GRPC_SRCS}
)

# 只有 ${PROJECT_NAME} 這個 target 自己會用到這個 include 路徑（PRIVATE）。
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/generated")
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
# JSON
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
# gRPC、protobuf
target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite gRPC::gpr gRPC::grpc++ gRPC::grpc++_alts)
# ws2_32：
  # bind()、listen()、accept() 等 socket 函式都屬於 Windows 的 ws2_32.lib，在 GCC (MinGW) 下必須加上 -lws2_32
  # 不然會出現沒有連結 Windows Socket 函式庫的錯誤
  # __imp_bind： Windows 上 DLL 函式的符號通常會以 __imp_ 開頭，表示這是從 DLL 匯入的函式（imported symbol）
# mswsock：
  # Boost.Asio 使用 Windows 的高效能 I/O 函式 AcceptEx（用於 IOCP 模式），但你的程式沒有正確連結 Mswsock.lib（MinGW（GCC 系列） 下為 -lmswsock、MSVC 下為 mswsock.lib）
target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 mswsock)
