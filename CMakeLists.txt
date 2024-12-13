cmake_minimum_required(VERSION 3.10)
project(pdf-viewer C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(POPPLER REQUIRED poppler-glib)

include_directories(${GTK_INCLUDE_DIRS} ${POPPLER_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS} ${POPPLER_LIBRARY_DIRS})

add_definitions(${GTK_CFLAGS_OTHER} ${POPPLER_CFLAGS_OTHER})

add_executable(pdf-viewer src/main.c)
target_link_libraries(pdf-viewer ${GTK_LIBRARIES} ${POPPLER_LIBRARIES})