set(example_cm_callable_SRCS
    conn.c
    conn.h
    connection-manager.c
    connection-manager.h
    media-channel.c
    media-channel.h
    media-manager.c
    media-manager.h
    media-stream.c
    media-stream.h)

add_library(ktptest-example-cm-callable STATIC ${example_cm_callable_SRCS})
target_link_libraries(ktptest-example-cm-callable ${TPGLIB_LIBRARIES})
tpqt4_generate_manager_file(${CMAKE_CURRENT_SOURCE_DIR}/manager-file.py example_callable.manager connection-manager.c)

install(TARGETS ktptest-example-cm-callable DESTINATION ${LIB_INSTALL_DIR}/KTelepathy/TestLib)
