include_directories (${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
)

set (telepathy_testlib_SRCS
     test.cpp
)

set (telepathy_testlib_HDRS
     Test
     test.h
)

#if(ENABLE_TP_GLIB_TESTS)

    # Find telepathy-glib
    set(REQUIRED_TP_GLIB 0.10.0)
    pkg_check_modules(TPGLIB telepathy-glib>=${REQUIRED_TP_GLIB})

    include_directories(${TPGLIB_INCLUDE_DIRS})

    # Enable also the GLib wrappers
    add_subdirectory(glib)

    set(telepathy_testlib_SRCS ${telepathy_qt4_testlib_SRCS}
        glibtest.cpp)

    set(telepathy_testlib_HDRS ${telepathy_qt4_testlib_HEADERS}
        glibtest.h)

#endif(ENABLE_TP_GLIB_TESTS)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KTelepathyTestLibMacros.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibMacros.cmake"
    IMMEDIATE @ONLY)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KTelepathyTestLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibConfig.cmake"
    IMMEDIATE @ONLY)


kde4_add_library (ktelepathytest
                  STATIC
                  ${telepathy_testlib_SRCS}
)

target_link_libraries (ktelepathytest
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTDBUS_LIBRARY}
                       ${TELEPATHY_QT4_LIBRARIES}
                       ktelepathyglibtest
)

set_target_properties (ktelepathytest PROPERTIES
                       VERSION ${GENERIC_LIB_VERSION}
                       SOVERSION ${GENERIC_LIB_SOVERSION}
)

if (MSVC)
    set_target_properties (ktelepathytest PROPERTIES
                           OUTPUT_NAME libktelepathytest
    )
endif (MSVC)

# Install:
install (TARGETS ktelepathytest
         ${INSTALL_TARGETS_DEFAULT_ARGS}
         COMPONENT Devel
)

install (FILES ${telepathy_testlib__HDRS}
         DESTINATION ${INCLUDE_INSTALL_DIR}/KTelepathy/TestLib
         COMPONENT Devel
)

install(FILES python/account-manager.py
        DESTINATION ${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/python
        COMPONENT Devel
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibMacros.cmake"
        DESTINATION ${KDE4_LIB_INSTALL_DIR}/KTelepathy/cmake)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibConfig.cmake"
        DESTINATION ${KDE4_LIB_INSTALL_DIR}/KTelepathy/cmake)

