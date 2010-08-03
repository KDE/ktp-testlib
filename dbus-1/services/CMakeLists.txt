set (path_to_account_manager_py ${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/python)
configure_file (account-manager.service.in ${CMAKE_BINARY_DIR}/account-manager.service)
configure_file (spurious.service.in ${CMAKE_CURRENT_BINARY_DIR}/spurious.service)


install (FILES ${CMAKE_BINARY_DIR}/account-manager.service
         DESTINATION ${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/dbus-1/services
         COMPONENT Devel
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/spurious.service
         DESTINATION ${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/dbus-1/services
         COMPONENT Devel
)

