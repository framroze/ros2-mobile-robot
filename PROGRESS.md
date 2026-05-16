# 📝 Development Progress

## 🖥️ System Info
- **GitHub:** [framroze](https://github.com/framroze)
- **OS:** Ubuntu 24.04 LTS
- **ROS2:** Jazzy

## 📂 Workspace
- ROS2 workspace: `~/ros2_ws`
- Package: `~/ros2_ws/src/mobile_robot_pkg`
- Map: `~/ros2_ws/src/mobile_robot_pkg/maps/turtlebot3_world_map.yaml`

---

## ⚠️ Important — Build Fix (Miniconda Conflict)

If `colcon build` fails due to library conflicts with Miniconda:

```bash
rm -rf build/mobile_robot_pkg
export PATH=/usr/bin:/usr/local/bin:/bin:/usr/sbin:/sbin
export LD_LIBRARY_PATH=/opt/ros/jazzy/lib:/usr/lib/x86_64-linux-gnu
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH
source /opt/ros/jazzy/setup.bash
```

---

## ▶️ How to Run

**Terminal 1 — Gazebo:**
```bash
source /opt/ros/jazzy/setup.bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py
```

**Terminal 2 — Nav2:**
```bash
source /opt/ros/jazzy/setup.bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_navigation2 navigation2.launch.py use_sim_time:=True map:=$HOME/ros2_ws/src/mobile_robot_pkg/maps/turtlebot3_world_map.yaml
```

In RViz2 click "2D Pose Estimate" → click center of map → drag arrow right.

**Terminal 3 — Run autonomous patrol:**
```bash
cd ~/ros2_ws
source install/setup.bash
ros2 run mobile_robot_pkg mobile_robot_controller
```

---

## ✅ Completed Milestones

- [x] TurtleBot3 Waffle in Gazebo Sim
- [x] LiDAR + Camera + IMU sensors integrated
- [x] Cartographer SLAM mapping working in real-time
- [x] Map saved permanently (`turtlebot3_world_map.yaml`)
- [x] Nav2 autonomous navigation working
- [x] Localization with AMCL
- [x] C++ controller with action client
- [x] Autonomous patrol through multiple waypoints
- [x] Obstacle detection via LiDAR
- [x] Odometry tracking
- [x] Recovery behavior on failed goals
- [x] Status publisher for mission monitoring
- [x] All code pushed to GitHub

---

## 🧠 Concepts Learned

- ROS2 action client/server pattern
- Nav2 NavigateToPose action
- SLAM with Cartographer
- AMCL localization
- Costmaps and path planning
- Map saving with map_server
- LiDAR LaserScan processing
- Odometry message handling
- TF transform tree
- Custom Gazebo world creation
- Launch file composition

---

## 🚦 Status

🟢 **PROJECT COMPLETE**
