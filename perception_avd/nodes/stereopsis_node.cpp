/**
 * @file   stereopsis_node.cpp
 *
 * Richárd, Sergi, Mathesh and Carlos.
 *
 * @brief ROS wrapper for detection of (spheric) red obstacles.
 * @brief Publishes image points corresponding to object centers.
 */

#include "perception_avd/stereopsis.h"

#include <message_filters/time_synchronizer.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/subscriber.h>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "triangulation");
  ros::NodeHandle nh;

  perception_avd::Stereopsis stereo_triangulation(nh);
  stereo_triangulation.initPublishers();

  // Call Subs synchronizer
  message_filters::Subscriber<rovi2_msgs::points2d> image_left(nh, stereo_triangulation.left_img_topic_, 1);
  message_filters::Subscriber<rovi2_msgs::points2d> image_right(nh, stereo_triangulation.right_img_topic_, 1);

  // Call to approximate time synchronizer
  typedef message_filters::sync_policies::ApproximateTime<rovi2_msgs::points2d, rovi2_msgs::points2d> MySyncPolicy;
  message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), image_left, image_right);

  // boost::bind requires an extra argument which is a pointer to the instance of that class.
  sync.registerCallback(
      boost::bind(&perception_avd::Stereopsis::synchronizedTriangulation, stereo_triangulation, _1, _2));

  ROS_INFO("Initialized Register Callback");

  while (ros::ok())
  {
    ros::spinOnce();
  }

  stereo_triangulation.freeMemory();
  return 0;
}