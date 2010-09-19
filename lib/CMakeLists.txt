include_directories (${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(glib)

set (ktelepathytest_SRCS
     test.cpp
     glibtest.cpp
     nepomuktest.cpp
)

set (ktelepathytest_HDRS
     Test
     test.h
     GLibTest
     glibtest.h
     NepomukTest
     nepomuktest.h
)

# Find telepathy.trig, which is installed already by telepathy-integration-daemon.
find_file (TELEPATHY_TRIG_SOURCE
           telepathy.trig
           PATHS "${DATA_INSTALL_DIR}" "${KDE4_DATA_DIR}" "${CMAKE_INSTALL_PREFIX}/share"
           ENV XDG_DATA_DIRS
           PATH_SUFFIXES "ontology/telepathy"
)

# We depend on telepathy.trig, so fail horribly if it's not found.${CMAKE_MODULE_PATH}
if (NOT TELEPATHY_TRIG_SOURCE)
    message (ERROR_FATAL
             "Could not find telepathy.trig Telepathy source file. You must install telepathy-integration-daemon before trying to build this program."
    )
endif (NOT TELEPATHY_TRIG_SOURCE)

# Add the ontologies we want to build the vocabulary for.
soprano_add_ontology (pimo_SRCS
                      ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/pimo/pimo.trig
                      "PIMO"
                      "Nepomuk::Vocabulary"
                      "trig"
)

soprano_add_ontology (nco_SRCS
                      ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nco.trig
                      "NCO"
                      "Nepomuk::Vocabulary"
                      "trig"
)

soprano_add_ontology (nao_SRCS
                      ${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nepomuk/nao.trig
                      "NAO"
                      "Nepomuk::Vocabulary"
                      "trig"
)

soprano_add_ontology (telepathy_SRCS
                      ${TELEPATHY_TRIG_SOURCE}
                      "Telepathy"
                      "Nepomuk::Vocabulary"
                      "trig"
)

kde4_add_library (ktelepathytest
                  STATIC
                  ${ktelepathytest_SRCS}
                  ${telepathy_SRCS}
                  ${pimo_SRCS}
                  ${nao_SRCS}
                  ${nco_SRCS}
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

install (FILES ${ktelepathytest_HDRS}
         DESTINATION ${INCLUDE_INSTALL_DIR}/KTelepathy/TestLib
         COMPONENT Devel
)

# Configure and install misc other files
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/KTelepathyTestLibMacros.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibMacros.cmake"
                IMMEDIATE @ONLY
)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/KTelepathyTestLibConfig.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibConfig.cmake"
                IMMEDIATE @ONLY
)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/nepomukserverrc.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nepomukserverrc"
                IMMEDIATE @ONLY
)


install (FILES python/account-manager.py
         DESTINATION ${KTELEPATHY_TESTLIB_PYTHON_INSTALL_DIR}
         COMPONENT Devel
)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibMacros.cmake"
         DESTINATION ${KTELEPATHY_TESTLIB_CMAKE_INSTALL_DIR}
         COMPONENT Devel
)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/KTelepathyTestLibConfig.cmake"
         DESTINATION ${KTELEPATHY_TESTLIB_CMAKE_INSTALL_DIR}
         COMPONENT Devel
)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/nepomukserverrc"
         DESTINATION ${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}
         COMPONENT Devel
)

