project (ktelepathytestlib)

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

# Set minimum versions of packages we need
set (KDE_MIN_VERSION "4.4.75")
set (SDO_MIN_VERSION "0.3.60")
set (TELEPATHYQT4_MIN_VERSION "0.3.2")
set (TELEPATHYGLIB_MIN_VERSION "0.10.0")

# Find all the required packages
find_package (KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package (TelepathyQt4 ${TELEPATHYQT4_MIN_VERSION} REQUIRED)
find_package (Nepomuk REQUIRED)
find_package (Soprano REQUIRED)
find_package (SharedDesktopOntologies ${SDO_MIN_VERSION} REQUIRED)
find_package (PythonLibrary REQUIRED)
find_package (TelepathyGLib ${TELEPATHYGLIB_MIN_VERSION} REQUIRED)
find_package (GLIB2 REQUIRED)
find_package (GObject REQUIRED)
find_package (DBusGLib REQUIRED)

# Check the Python version is new enough
set (REQUIRED_PY 2.5)
if (${PYTHON_SHORT_VERSION} VERSION_GREATER ${REQUIRED_PY} OR ${PYTHON_SHORT_VERSION} VERSION_EQUAL ${REQUIRED_PY})
    message (STATUS "Python ${PYTHON_SHORT_VERSION} found")
else (${PYTHON_SHORT_VERSION} VERSION_GREATER ${REQUIRED_PY} OR ${PYTHON_SHORT_VERSION} VERSION_EQUAL ${REQUIRED_PY})
    message (SEND_ERROR "Python >= ${REQUIRED_PY} is required")
endif (${PYTHON_SHORT_VERSION} VERSION_GREATER ${REQUIRED_PY} OR ${PYTHON_SHORT_VERSION} VERSION_EQUAL ${REQUIRED_PY})

# Check for dbus-python
execute_process (COMMAND ${PYTHON_EXECUTABLE} -c "import dbus.mainloop.glib"
                 RESULT_VARIABLE PYTHON_DBUS_RESULT
)
if (PYTHON_DBUS_RESULT EQUAL 0)
    message (STATUS "dbus-python found")
else (PYTHON_DBUS_RESULT EQUAL 0)
    message (SEND_ERROR "dbus-python not found. dbus-python is required for this library")
endif (PYTHON_DBUS_RESULT EQUAL 0)

# Include some stuff
include (KDE4Defaults)
include (MacroLibrary)

# Add some misc definitions
add_definitions (${KDE4_DEFINITIONS}
                 -DDISABLE_NEPOMUK_LEGACY
                 -DQT_NO_CAST_FROM_ASCII
                 -DQT_NO_KEYWORDS
                 -fPIC
)

# Set the basic include directories
include_directories (${KDE4_INCLUDES}
                     ${TELEPATHY_QT4_INCLUDE_DIR}
                     ${TELEPATHY_GLIB_INCLUDE_DIR}
                     ${NEPOMUK_INCLUDES}
                     ${GLIB2_INCLUDE_DIR}
                     ${GOBJECT_INCLUDE_DIR}
                     ${DBUS_GLIB_INCLUDE_DIR}
                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
)

# Set variables for some special install directories
set (KTELEPATHY_TESTLIB_DATA_INSTALL_DIR
     "${KDE4_DATA_INSTALL_DIR}/KTelepathy/TestLib"
)

set (KTELEPATHY_TESTLIB_DBUS_SERVICE_AUTOSTART_INSTALL_DIR
     "${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/dbus-1/services/"
)

set (KTELEPATHY_TESTLIB_TOOLS_INSTALL_DIR
     "${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/tools"
)

set (KTELEPATHY_TESTLIB_PYTHON_INSTALL_DIR
     "${KTELEPATHY_TESTLIB_DATA_INSTALL_DIR}/python"
)

# FIXME: This is blatantly the wrong place to install cmake files to.
set (KTELEPATHY_TESTLIB_CMAKE_INSTALL_DIR
     "${KDE4_LIB_INSTALL_DIR}/KTelepathy/cmake"
)

# Add subdirectories
add_subdirectory (dbus-1)
add_subdirectory (lib)
add_subdirectory (tools)

