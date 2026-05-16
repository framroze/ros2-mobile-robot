cmake_minimum_required(VERSION 3.8)
project(mobile_robot_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(mobile_robot_controller src/mobile_robot_controller.cpp)
ament_target_dependencies(mobile_robot_controller
  rclcpp
  rclcpp_action
  nav2_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  std_msgs)

install(TARGETS mobile_robot_controller
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch maps config worlds rviz
  DESTINATION share/${PROJECT_NAME})

ament_package()
