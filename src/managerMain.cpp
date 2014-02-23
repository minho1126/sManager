#include <iostream>
#include <cstdio>
#include "ros/ros.h"
#include <cstdlib>
#include "smanager/RegisterMessage.h"
#include "SensorManager.hpp"
#include "smanager/data.h"
#include <pthread.h>
#include <sstream>
#include "smanager/ServiceRequest.h"
#include <csignal>


using namespace std;

void threadCloser(int signum){
	cout<<"closing the thread"<<endl;
	pthread_exit(NULL);
	exit(0);
}

void* checker(void* i){
	signal(SIGINT,threadCloser);
	SensorManager* sm= (SensorManager*)i;
	while(true){
		sleep(10);
		ROS_INFO("pinging");	
		sm->Ping();
	}
	ROS_INFO("ended");
	pthread_exit(NULL);
}

int main(int argc,char **argv){
	SensorManager sm;
	ros::init(argc,argv,"Manager_1");
	ros::NodeHandle n;
	ros::ServiceServer service=n.advertiseService("driver_request",&SensorManager::foo,&sm);
	ROS_INFO("READY");
	pthread_t threads[1];
	ros::ServiceServer Reqlistener=n.advertiseService("request_listener",&SensorManager::listenForRequest,&sm);
	pthread_create(&threads[0], NULL, checker, (void*) &sm);
	while(true){
		ros::spinOnce();
	}
	return 0;
}
