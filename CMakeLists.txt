cmake_minimum_required(VERSION 2.8)
project( DisplayImage )
find_package( OpenCV REQUIRED )
include_directories(${GSL_INCLUDE_DIRS} ${GSLCBLAS_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GSL_LIBRARIES} ${GSLCBLAS_LIBRARIES})
add_executable(captureImage captureImage.cpp CalibratorWindow.cpp LineFunction.cpp ErrorDetector.cpp)

target_link_libraries(captureImage ${OpenCV_LIBS} -lgsl)
