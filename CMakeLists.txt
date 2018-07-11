cmake_minimum_required(VERSION 3.5)
project(videoSystem)

set(CMAKE_CXX_STANDARD 11)


find_package(OpenCV REQUIRED)
find_package(dlib)
find_package(CUDA)

#set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

message(STATUS "CUDA library status:")
message(STATUS "    version: ${CUDA_VERSION}")
message(STATUS "dlib version: ${dlib_VERSION}")
#message(STATUS "curl version: ${libcurl_VERSION}")
#include_directories("${PROJECT_BINARY_DIR}")
set(include_list ${PROJECT_SOURCE_DIR}/detector
        ${PROJECT_SOURCE_DIR}/tracker
        ${PROJECT_SOURCE_DIR}/util
        /home/lyl/darknet-master/include
        /usr/local/include)


aux_source_directory(${PROJECT_SOURCE_DIR}/detector SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/tracker SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/util SRC_LIST)
message(${SRC_LIST})


message(${include_list})
include_directories(${include_list})
LINK_DIRECTORIES(/home/lyl/darknet-master)
LINK_DIRECTORIES(/usr/local/lib)
#LINK_DIRECTORIES(/usr/local/curl)

add_executable(videoSystem main.cpp ${SRC_LIST} ${include_list})

#TARGET_LINK_LIBRARIES(videoSystem libdarknet.so)
#TARGET_LINK_LIBRARIES(videoSystem dlib)
#LINK_LIBRARIES(${PROJECT_SOURCE_DIR}/darknet/libdarknet)
#link_libraries(${PROJECT_SOURCE_DIR}/darknet/dlib/libdlib)
#/usr/local/cuda-8.0/lib64
#-lcuda -lcudart -lcublas -lcurand
#message(${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(videoSystem -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_highgui -lopencv_imgcodecs)
#target_link_libraries(videoSystem ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(videoSystem ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(videoSystem -llapack -lpthread)
target_link_libraries(videoSystem -ldarknet -ldlib -lcurl)