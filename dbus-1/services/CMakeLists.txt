configure_file (account-manager.service.in ${CMAKE_CURRENT_BINARY_DIR}/account-manager.service)
configure_file (spurious.service.in ${CMAKE_CURRENT_BINARY_DIR}/spurious.service)


install (FILES ${CMAKE_CURRENT_BINARY_DIR}/account-manager.service
         DESTINATION ${KTELEPATHY_TESTLIB_DBUS_SERVICE_AUTOSTART_INSTALL_DIR}
         COMPONENT Devel
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/spurious.service
         DESTINATION ${KTELEPATHY_TESTLIB_DBUS_SERVICE_AUTOSTART_INSTALL_DIR}
         COMPONENT Devel
)

