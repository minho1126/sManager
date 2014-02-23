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

void Resultcallback(smanager::data msg){
	ROS_INFO("%s",msg.data.c_str());
}

int main(int argc,char **argv){
		ros::init(argc,argv,"UserA");
		sleep(10);
		ROS_INFO("starting service");
		ros::NodeHandle n;
		//alocateService
		ros::ServiceClient client =n.serviceClient<smanager::ServiceRequest>("request_listener");
		//make message
		smanager::ServiceRequest msg;
		msg.request.Request="Request";
		msg.request.type="SPO2";
		msg.request.id=-1;
		msg.request.bid=-1;
		msg.request.uid=1;
		if(client.call(msg)){
			ROS_INFO("REQUEST SENT");
		}else{
			ROS_INFO("REQUEST FAILED");
		}
		if(msg.response.id==-1){
			ROS_INFO("couldnt find a right sensor terminating");
		}
		
		sleep(1);
		msg.request.Request="Start";
		msg.request.id=msg.response.id;
		if(client.call(msg)){
			ROS_INFO("Start SENT");
			if(msg.response.Response=="Success"){
				ROS_INFO("starting now");
			}else{
				ROS_INFO("failed to start");
				ROS_INFO("reason:%s",msg.response.Response.c_str());
			}
			//ros::Subscriber sub=n.subscribe(name,1000,Resultcallback,NULL);
		}else{
			ROS_INFO("Start FAILED-connection error");
		}
		
		sleep(10);
		//trial 2-stop
		msg.request.Request="Stop";
		//msg.request.id=msg.response.id;
//		ROS_INFO("the id is %s",msg.request.id.c_str);
		if(client.call(msg)){
			ROS_INFO("Stop SENT");
			if(msg.response.Response=="Success"){
				ROS_INFO("Stopping now");
			}else{
				ROS_INFO("failed to Stop");
				ROS_INFO("reason:%s",msg.response.Response.c_str());
			}
			//ros::Subscriber sub=n.subscribe(name,1000,Resultcallback,NULL);
		}else{
			ROS_INFO("Stop FAILED-connection error");
		}
		sleep(10);
		//trial 3-one more time
//		ROS_INFO("the id is %s",msg.request.id.c_str());
		msg.request.Request="Start";
		//msg.request.id=msg.response.id;
		if(client.call(msg)){
			ROS_INFO("Start2 SENT");
			if(msg.response.Response=="Success"){
				ROS_INFO("starting now");
			}else{
				ROS_INFO("failed to start");
				ROS_INFO("reason:%s",msg.response.Response.c_str());
			}
			//ros::Subscriber sub=n.subscribe(name,1000,Resultcallback,NULL);
		}else{
			ROS_INFO("Start FAILED-connection error");
		}
		sleep(10);
		//kill
		msg.request.Request="Terminate";
		ROS_INFO("request id is %d",msg.request.id);
		//msg.request.id=msg.response.id;
		if(client.call(msg)){
			ROS_INFO("Terminate SENT");
		}else{
			ROS_INFO("Terminate FAILED");
		}
		/*//2nd trial
		//make message
		//smanager::ServiceRequest msg;
		msg.request.Request="Request";
		msg.request.type="SPO2";
		msg.request.id=-1;
		msg.request.bid=-1;
		msg.request.uid=1;
		if(client.call(msg)){
			ROS_INFO("REQUEST SENT");
		}else{
			ROS_INFO("REQUEST FAILED");
		}
		if(msg.response.id==-1){
			ROS_INFO("couldnt find a right sensor terminating");
			pthread_exit(NULL);
		}
		sleep(1);
		msg.request.Request="Start";
		msg.request.id=msg.response.id;
		if(client.call(msg)){
			ROS_INFO("Start SENT");
			//ros::Subscriber sub=n.subscribe(name,1000,Resultcallback,NULL);
		}else{
			ROS_INFO("Start FAILED");
		}
		sleep(30);
		msg.request.Request="Terminate";
		ROS_INFO("request id is %d",msg.request.id);
		//msg.request.id=msg.response.id;
		if(client.call(msg)){
			ROS_INFO("Terminate SENT");
		}else{
			ROS_INFO("Terminate FAILED");
		}*/
		
		//terminate
		ROS_INFO("EXITING");
		return 0;
}
