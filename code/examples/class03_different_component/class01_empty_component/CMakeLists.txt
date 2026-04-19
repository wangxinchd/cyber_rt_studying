cmake_minimum_required(VERSION 3.5.1)

project(class03_class01_empty_component)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../code)


set(PROTO_DIR ${CMAKE_SOURCE_DIR}/examples/class03_different_component/class01_empty_component/proto)

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
add_library(test_class03_planning SHARED planning_component.cc)
target_link_libraries(test_class03_planning cyber ${PROJECT_NAME}_proto_lib)

# -------------------------
# localization
# -------------------------
add_library(test_class03_loc SHARED loc_component.cc)
target_link_libraries(test_class03_loc cyber ${PROJECT_NAME}_proto_lib)

set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/../install)

install(TARGETS test_class03_planning test_class03_loc
    ARCHIVE DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class01_empty_component/lib
    LIBRARY DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class01_empty_component/lib
    RUNTIME DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class01_empty_component/bin
)

install(FILES dag/class03_planning.dag dag/class03_loc.dag
  DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class01_empty_component/dag
)
install(FILES conf/cyber.pb.conf
  DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class01_empty_component/conf
)
install(FILES scripts/start_class03_planning_component.sh scripts/start_class03_loc_component.sh scripts/start_class03_planning_loc.sh
  DESTINATION ${INSTALL_PATH}/examples/class03_different_component/class01_empty_component/scripts
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)
