cmake_minimum_required(VERSION 2.8)
project( CLocalSystem )
#find_package( OpenCV REQUIRED )
#include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( dSensor.elf CDaemon.cpp CBluetoothCom.cpp dAcquisition.cpp CDevCamera.cpp CDevSensor.cpp)
#target_link_libraries( detection ${OpenCV_LIBS} )