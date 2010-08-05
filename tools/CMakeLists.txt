# Configure the tools
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/with-nepomuk-sandbox.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/with-nepomuk-sandbox.sh"
                IMMEDIATE @ONLY
)

# Install the tools
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/with-session-bus.sh"
         DESTINATION "${KTELEPATHY_TESTLIB_TOOLS_INSTALL_DIR}"
         COMPONENT Devel
)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/with-nepomuk-sandbox.sh"
         DESTINATION "${KTELEPATHY_TESTLIB_TOOLS_INSTALL_DIR}"
         COMPONENT Devel
)

