project(facebookBasketball)
cmake_minimum_required(VERSION 2.8)

#Find and include OpenCV directories
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( "FacebookBasketBaller" )

#Setup flags and files to compile
set( CMAKE_CXX_FLAGS "-std=c++0x -Wall -g -fexceptions -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videoio -lopencv_videostab ")
file(GLOB SOURCES "FacebookBasketBaller/*.cpp")

add_executable (facebookBasketball ${SOURCES})
target_link_libraries(facebookBasketball ${OpenCV_LIBS} )
