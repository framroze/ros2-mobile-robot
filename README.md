# 🤖 ROS2 Mobile Robot — Autonomous Navigation with SLAM

<p align="center">
  <img src="https://img.shields.io/badge/ROS2-Jazzy-22314E?style=for-the-badge&logo=ros&logoColor=white"/>
  <img src="https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white"/>
  <img src="https://img.shields.io/badge/Nav2-Autonomous-2BAF2B?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/SLAM-Mapping-FF6600?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Ubuntu-24.04-E95420?style=for-the-badge&logo=ubuntu&logoColor=white"/>
  <img src="https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge"/>
</p>

<p align="center">
  <b>An autonomous mobile robot that maps its environment with SLAM and navigates between waypoints using LiDAR, Camera, and Nav2 — fully controlled by C++ code.</b>
</p>

---

## ✨ What it does

A complete autonomous mobile robot system that:

1. 🗺️ **Maps the environment** in real-time using LiDAR + Cartographer SLAM
2. 💾 **Saves the generated map** permanently for reuse
3. 🤖 **Autonomously navigates** to multiple waypoints in sequence
4. 📡 **Avoids obstacles** dynamically using Nav2 path planning
5. 📊 **Reports progress** with distance, ETA, and status updates
6. 🛡️ **Recovers from failures** with automatic retry behavior
7. 📷 **Streams camera feed** during navigation

---

## 🧰 Tech Stack

| Component | Used For |
|-----------|----------|
| **ROS2 Jazzy** | Robotics middleware |
| **C++17 / rclcpp / rclcpp_action** | Robot control logic |
| **Nav2** | Path planning + autonomous navigation |
| **Cartographer SLAM** | Real-time mapping |
| **Gazebo Sim** | Physics simulation |
| **RViz2** | 3D visualization |
| **TurtleBot3 Waffle** | LiDAR + Camera + IMU mobile robot |

---

## 📋 Requirements

- Ubuntu 24.04 LTS
- ROS2 Jazzy ([Install Guide](https://docs.ros.org/en/jazzy/Installation.html))
- TurtleBot3 simulation packages
- Nav2 and SLAM Toolbox

---

## ⚙️ Installation

### 1️⃣ Install required packages

```bash
sudo apt update
sudo apt install -y \
  ros-jazzy-turtlebot3 \
  ros-jazzy-turtlebot3-simulations \
  ros-jazzy-turtlebot3-gazebo \
  ros-jazzy-turtlebot3-cartographer \
  ros-jazzy-turtlebot3-navigation2 \
  ros-jazzy-navigation2 \
  ros-jazzy-nav2-bringup \
  ros-jazzy-slam-toolbox \
  ros-jazzy-cartographer-ros \
  python3-colcon-common-extensions \
  git
```

### 2️⃣ Set environment variables

```bash
echo "export TURTLEBOT3_MODEL=waffle" >> ~/.bashrc
source ~/.bashrc
```

### 3️⃣ Clone this repository

```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
git clone https://github.com/framroze/ros2-mobile-robot.git
cp -r ros2-mobile-robot/mobile_robot_pkg .
```

### 4️⃣ Build the package

```bash
cd ~/ros2_ws
source /opt/ros/jazzy/setup.bash
colcon build --packages-select mobile_robot_pkg
```

---

## 🚀 How to Run

### Phase 1 — Build a Map with SLAM

**Terminal 1 — Launch Gazebo:**
```bash
source /opt/ros/jazzy/setup.bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py
```

**Terminal 2 — Launch SLAM:**
```bash
source /opt/ros/jazzy/setup.bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_cartographer cartographer.launch.py use_sim_time:=True
```

**Terminal 3 — Drive the robot:**
```bash
source /opt/ros/jazzy/setup.bash
ros2 run turtlebot3_teleop teleop_keyboard
```

> Use **W A S D X** to drive and explore. Watch the map build live in RViz2!

**Terminal 4 — Save the map:**
```bash
cd ~/ros2_ws/src/mobile_robot_pkg/maps
ros2 run nav2_map_server map_saver_cli -f turtlebot3_world_map
```

---

### Phase 2 — Autonomous Navigation

**Terminal 1 — Gazebo:**
```bash
source /opt/ros/jazzy/setup.bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py
```

**Terminal 2 — Nav2 with saved map:**
```bash
source /opt/ros/jazzy/setup.bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_navigation2 navigation2.launch.py use_sim_time:=True map:=$HOME/ros2_ws/src/mobile_robot_pkg/maps/turtlebot3_world_map.yaml
```

> In RViz2, click **"2D Pose Estimate"** and click where the robot is, then drag to set its direction.

**Terminal 3 — Run the autonomous patrol:**
```bash
cd ~/ros2_ws
source install/setup.bash
ros2 run mobile_robot_pkg mobile_robot_controller
```

> Watch the robot autonomously navigate through all waypoints! 🎉

---

## 🩺 Troubleshooting

### Build error with `libssl` or `libcrypto` (Miniconda users)

```bash
cd ~/ros2_ws
rm -rf build/mobile_robot_pkg
export PATH=/usr/bin:/usr/local/bin:/bin:/usr/sbin:/sbin
export LD_LIBRARY_PATH=/opt/ros/jazzy/lib:/usr/lib/x86_64-linux-gnu
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH
source /opt/ros/jazzy/setup.bash
colcon build --packages-select mobile_robot_pkg
```

---

## 📂 Project Structure

| Path | Description |
|------|-------------|
| 📦 [`mobile_robot_pkg/`](mobile_robot_pkg/) | ROS2 C++ package |
| 📄 [`mobile_robot_pkg/src/mobile_robot_controller.cpp`](mobile_robot_pkg/src/mobile_robot_controller.cpp) | Autonomous navigation controller |
| 📄 [`mobile_robot_pkg/CMakeLists.txt`](mobile_robot_pkg/CMakeLists.txt) | Build configuration |
| 📄 [`mobile_robot_pkg/package.xml`](mobile_robot_pkg/package.xml) | Package manifest |
| 🗺️ [`mobile_robot_pkg/maps/`](mobile_robot_pkg/maps/) | Saved SLAM maps |
| 📖 [`README.md`](README.md) | This file |

---

## 🧠 Concepts Demonstrated

- ✅ ROS2 nodes, action clients, publishers, subscribers
- ✅ C++ control with `rclcpp` and `rclcpp_action`
- ✅ Nav2 NavigateToPose action client integration
- ✅ Cartographer SLAM real-time mapping
- ✅ LiDAR (LaserScan) processing for obstacle detection
- ✅ Odometry tracking for position feedback
- ✅ Autonomous waypoint navigation
- ✅ Recovery behaviors and retry logic
- ✅ Status publishing for mission monitoring

---

## 🎬 Highlights

- 🟢 **Full autonomy** — robot maps, navigates, and avoids obstacles by itself
- 🟢 **Real sensors** — LiDAR, Camera, IMU, Odometry all integrated
- 🟢 **Production-grade Nav2** — same stack used by industrial robots
- 🟢 **Clean C++ code** — modular, well-commented, easy to extend
- 🟢 **Live feedback** — distance, ETA, and obstacle warnings in real-time

---

## 👤 Author

**framroze**  
🌐 GitHub: [@framroze](https://github.com/framroze)

---

## 📜 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
