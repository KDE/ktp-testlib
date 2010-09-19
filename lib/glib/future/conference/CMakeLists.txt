include_directories(
    ${CMAKE_SOURCE_DIR}/tests/lib/glib/future)

set(future_example_cm_conference_SRCS
    chan.c
    chan.h)

add_library(ktptest-future-example-cm-conference STATIC ${future_example_cm_conference_SRCS})
target_link_libraries(ktptest-future-example-cm-conference ${TPGLIB_LIBRARIES}
                        ktptest-tp-glib-tests-future-extensions)

install(TARGETS ktptest-future-example-cm-conference DESTINATION ${LIB_INSTALL_DIR}/KTelepathy/TestLib)