#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <nav2_msgs/action/navigate_through_poses.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <std_msgs/msg/string.hpp>

class MobileRobotController : public rclcpp::Node
{
public:
  using NavigateToPose = nav2_msgs::action::NavigateToPose;
  using NavigateThroughPoses = nav2_msgs::action::NavigateThroughPoses;
  using GoalHandleNav = rclcpp_action::ClientGoalHandle<NavigateToPose>;

  MobileRobotController() : Node("mobile_robot_controller")
  {
    RCLCPP_INFO(this->get_logger(), "========================================");
    RCLCPP_INFO(this->get_logger(), "   Mobile Robot Navigation Controller   ");
    RCLCPP_INFO(this->get_logger(), "   LiDAR + Camera + Nav2 + SLAM        ");
    RCLCPP_INFO(this->get_logger(), "========================================");

    // Nav2 action client
    nav_client_ = rclcpp_action::create_client<NavigateToPose>(this, "navigate_to_pose");

    // Status publisher
    status_pub_ = this->create_publisher<std_msgs::msg::String>("robot_status", 10);

    // LiDAR subscriber - monitor for obstacles
    lidar_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", 10,
      [this](const sensor_msgs::msg::LaserScan::SharedPtr msg) {
        monitor_obstacles(msg);
      });

    // Odometry subscriber - track position
    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
      "odom", 10,
      [this](const nav_msgs::msg::Odometry::SharedPtr msg) {
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;
      });

    RCLCPP_INFO(this->get_logger(), "Waiting for Nav2 action server...");
    if (!nav_client_->wait_for_action_server(std::chrono::seconds(30))) {
      RCLCPP_ERROR(this->get_logger(), "Nav2 action server not available!");
      return;
    }
    RCLCPP_INFO(this->get_logger(), "Nav2 connected! Starting mission...");
  }

  void publish_status(const std::string & status)
  {
    auto msg = std_msgs::msg::String();
    msg.data = status;
    status_pub_->publish(msg);
    RCLCPP_INFO(this->get_logger(), "STATUS: %s", status.c_str());
  }

  void monitor_obstacles(const sensor_msgs::msg::LaserScan::SharedPtr msg)
  {
    // Check front of robot for close obstacles
    float min_dist = std::numeric_limits<float>::infinity();
    int num_ranges = msg->ranges.size();
    int front_start = num_ranges * 0 / 360;
    int front_end = num_ranges * 30 / 360;

    for (int i = front_start; i < front_end; i++) {
      if (msg->ranges[i] > msg->range_min && msg->ranges[i] < msg->range_max) {
        min_dist = std::min(min_dist, msg->ranges[i]);
      }
    }

    if (min_dist < 0.3f) {
      RCLCPP_WARN(this->get_logger(), "⚠️  OBSTACLE DETECTED at %.2fm - Nav2 handling avoidance!", min_dist);
    }
  }

  bool send_goal(double x, double y, double yaw, const std::string & label)
  {
    publish_status("Navigating to " + label + " at (" + 
                   std::to_string(x) + ", " + std::to_string(y) + ")");

    auto goal_msg = NavigateToPose::Goal();
    goal_msg.pose.header.frame_id = "map";
    goal_msg.pose.header.stamp = this->now();
    goal_msg.pose.pose.position.x = x;
    goal_msg.pose.pose.position.y = y;
    goal_msg.pose.pose.position.z = 0.0;
    goal_msg.pose.pose.orientation.z = sin(yaw / 2.0);
    goal_msg.pose.pose.orientation.w = cos(yaw / 2.0);

    auto send_goal_options = rclcpp_action::Client<NavigateToPose>::SendGoalOptions();

    send_goal_options.feedback_callback =
      [this, label](GoalHandleNav::SharedPtr,
        const std::shared_ptr<const NavigateToPose::Feedback> feedback) {
        RCLCPP_INFO(this->get_logger(),
          "[%s] Distance remaining: %.2fm | ETA: %.1fs",
          label.c_str(),
          feedback->distance_remaining,
          feedback->estimated_time_remaining.sec + feedback->estimated_time_remaining.nanosec * 1e-9);
      };

    send_goal_options.result_callback =
      [this, label](const GoalHandleNav::WrappedResult & result) {
        if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
          RCLCPP_INFO(this->get_logger(), "✅ [%s] REACHED! Position: (%.2f, %.2f)",
            label.c_str(), current_x_, current_y_);
          publish_status(label + " REACHED!");
          goal_succeeded_ = true;
        } else {
          RCLCPP_WARN(this->get_logger(), "❌ [%s] Failed - attempting recovery...", label.c_str());
          publish_status(label + " FAILED - recovering...");
          goal_succeeded_ = false;
        }
        goal_done_ = true;
      };

    goal_done_ = false;
    goal_succeeded_ = false;
    nav_client_->async_send_goal(goal_msg, send_goal_options);

    while (rclcpp::ok() && !goal_done_) {
      rclcpp::spin_some(this->get_node_base_interface());
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return goal_succeeded_;
  }

  void run_mission()
  {
    RCLCPP_INFO(this->get_logger(), "");
    RCLCPP_INFO(this->get_logger(), "🚀 MISSION START: Autonomous Patrol");
    RCLCPP_INFO(this->get_logger(), "Robot equipped with LiDAR + Camera");
    RCLCPP_INFO(this->get_logger(), "Nav2 handling path planning + obstacle avoidance");
    RCLCPP_INFO(this->get_logger(), "");

    struct Waypoint {
      double x, y, yaw;
      std::string name;
    };

    std::vector<Waypoint> waypoints = {
      {1.2,  0.7,  1.57, "Waypoint 1 — Northeast"},
      {-1.2, 0.7,  3.14, "Waypoint 2 — Northwest"},
      {1.2,  -0.7, 0.0,  "Waypoint 3 — Southeast"},
    };

    int total = waypoints.size();
    int reached = 0;
    int failed = 0;

    for (int i = 0; i < total; i++) {
      auto & wp = waypoints[i];
      RCLCPP_INFO(this->get_logger(), "");
      RCLCPP_INFO(this->get_logger(), "--- Mission Step %d/%d ---", i + 1, total);

      bool success = send_goal(wp.x, wp.y, wp.yaw, wp.name);
      if (success) {
        reached++;
      } else {
        failed++;
        // Recovery: try once more with slightly different position
        RCLCPP_INFO(this->get_logger(), "Retrying with offset position...");
        success = send_goal(wp.x + 0.1, wp.y + 0.1, wp.yaw, wp.name + " (retry)");
        if (success) reached++;
      }

      rclcpp::sleep_for(std::chrono::milliseconds(500));
    }

    RCLCPP_INFO(this->get_logger(), "");
    RCLCPP_INFO(this->get_logger(), "========================================");
    RCLCPP_INFO(this->get_logger(), "   MISSION COMPLETE!");
    RCLCPP_INFO(this->get_logger(), "   Waypoints reached: %d/%d", reached, total);
    RCLCPP_INFO(this->get_logger(), "   Failed: %d", failed);
    RCLCPP_INFO(this->get_logger(), "========================================");
    publish_status("Mission complete! " + std::to_string(reached) + "/" + 
                   std::to_string(total) + " waypoints reached.");
  }

private:
  rclcpp_action::Client<NavigateToPose>::SharedPtr nav_client_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr lidar_sub_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  bool goal_done_ = false;
  bool goal_succeeded_ = false;
  double current_x_ = 0.0;
  double current_y_ = 0.0;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MobileRobotController>();
  node->run_mission();
  rclcpp::shutdown();
  return 0;
}
