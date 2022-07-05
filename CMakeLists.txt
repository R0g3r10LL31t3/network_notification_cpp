# CMakeList.txt : CMake project for network_notification, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)

# COMPILATION
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SOURCE-FILES-LIST
list(APPEND NETWORK_NOTIFICATION_MAIN_SOURCE_FILES_LIST "network_notification_main.cpp"
                                                        "network_notification.cpp" "network_notification.h")

# NETWORK-NOTIFICATION-MAIN
add_executable (network_notification_main ${NETWORK_NOTIFICATION_MAIN_SOURCE_FILES_LIST})
#target_include_directories(network_notification_main PRIVATE ${DEPENDENCIES_INCLUDE_DIRECTORY_LIST})
#target_link_directories(network_notification_main PRIVATE ${DEPENDENCIES_LIBRARY_DIRECTORIES_LIST})
#target_link_libraries(network_notification_main PRIVATE ${DEPENDENCIES_LIBRARY_FILES_LIST})

# SOURCE-FILES-LIST
list(APPEND NETWORK_NOTIFICATION_ZMQ_MAIN_SOURCE_FILES_LIST "network_notification_zmq_main.cpp"
                                                            "network_notification.cpp" "network_notification.h"
                                                            "network_notification_zmq.cpp" "network_notification_zmq.h")
# NETWORK-NOTIFICATION-MAIN
# TODO: add cppzmq library
add_executable (network_notification_zmq_main ${NETWORK_NOTIFICATION_MAIN_SOURCE_FILES_LIST})
#target_include_directories(network_notification_zmq_main PRIVATE ${DEPENDENCIES_INCLUDE_DIRECTORY_LIST})
#target_link_directories(network_notification_zmq_main PRIVATE ${DEPENDENCIES_LIBRARY_DIRECTORIES_LIST})
#target_link_libraries(network_notification_zmq_main PRIVATE ${DEPENDENCIES_LIBRARY_FILES_LIST})
