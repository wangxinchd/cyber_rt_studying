cmake_minimum_required(VERSION 3.5.1)

project(class01_test_mainboard)
set(CMAKE_CXX_STANDARD 14)
include_directories(${CMAKE_SOURCE_DIR})

set(CYBER_EXAMPLES_PROTO_EXAMPLES_SRC ${CMAKE_SOURCE_DIR}/cyber/examples/proto/examples.pb.cc)
add_library(class01_test_mainboard_reader SHARED reader_component.cc ${CYBER_EXAMPLES_PROTO_EXAMPLES_SRC})
target_link_libraries(class01_test_mainboard_reader cyber )

add_library(class01_test_mainboard_writer SHARED writer_component.cc ${CYBER_EXAMPLES_PROTO_EXAMPLES_SRC})
target_link_libraries(class01_test_mainboard_writer cyber)


# install path
set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/../install)

install(TARGETS class01_test_mainboard_reader class01_test_mainboard_writer
    ARCHIVE DESTINATION ${INSTALL_PATH}/examples/class01_test_mainboard/lib
    LIBRARY DESTINATION ${INSTALL_PATH}/examples/class01_test_mainboard/lib
    RUNTIME DESTINATION ${INSTALL_PATH}/examples/class01_test_mainboard/bin
)

install(FILES dag/class01_test_mainboard_reader.dag dag/class01_test_mainboard_writer.dag
  DESTINATION ${INSTALL_PATH}/examples/class01_test_mainboard/dag
)
install(FILES conf/cyber.pb.conf
  DESTINATION ${INSTALL_PATH}/examples/class01_test_mainboard/conf
)
install(FILES scripts/start_class01_test_mainboard_reader_component.sh scripts/start_class01_test_mainboard_writer_component.sh scripts/start_class01_test_mainboard_reader_writer.sh
  DESTINATION ${INSTALL_PATH}/examples/class01_test_mainboard/scripts
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)