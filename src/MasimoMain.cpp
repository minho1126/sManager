#include <iostream>
#include <cstdio>
#include "ros/ros.h"
#include <cstdlib>
#include "MasimoDriver.hpp"
#include "std_msgs/String.h"
#include "smanager/RegisterMessage.h"
#include "smanager/Checker.h"
#include "smanager/ServiceRequest.h"
#include "smanager/data.h"
#include <csignal>
//http://www.thomaswhitton.com/blog/2013/06/27/json-c-plus-plus-examples/
using namespace std;

void threadCloser(int signum){
	cout<<"closing the thread"<<endl;
	pthread_exit(NULL);
	exit(0);
}

void *fake_call(void* conman){
	ROS_INFO("Starting fake call");
	signal(SIGINT,threadCloser);
	MasimoDriver* cm=(MasimoDriver*)conman;
	cm->StartService();
	pthread_exit(NULL);
}

int main(int argc,char **argv){
	MasimoDriver conman;
	ros::init(argc,argv,conman.getServiceName());
	ros::NodeHandle n,m;
	ros::ServiceClient client=n.serviceClient<smanager::RegisterMessage>("driver_request");
	if(!conman.Register(client)){
		exit(0);
	}
	ROS_INFO("client registering Masimo_request");
	ros::ServiceServer services=n.advertiseService("Masimo_request",&MasimoDriver::InitialiseService,&conman);
	ros::Rate loop_rate(10);
	while(1){
		if(conman.is_Started()){
			ROS_INFO("ONLY ONCE");
			pthread_t threads[1];
			pthread_create(&threads[0], NULL, fake_call,(void*)&conman);
		}
		smanager::data msg;
		msg.data="aaa";
		ros::spinOnce();
	}
	return 0;
}
