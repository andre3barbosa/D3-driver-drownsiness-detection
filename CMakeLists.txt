# cmake_minimum_required(VERSION 3.22.1)
# project( facev2 )
# find_package( OpenCV REQUIRED )
# include_directories( ${OpenCV_INCLUDE_DIRS} )
# add_executable( facev2.elf facev2.cpp)
# target_link_libraries( facev2.elf ${OpenCV_LIBS} )


cmake_minimum_required(VERSION 3.22.1)
project( facev2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

#opencv package
find_package( OpenCV REQUIRED )

# Compilers
set(CMAKE_CXX_COMPILER "/home/andre/buildroot-2021.08/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++")
set(CMAKE_CC_COMPILER "/home/andre/buildroot-2021.08/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-gcc")

# bin files to bin dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# OpenCv include DIRs
set(OpenCV_INCLUDE_DIRS "/usr/include/opencv2")
set(OpenCV_INCLUDE_DIRS "/usr/lib")



add_compile_options(-lrt)
add_compile_options(-lbcm2835)


# including dirs
include_directories( include ${OpenCV_INCLUDE_DIRS} )
include_directories( include ${CMAKE_CURRENT_SOURCE_DIR}/inc/ )
include_directories( include ${CMAKE_CURRENT_SOURCE_DIR}/../inc )

# All Source files
file(GLOB all_SRCS
   "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
   "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"
)

# Main
add_executable( facev2.elf facev2.cpp ${all_SRCS} ) 
target_link_libraries( facev2.elf ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_CXX_FLAGS} -lbcm2835 )

