cmake_minimum_required(VERSION 3.5)

project(medit)

find_package(zlib)
find_package(TagLib 2.1.1 REQUIRED)

set(SOURCES
	./src/main.cpp
	./src/ME_Core.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ./include/)

target_link_libraries(${PROJECT_NAME} PRIVATE TagLib::tag)
