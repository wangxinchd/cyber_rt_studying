cmake_minimum_required(VERSION 3.5.1)

project(class03_class02_component_one_input)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../code)


set(PROTO_DIR ${CMAKE_SOURCE_DIR}/examples/class03_different_component/class02_component_one_input/proto)

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
add_library(test_class02_planning SHARED planning_component.cc)
target_link_libraries(test_class02_planning cyber ${PROJECT_NAME}_proto_lib)

# -------------------------
# localization
# -------------------------
add_library(test_class02_loc SHARED loc_component.cc)
target_link_libraries(test_class02_loc cyber ${PROJECT_NAME}_proto_lib)

set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/../install)

install(TARGETS test_class02_planning test_class02_loc
    ARCHIVE DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class02_component_one_input/lib
    LIBRARY DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class02_component_one_input/lib
    RUNTIME DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class02_component_one_input/bin
)

file(GLOB DAG_FILES "dag/*.dag")
install(FILES ${DAG_FILES}
  DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class02_component_one_input/dag
)

install(FILES conf/cyber.pb.conf
  DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class02_component_one_input/conf
)

file(GLOB SCRIPT_FILES "scripts/*.sh")
install(FILES ${SCRIPT_FILES}
  DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class02_component_one_input/scripts
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)
