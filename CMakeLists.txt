CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

EXECUTE_PROCESS(COMMAND "/bin/uname" "-r" OUTPUT_VARIABLE uname)
STRING(REGEX REPLACE "\n$" "" uname "${uname}")

PROJECT(driver)
PROJECT(client)
PROJECT(server)

SET(driver_src "source/vmouse.c")
ADD_EXECUTABLE(driver ${driver_src})
TARGET_INCLUDE_DIRECTORIES(driver PUBLIC "/lib/modules/${uname}/build/include")

#SET(src "source/bnp_mouse_event.cpp")
#ADD_EXECUTABLE(client ${src})
#TARGET_INCLUDE_DIRECTORIES(client PUBLIC "/lib/modules/${uname}/build")