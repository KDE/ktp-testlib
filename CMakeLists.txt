project (telepathy-testlib)

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

set(KDE_MIN_VERSION "4.4.75")
find_package (KDE4 4.4.75 REQUIRED)
find_package (TelepathyQt4 REQUIRED)
find_package (Nepomuk REQUIRED)
find_package (Soprano REQUIRED)

# Find python version >= 2.5
find_package(PythonLibrary REQUIRED)
set(REQUIRED_PY 2.5)
if(${PYTHON_SHORT_VERSION} VERSION_GREATER ${REQUIRED_PY} OR ${PYTHON_SHORT_VERSION} VERSION_EQUAL ${REQUIRED_PY})
    message(STATUS "Python ${PYTHON_SHORT_VERSION} found")
else(${PYTHON_SHORT_VERSION} VERSION_GREATER ${REQUIRED_PY} OR ${PYTHON_SHORT_VERSION} VERSION_EQUAL ${REQUIRED_PY})
    message(SEND_ERROR "Python >= ${REQUIRED_PY} is required")
endif(${PYTHON_SHORT_VERSION} VERSION_GREATER ${REQUIRED_PY} OR ${PYTHON_SHORT_VERSION} VERSION_EQUAL ${REQUIRED_PY})

# Check for dbus-python
# FIXME: Python is required in the testlib
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import dbus.mainloop.glib"
                RESULT_VARIABLE PYTHON_DBUS_RESULT)
if(PYTHON_DBUS_RESULT EQUAL 0)
    message(STATUS "dbus-python installed, running additional tests")
    set(HAVE_TEST_PYTHON 1)
else(PYTHON_DBUS_RESULT EQUAL 0)
    message(STATUS "dbus-python not installed, skipping some tests")
    set(HAVE_TEST_PYTHON 0)
endif(PYTHON_DBUS_RESULT EQUAL 0)

include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${KDE4_DEFINITIONS}
                 -DDISABLE_NEPOMUK_LEGACY
                 -DQT_NO_CAST_FROM_ASCII
)

include_directories (${KDE4_INCLUDES}
                     ${TELEPATHY_QT4_INCLUDE_DIR}
                     ${NEPOMUK_INCLUDES}
)

set (KTELEPATHY_TESTLIB_DATA_INSTALL_DIR
     "${KDE4_DATA_INSTALL_DIR}/KTelepathy/TestLib"
)

set (KTELEPATHY_TESTLIB_DBUS_SERVICE_AUTOSTART_DIR
     "${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/dbus-1/services/"
)

add_subdirectory (lib)
add_subdirectory (dbus-1)

