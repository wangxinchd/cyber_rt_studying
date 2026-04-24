cmake_minimum_required(VERSION 3.5.1)

project(class02_test_callback)
set(CMAKE_CXX_STANDARD 14)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../code)

set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/examples/class02_test_callback/proto)
set(PROTO_OUT_DIR ${CMAKE_SOURCE_DIR}/examples/class02_test_callback/proto)

add_custom_command(
    OUTPUT ${PROTO_OUT_DIR}/class02.pb.cc
    COMMAND ${CMAKE_SOURCE_DIR}/env/bin/protoc -I=${PROTO_SRC_DIR} --cpp_out=${PROTO_OUT_DIR} ${PROTO_SRC_DIR}/class02.proto
    DEPENDS ${PROTO_SRC_DIR}/class02.proto
    COMMENT "Generating protobuf C++ code for class02.proto"
    VERBATIM
)

add_custom_target(generate_class02_proto ALL DEPENDS ${PROTO_OUT_DIR}/class02.pb.cc)

set(CYBER_EXAMPLES_PROTO_CLASS02_SRC
    ${PROTO_OUT_DIR}/class02.pb.cc
)

add_library(test_planning SHARED planning_component.cc ${CYBER_EXAMPLES_PROTO_CLASS02_SRC})
add_dependencies(test_planning generate_class02_proto)
target_link_libraries(test_planning cyber)

add_library(test_loc SHARED loc_component.cc ${CYBER_EXAMPLES_PROTO_CLASS02_SRC})
add_dependencies(test_loc generate_class02_proto)
target_link_libraries(test_loc cyber)

add_library(test_custom_planning SHARED custom_planning_component.cc ${CYBER_EXAMPLES_PROTO_CLASS02_SRC})
add_dependencies(test_custom_planning generate_class02_proto)
target_link_libraries(test_custom_planning cyber dl)

set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/../install)

install(TARGETS test_planning test_loc test_custom_planning
    ARCHIVE DESTINATION ${INSTALL_PATH}/examples/class02_test_callback/lib
    LIBRARY DESTINATION ${INSTALL_PATH}/examples/class02_test_callback/lib
    RUNTIME DESTINATION ${INSTALL_PATH}/examples/class02_test_callback/bin
)

file(GLOB DAG_FILES "dag/*.dag")
install(FILES ${DAG_FILES}
  DESTINATION ${INSTALL_PATH}/examples/class02_test_callback/dag
)

install(FILES conf/cyber.pb.conf
  DESTINATION ${INSTALL_PATH}/examples/class02_test_callback/conf
)

file(GLOB SCRIPT_FILES "scripts/*.sh")
install(FILES ${SCRIPT_FILES}
  DESTINATION ${INSTALL_PATH}/examples/class02_test_callback/scripts
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)
