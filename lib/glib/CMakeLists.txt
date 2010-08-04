include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${TPGLIB_INCLUDE_DIRS})

#if(ENABLE_TP_GLIB_TESTS)
    add_definitions(-fPIC)

 # FIXME: Re-enable or remove these subdirs depending on whether they are necessary.
 #   add_subdirectory(callable)
 #   add_subdirectory(contactlist)
 #   add_subdirectory(csh)
 #   add_subdirectory(echo)
 #   add_subdirectory(echo2)
 #   add_subdirectory(future)

    set(tp_glib_tests_SRCS
        contacts-conn.c
        contacts-conn.h
        debug.h
        simple-conn.c
        simple-conn.h
        simple-manager.c
        simple-manager.h
)
    kde4_add_library(ktelepathyglibtests SHARED ${tp_glib_tests_SRCS})
    target_link_libraries(ktelepathyglibtests
                          ${TPGLIB_LIBRARIES}
                          glib-2.0
                          gobject-2.0
                          dbus-glib-1
                         # example-cm-callable
                         # example-cm-contactlist
                         # example-cm-csh
                         # example-cm-echo
                         # example-cm-echo2
                         # tp-glib-tests-future-extensions
                         # future-example-cm-call
                         # future-example-cm-conference
)

    install(TARGETS ktelepathyglibtests DESTINATION ${KDE4_LIB_INSTALL_DIR})
#endif(ENABLE_TP_GLIB_TESTS)
