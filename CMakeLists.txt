cmake_minimum_required(VERSION 3.5)

project(protoc_test LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
find_package(Protobuf REQUIRED)

if(MSVC)
    find_package(gRPC CONFIG REQUIRED)
    set(GRPC_LIBRARIES c-ares::cares gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    if(UNIX AND NOT APPLE)
        pkg_check_modules(GRPC REQUIRED grpc++ grpc)
        set(GRPC_LIBRARIES ${GRPC_LIBRARIES})
    else()
        find_library(UPB_LIBRARY NAMES upb)
        find_library(ADDRESS_SORTING NAMES address_sorting)
        pkg_check_modules(GRPC REQUIRED grpc++ grpc gpr)
        set(GRPC_LIBRARIES ${GRPC_LINK_LIBRARIES} ${UPB_LIBRARY} ${ADDRESS_SORTING})
    endif()
endif()

file(GLOB_RECURSE PROTO_FILES "v2ray.com/*.proto")

set(PROTO_DIR "${CMAKE_CURRENT_BINARY_DIR}/proto_gen")
set(PROTO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/v2ray.com/")
make_directory(${PROTO_DIR})

message("Protobuf Generated Dir: ${PROTO_DIR}")

foreach(proto ${PROTO_FILES})
    get_filename_component(PROTO_ABS_DIR_PATH ${proto} DIRECTORY)
    file(RELATIVE_PATH PROTO_RELATIVE_PATH ${PROTO_SOURCE_DIR} ${proto})
    #
    get_filename_component(PROTO_DIR_PATH ${PROTO_RELATIVE_PATH} DIRECTORY)
    get_filename_component(PROTO_NAME_PATH ${PROTO_RELATIVE_PATH} NAME_WE)
    #
    set(PROTO_HEADER_FILE      "${PROTO_DIR}/v2ray.com/${PROTO_DIR_PATH}/${PROTO_NAME_PATH}.pb.h")
    set(PROTO_SOURCE_FILE      "${PROTO_DIR}/v2ray.com/${PROTO_DIR_PATH}/${PROTO_NAME_PATH}.pb.cc")
    set(PROTO_GRPC_HEADER_FILE "${PROTO_DIR}/v2ray.com/${PROTO_DIR_PATH}/${PROTO_NAME_PATH}.grpc.pb.h")
    set(PROTO_GRPC_SOURCE_FILE "${PROTO_DIR}/v2ray.com/${PROTO_DIR_PATH}/${PROTO_NAME_PATH}.grpc.pb.cc")
    #
    list(APPEND PROTO_HEADERS ${PROTO_HEADER_FILE} ${PROTO_GRPC_HEADER_FILE})
    list(APPEND PROTO_SOURCES ${PROTO_SOURCE_FILE} ${PROTO_GRPC_SOURCE_FILE})
    #
    add_custom_command(
        OUTPUT "${PROTO_HEADER_FILE}" "${PROTO_SOURCE_FILE}" "${PROTO_GRPC_HEADER_FILE}" "${PROTO_GRPC_SOURCE_FILE}"
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS
            --grpc_out "${PROTO_DIR}"
            --cpp_out "${PROTO_DIR}"
            -I "${CMAKE_SOURCE_DIR}"
            --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
            "${proto}"
        DEPENDS "${proto}"
        )
endforeach()

include_directories(${PROTO_DIR})
add_compile_options(-Wno-deprecated-declarations)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

cmake_policy(SET CMP0071 OLD)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui REQUIRED)

add_executable(protoc_test
    main.cpp
    ${PROTO_SOURCES}
    ${PROTO_HEADERS}
    )


target_link_libraries(protoc_test Qt5::Core Qt5::Gui ${GRPC_LIBRARIES} protobuf)
