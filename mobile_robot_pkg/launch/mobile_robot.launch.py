import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')
    map_file = LaunchConfiguration('map', default=os.path.join(
        os.path.expanduser('~'),
        'ros2_ws/src/mobile_robot_pkg/maps/turtlebot3_world_map.yaml'))

    turtlebot3_gazebo = get_package_share_directory('turtlebot3_gazebo')
    turtlebot3_navigation2 = get_package_share_directory('turtlebot3_navigation2')

    # Launch Gazebo world
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(turtlebot3_gazebo, 'launch', 'turtlebot3_world.launch.py')
        )
    )

    # Launch Navigation2
    navigation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(turtlebot3_navigation2, 'launch', 'navigation2.launch.py')
        ),
        launch_arguments={
            'use_sim_time': use_sim_time,
            'map': map_file
        }.items()
    )

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='true'),
        DeclareLaunchArgument('map', default_value=os.path.join(
            os.path.expanduser('~'),
            'ros2_ws/src/mobile_robot_pkg/maps/turtlebot3_world_map.yaml')),
        gazebo,
        navigation,
    ])
