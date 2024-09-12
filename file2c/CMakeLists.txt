file(GLOB HEADERS *.h)
file(GLOB SRC *.cc)

add_executable(file2c ${HEADERS} ${SRC})

install(TARGETS file2c DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
