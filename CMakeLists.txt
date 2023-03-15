cmake_minimum_required(VERSION 3.24)
project(curltest C)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c)
add_executable(curl_test ${SOURCE_FILES} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(curl/include/curl)
link_directories(curl/bin)




target_link_libraries(curl_test curl)