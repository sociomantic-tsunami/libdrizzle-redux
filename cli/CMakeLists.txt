find_package(GLIB2 REQUIRED)

add_executable(libdrizzle_config libdrizzle_config.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB2_INCLUDE_DIR})
target_link_libraries(libdrizzle_config drizzle-redux ${GLIB2_LIBRARY})

INSTALL(TARGETS libdrizzle_config RUNTIME DESTINATION bin)
