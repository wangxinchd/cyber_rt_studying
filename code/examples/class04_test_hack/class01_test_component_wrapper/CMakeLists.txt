cmake_minimum_required(VERSION 3.5.1)

project(class04_class01_test_component_wrapper)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../code)

set(PROTO_DIR ${CMAKE_SOURCE_DIR}/examples/class04_test_hack/class01_test_component_wrapper/proto)
file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")

set(PROTO_SRCS "")
set(PROTO_HDRS "")

foreach(proto ${PROTO_FILES})
    get_filename_component(name ${proto} NAME_WE)

    set(cc_file "${PROTO_DIR}/${name}.pb.cc")
    set(h_file  "${PROTO_DIR}/${name}.pb.h")

    list(APPEND PROTO_SRCS ${cc_file})
    list(APPEND PROTO_HDRS ${h_file})

    add_custom_command(
        OUTPUT ${cc_file} ${h_file}
        COMMAND ${CMAKE_SOURCE_DIR}/env/bin/protoc
                -I=${PROTO_DIR}
                --cpp_out=${PROTO_DIR}
                ${proto}
        DEPENDS ${proto}
        COMMENT "Generating ${name}.pb.cc"
    )
endforeach()

# ✅ 关键：proto 单独做一个库
add_library(${PROJECT_NAME}_proto_lib ${PROTO_SRCS})
target_include_directories(${PROJECT_NAME}_proto_lib PUBLIC ${PROTO_DIR})

# -------------------------
# planning
# -------------------------
add_library(class04_class01_test_component_wrapper_planning SHARED planning_component.cc)
target_link_libraries(class04_class01_test_component_wrapper_planning cyber ${PROJECT_NAME}_proto_lib)

set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/../install)

install(TARGETS class04_class01_test_component_wrapper_planning
    ARCHIVE DESTINATION ${INSTALL_PATH}/examples/class04_test_hack/class01_test_component_wrapper/lib
    LIBRARY DESTINATION ${INSTALL_PATH}/examples/class04_test_hack/class01_test_component_wrapper/lib
    RUNTIME DESTINATION ${INSTALL_PATH}/examples/class04_test_hack/class01_test_component_wrapper/bin
)

file(GLOB DAG_FILES "dag/*.dag")
install(FILES ${DAG_FILES}
  DESTINATION ${INSTALL_PATH}/examples/class04_test_hack/class01_test_component_wrapper/dag
)

install(FILES conf/cyber.pb.conf
  DESTINATION ${INSTALL_PATH}/examples/class04_test_hack/class01_test_component_wrapper/conf
)

file(GLOB SCRIPT_FILES "scripts/*.sh")
install(FILES ${SCRIPT_FILES}
  DESTINATION ${INSTALL_PATH}/examples/class04_test_hack/class01_test_component_wrapper/scripts
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)
