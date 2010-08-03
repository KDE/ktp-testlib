set (tp4_dbus_service_autostart_dir "${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/dbus-1/services/")
configure_file (session.conf.in ${CMAKE_CURRENT_BINARY_DIR}/_toInstall/session.conf)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/_toInstall/session.conf
         DESTINATION ${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}
         COMPONENT Devel
)

add_subdirectory(services)
