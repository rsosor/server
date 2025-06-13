cmake_minimum_required(VERSION 3.20)
project(game_client CXX)

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
add_compile_definitions(_WIN32_WINNT=0x0601)
#
# ---------- Compiler Options End ----------
#

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

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

# 收集 source 檔案
# file(GLOB_RECURSE GAME_SERVER_SOURCES "src/*/*.cpp")
file(GLOB GAME_CLIENT_SOURCES_CPP "src/*/*.cpp")
file(GLOB GAME_CLIENT_SOURCES_CC "src/*/*.cc")
# 合併兩個列表
set(GAME_CLIENT_SOURCES ${GAME_CLIENT_SOURCES_CPP} ${GAME_CLIENT_SOURCES_CC})

# 過濾掉任何路徑中包含 test
list(FILTER GAME_CLIENT_SOURCES EXCLUDE REGEX ".*/client/grpc/src/test/.*")
# list(FILTER GAME_CLIENT_SOURCES EXCLUDE REGEX ".*/client/grpc/src/(test|grpc)/.*")

message(STATUS "GAME_CLIENT_SOURCES files: ${GAME_CLIENT_SOURCES}")
message(STATUS "Proto generated files: ${GAME_CLIENT_SOURCES_CC}")

# 建立可執行檔
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${GAME_CLIENT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/generated")

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    protobuf::libprotobuf
    gRPC::grpc++
    absl::synchronization
    absl::time
    absl::strings
    absl::status
    absl::base
    # 依需求加入其他 absl lib
    ws2_32.lib  # Windows socket library
)
