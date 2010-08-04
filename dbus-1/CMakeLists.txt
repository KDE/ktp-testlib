configure_file (session.conf.in "${CMAKE_CURRENT_BINARY_DIR}/session.conf")

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/session.conf
         DESTINATION ${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}
         COMPONENT Devel
)

add_subdirectory(services)
