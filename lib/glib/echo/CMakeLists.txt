include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(example_cm_echo_SRCS
    chan.c
    chan.h
    conn.c
    conn.h
    connection-manager.c
    connection-manager.h
    im-manager.c
    im-manager.h
)

add_library(ktptest-example-cm-echo STATIC ${example_cm_echo_SRCS})
target_link_libraries(ktptest-example-cm-echo ${TPGLIB_LIBRARIES})
tpqt4_generate_manager_file(${CMAKE_CURRENT_SOURCE_DIR}/manager-file.py example_echo.manager connection-manager.c)

install(TARGETS ktptest-example-cm-echo DESTINATION ${LIB_INSTALL_DIR}/KTelepathy/TestLib)
