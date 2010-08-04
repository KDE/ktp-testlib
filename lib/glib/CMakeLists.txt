include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

# FIXME: Re-enable or remove these subdirs depending on whether they are necessary.
#   add_subdirectory(callable)
#   add_subdirectory(contactlist)
#   add_subdirectory(csh)
#   add_subdirectory(echo)
#   add_subdirectory(echo2)
#   add_subdirectory(future)

set (ktelepathyglibtests_SRCS
     contacts-conn.c
     contacts-conn.h
     debug.h
     simple-conn.c
     simple-conn.h
     simple-manager.c
     simple-manager.h
)

kde4_add_library (ktelepathyglibtests STATIC
                  ${ktelepathyglibtests_SRCS}
)

target_link_libraries (ktelepathyglibtests
                       ${TELEPATHY_GLIB_LIBRARIES}
                       ${DBUS_GLIB_LIBRARIES}
                       ${GOBJECT_LIBRARIES}
                       ${GLIB2_LIBRARIES}
                       # example-cm-callable
                       # example-cm-contactlist
                       # example-cm-csh
                       # example-cm-echo
                       # example-cm-echo2
                       # tp-glib-tests-future-extensions
                       # future-example-cm-call
                       # future-example-cm-conference
)

install (TARGETS ktelepathyglibtests
         ${INSTALL_TARGETS_DEFAULT_ARGS}
         COMPONENT Devel
)

