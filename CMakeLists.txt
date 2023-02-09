cmake_minimum_required(VERSION 3.22.1)
project( CLocalSystem LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# OpenCV package
find_package( OpenCV REQUIRED )

# Compilers
set(CMAKE_CXX_COMPILER "/home/andre/buildroot-2021.08/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++")
set(CMAKE_CC_COMPILER "/home/andre/buildroot-2021.08/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-gcc")

# Executable directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# OpenCV include DIRs
set(OpenCV_INCLUDE_DIRS "/usr/include/opencv2")
set(OpenCV_INCLUDE_DIRS "/usr/lib")


# Including all includes DIRs
include_directories( include ${OpenCV_INCLUDE_DIRS} )
include_directories( include ${CMAKE_CURRENT_SOURCE_DIR}/inc/ )
include_directories( include ${CMAKE_CURRENT_SOURCE_DIR}/../inc )

# All Source files
file(GLOB all_SRCS
   "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
   "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"   
)

# Daemon process
# add_executable( dSensor.elf CDaemon.cpp CDevCamera.cpp CDrowsinessCam.cpp CBluetoothCom.cpp CDevSensor.cpp dAcquisition.cpp ${all_SRCS} ) 
# target_link_libraries( dSensor.elf ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_CXX_FLAGS} -lbluetooth)

# Main process
add_executable( d3.elf localmain.cpp CLocalSystem.cpp CBluetoothCom.cpp CDevSound.cpp ${all_SRCS} ) 
target_link_libraries( d3.elf ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_CXX_FLAGS} -lbluetooth)