# CMakeList.txt : 최상위 CMake 프로젝트 파일, 전역 구성을 수행하고
# 여기에 하위 프로젝트를 포함합니다.
#
cmake_minimum_required (VERSION 3.8)

set(opencv_DIR "D:/2020-1/DIP/opencv/build")
find_package(opencv)

include_directories(${OpenCV_INCLUDE_DIRS})

get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_"  ProjectId ${ProjectId})
project (${ProjectId} C CXX)

set (CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})