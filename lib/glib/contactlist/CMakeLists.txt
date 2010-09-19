set(example_cm_contactlist_SRCS
    conn.c
    conn.h
    connection-manager.c
    connection-manager.h
    contact-list.c
    contact-list.h
    contact-list-manager.c
    contact-list-manager.h)

add_library(ktptest-example-cm-contactlist STATIC ${example_cm_contactlist_SRCS})
target_link_libraries(ktptest-example-cm-contactlist ${TPGLIB_LIBRARIES})
tpqt4_generate_manager_file(${CMAKE_CURRENT_SOURCE_DIR}/manager-file.py example_contact_list.manager connection-manager.c)

install(TARGETS ktptest-example-cm-contactlist DESTINATION ${LIB_INSTALL_DIR}/KTelepathy/TestLib)
