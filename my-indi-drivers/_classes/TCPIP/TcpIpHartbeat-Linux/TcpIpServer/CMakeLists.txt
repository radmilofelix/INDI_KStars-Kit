cmake_minimum_required(VERSION 3.0)

project(TcpIpServer)

add_library(${PROJECT_NAME} STATIC ${PROJECT_NAME}.cpp ${PROJECT_NAME}.h)
