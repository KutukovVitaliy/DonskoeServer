cmake_minimum_required(VERSION 3.20)
project(DonskoeServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(DonskoeServer main.cpp MqttModbus.cpp MqttModbus.h ActionListener.cpp ActionListener.h)
target_link_libraries(DonskoeServer paho-mqttpp3 paho-mqtt3as)