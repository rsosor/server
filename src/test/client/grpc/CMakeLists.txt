cmake_minimum_required(VERSION 3.20)
project(game_client CXX)

set(CMAKE_BUILD_TYPE Debug)

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

set(Boost_ROOT "C://Users/godpk/vcpkg/installed/x64-windows")
set(nlohmann_json_DIR "C://Users/godpk/vcpkg/installed/x64-windows/share/nlohmann_json")

find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

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

# 收集 source 檔案
# file(GLOB_RECURSE GAME_CLIENT_SOURCES "src/*/*.cpp")
file(GLOB_RECURSE GAME_CLIENT_SOURCES_CPP "src/**/*.cpp")
file(GLOB_RECURSE GAME_CLIENT_SOURCES_CC "src/**/*.cc")

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

# Boost components
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::filesystem Boost::system)
# JSON
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

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
