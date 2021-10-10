cmake_minimum_required(VERSION 3.5)
project(tcp2serial)

find_package(PkgConfig REQUIRED)

find_package(Threads)

aux_source_directory(./src SRC)
#aux_source_directory(./src/data_structures SRC)
aux_source_directory(./src/thread SRC)

#FILE(GLOB SRC "src/*.cpp")
#SET(SRC ${SRC})

include_directories(
    /usr/local/include/linuxserialport
    /usr/local/include/tcpclientserver
    include
    include/data_structures
    include/thread
    )

link_directories(   
    /usr/local/lib/mylib
    )

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    linuxserialport
    tcpclientserver
    pthread
    )

#install(TARGETS ${PROJECT_NAME} DESTINATION "/home/nvidia/gst_1-14-5/gst-plugins-bad-1.14.5/tools/my_filters/nvtrackerwrapper")
