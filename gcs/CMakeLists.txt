#
# FALSA Model Problem
# 
# Copyright 2024 Carnegie Mellon University.
# 
# NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
# INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
# UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, AS
# TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE
# OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
# MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
# WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
# 
# Licensed under a MIT (SEI)-style license, please see license.txt or contact
# permission@sei.cmu.edu for full terms.
# 
# [DISTRIBUTION STATEMENT A] This material has been approved for public
# release and unlimited distribution.  Please see Copyright notice for non-US
# Government use and distribution.
# 
# This Software includes and/or makes use of Third-Party Software each subject
# to its own license.
# 
# DM24-0251
#

# Minimum CMake required
cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Project
project(payload)


#Mac Libraries
if(APPLE)
  list(APPEND CMAKE_PREFIX_PATH
    "/usr/local"
    "~/install/vcpkg/installed/arm64-osx"
    "~/install/poco/PocoInstall"
  )
endif()


INCLUDE_DIRECTORIES(../interfaces ../utils .)


#MAVSDK
# find_package(MAVSDK REQUIRED)
# INCLUDE_DIRECTORIES(~/MAVSDK/install/include/ ../interfaces .)
# LINK_DIRECTORIES(~/MAVSDK/install/lib)


#Poco
INCLUDE_DIRECTORIES(/usr/include/Poco/)
LINK_DIRECTORIES(/usr/local/lib)



# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# GCS
# Generated sources

get_filename_component(hw_proto1 "../protos/Time.proto" ABSOLUTE)
get_filename_component(hw_proto_path1 "${hw_proto1}" PATH)
set(hw_proto_srcs1 "${CMAKE_CURRENT_BINARY_DIR}/Time.pb.cc")
set(hw_proto_hdrs1 "${CMAKE_CURRENT_BINARY_DIR}/Time.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs1}" "${hw_proto_hdrs1}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path1}"
        "${hw_proto1}"
        DEPENDS "${hw_proto1}")

get_filename_component(hw_proto2 "../protos/LatLonCoord.proto" ABSOLUTE)
get_filename_component(hw_proto_path2 "${hw_proto2}" PATH)
set(hw_proto_srcs2 "${CMAKE_CURRENT_BINARY_DIR}/LatLonCoord.pb.cc")
set(hw_proto_hdrs2 "${CMAKE_CURRENT_BINARY_DIR}/LatLonCoord.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs2}" "${hw_proto_hdrs2}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path2}"
        "${hw_proto2}"
        DEPENDS "${hw_proto2}")

get_filename_component(hw_proto3 "../protos/Waypoint.proto" ABSOLUTE)
get_filename_component(hw_proto_path3 "${hw_proto3}" PATH)
set(hw_proto_srcs3 "${CMAKE_CURRENT_BINARY_DIR}/Waypoint.pb.cc")
set(hw_proto_hdrs3 "${CMAKE_CURRENT_BINARY_DIR}/Waypoint.pb.h")
add_custom_command(
        OUTPUT "${hw_proto_srcs3}" "${hw_proto_hdrs3}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS
          --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
          -I "${hw_proto_path3}"
          "${hw_proto3}"
          DEPENDS "${hw_proto3}")

get_filename_component(hw_proto "../protos/IMissionManager.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/IMissionManager.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/IMissionManager.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/IMissionManager.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/IMissionManager.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Targets (payloadapp)
foreach(_target
  gcsapp)
  add_executable(${_target} "${_target}.cc"
    ${hw_proto_srcs1}
    ${hw_proto_srcs2}
    ${hw_proto_srcs3}
    ${hw_proto_srcs}
    ${hw_grpc_srcs}
    client.cc
    )
  target_link_libraries(${_target}
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    PocoFoundation PocoNet PocoUtil PocoJSON
    )
endforeach()
