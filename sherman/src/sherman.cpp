#include <ros/ros.h>
#include <ros/package.h>
#include <sound_play/sound_play.h>
#include <sensor_msgs/LaserScan.h>
#include <time.h>
#include <unistd.h>

sensor_msgs::LaserScan msg;

void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scanmsg){
	//set the scanmsg so that main can work with it
	msg = *scanmsg;
}


int main(int argc, char **argv){
	ros::init(argc, argv, "sherman");
	
	ros::NodeHandle n;

	int threshold = 1;

	sound_play::SoundClient sc;

	ros::Subscriber sub = n.subscribe<sensor_msgs::LaserScan>("/scan", 1000, &scanCallback);
	
	ros::spinOnce();

	std::string name = ros::package::getPath("sherman");
	ROS_INFO_STREAM(name);

	ROS_INFO_STREAM("Running Sherman...");	
	while(ros::ok()){
		//check to make sure that the msg has been set by the callback
		if (msg.ranges.size() > 0){
			int startQ1 = (msg.ranges.size())/4;
	        	int startQ2 = (msg.ranges.size())/2;
			int endQ1 = startQ2;
			int endQ2 = startQ2 + startQ1;
	        	double min1 = 500;
	        	double min2 = 500;

			//find the minimum value on the left and the right
	        	for (int i = startQ1; i < endQ1; i++){
				if (msg.ranges[i] < min1) min1 = msg.ranges[i];
	        	}
			
		        for (int i = startQ2; i < endQ2; i++){
				if (msg.ranges[i] < min2) min2 = msg.ranges[i];
	        	}
			
			//say which side is too close
			//if it's both, say both
			//if they're both good, say good
			if (min1 <= threshold && min2 <= threshold) sc.playWave(name + "/both.wav");
			else if (min1 <= threshold) sc.playWave(name + "/right.wav");
			else if (min2 <= threshold) sc.playWave(name + "/left.wav");
			else sc.playWave(name + "/good.wav");

			sleep(1);
		}
	
		ros::spinOnce();
	}

	return 0;
}
