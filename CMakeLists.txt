#cmake file for ev server
#author changfa.zheng

cmake_minimum_required(VERSION 3.1.0)


#project name
PROJECT(config-lib)


set(EKV_VERSION_MAJOR 1)
set(EKV_VERSION_MINOR 1)
set(EKV_VERSION_FIXED 0)
string(TIMESTAMP EKV_BUILD_TIME "%Y-%m-%d %H:%M")
execute_process(COMMAND git rev-parse HEAD OUTPUT_VARIABLE EKV_GIT_HASH)
message(STATUS "\tversion: ${EKV_VERSION_MAJOR}.${EKV_VERSION_MINOR}.${EKV_VERSION_FIXED}\n\tgit hash: ${EKV_GIT_HASH}\tbuild time: ${EKV_BUILD_TIME}")

set(THIRD_PARTH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

configure_file (
		"${PROJECT_SOURCE_DIR}/version.h.in"
		"${PROJECT_BINARY_DIR}/version.h"
		)


add_subdirectory(fmt)

#add_custom_command(OUTPUT  ekv.pb.h ekv.pb.cc ekv.grpc.pb.h ekv.grpc.pb.cc
#  COMMAND ${PROTOBUF_PATH}/bin/protoc ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/ekv.proto -I${CMAKE_CURRENT_SOURCE_DIR}
#  COMMAND ${PROTOBUF_PATH}/bin/protoc ARGS --grpc_out . --plugin=protoc-gen-grpc=${THIRD_PARTH_DIR}/grpc/bin/grpc_cpp_plugin ${CMAKE_CURRENT_SOURCE_DIR}/ekv.proto -I${CMAKE_CURRENT_SOURCE_DIR}
#  DEPENDS ekv.proto
#  VERBATIM )
#add_custom_target(ekvserver_proto DEPENDS  ekv.pb.h ekv.pb.cc ekv.grpc.pb.h ekv.grpc.pb.cc)
#
#add_custom_command(OUTPUT ps.pb.cc ps.pb.h ps.grpc.pb.h ps.grpc.pb.cc 
#  COMMAND ${PROTOBUF_PATH}/bin/protoc ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/ps.proto -I${CMAKE_CURRENT_SOURCE_DIR}
#  COMMAND ${PROTOBUF_PATH}/bin/protoc ARGS --grpc_out . --plugin=protoc-gen-grpc=${THIRD_PARTH_DIR}/grpc/bin/grpc_cpp_plugin ${CMAKE_CURRENT_SOURCE_DIR}/ps.proto -I${CMAKE_CURRENT_SOURCE_DIR}
#  DEPENDS ps.proto
#  VERBATIM )
#add_custom_target(ekvps_proto DEPENDS ps.pb.cc ps.pb.h ps.grpc.pb.h ps.grpc.pb.cc)

set(CMAKE_CXX_COMPILER "g++")
#SET(CMAKE_CXX_FLAGS "-std=c++11 -lc++experimental")
SET(CMAKE_CXX_FLAGS "-std=c++14")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_BUILD_TYPE DEBUG)

#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)


#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/rpc)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
#LINK_DIRECTORIES(${LIBRARY_OUTPUT_DIRECTORY})


#third_party include 
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/fmt/include
)
#third_party lib 
LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/fmt)


#获得一个目录下所有源文件（不包括头文件）到变量SRCS
#AUX_SOURCE_DIRECTORY(dirname SRCS)
#AUX_SOURCE_DIRECTORY(./ SOURCE_FILES)


#生成静态链接库libname.a
#ADD_LIBRARY(name $SOURCE_FILES)


ADD_EXECUTABLE(config-test
  main.cc 
  src/config.cc
  src/config_file.cc
  src/log.cc
  src/posix.cc
  src/backtrace.cc
  src/program-options.cc
 )

add_dependencies(config-test fmt)

#依赖的库文件

TARGET_LINK_LIBRARIES(config-test
  -lboost_program_options -lboost_system -lboost_filesystem
  -lyaml-cpp
  -lpthread
  libfmt.a)

