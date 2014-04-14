#include "SPDriver.hpp"
#include "OxySmartSpO2.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <sys/types.h>
#include "ros/ros.h"
#include "smanager/Checker.h"
#include "smanager/RegisterMessage.h"
#include "smanager/ServiceRequest.h"
#include "smanager/data.h"
#include <string>
#include <sstream>
#include <pthread.h>
#include <fstream>

//#include "jsoncpp-src-0.50/"
using namespace std;

SPDriver::SPDriver(){
	__name="OxySmartSp02";
	__type="SPO2";//type can only be SPO2, BP
	__ipAddress="127.0.0.1";//example
	__id =generateID();
	//read bid
	std::ifstream infile;
	infile.open("settings.txt");
	if(!infile.is_open()){
		ROS_INFO("no setup file");
		__bid=1;
	}else{
		infile >> __bid;
		//__bid=1;//example
		infile.close();
	}
	ROS_INFO("bid is %d",__bid);
	///////
	__provide_Service=false;
	__pause=false;
	isFresh=false;
	isConnected=false;
}

void SPDriver::StartService(){
	smanager::data msg;
	pthread_t t; 
	int* i;
	pthread_create(&t,NULL,&SPDriver::updateReadingsThread,this);
	ROS_INFO("ENTERING WHILE LOOP");
	while(__processing){
		if(!isFresh){
				//ROS_INFO("Sleepping...");
				usleep(1000);
				continue;
		}
		//sleep(1);
		if(spo2.pulseRate > 0 && spo2.spo2Percent > 0.0 && ((int) spo2.spo2Percent) < 126){
			//sleeping
			Json::Value val;
			jsonWriter("spo2",spo2.spo2Percent,val);
			jsonWriter("bpm",spo2.pulseRate,val);
			//stringstream aaa;
			//aaa << "OxySmart spo2 is " << spo2.spo2Percent <<  " bpm is " << spo2.pulseRate;
			string out=jsonOutput(val);
			ROS_INFO("%s size %d",out.c_str(),out.length());
			msg.data=out;
			__service_provider.publish(msg);
			ROS_INFO("message sent");
			isFresh = false;
			sleep(1);
		}
	}
}

bool SPDriver::checkDeviceConnection(){
	//stub
	return true;
}


bool SPDriver::InitialiseService(smanager::ServiceRequest::Request &req,
 smanager::ServiceRequest::Response &res){
	//request to start
	if(req.Request.compare("Request")==0){
		if(!initialisingDevice()){
			ROS_INFO("failed to initialise");
			res.Response="notConnected";
			return true;
		}
		res.Response="Ready";
		ROS_INFO("READY");
		ros::NodeHandle n;
		stringstream aaa;
		aaa<<__id;
		string name=__type+"_"+aaa.str()+"_data";
		__service_provider=n.advertise<smanager::data>(name,1000);
		ROS_INFO("the service is \"%s\"",name.c_str());
		__provide_Service=true;
	//request to stop	
	}else if(req.Request.compare("Stop")==0){
		ROS_INFO("STOOPPING");
		stopMeasurement();
	//request to end session
	}else{
		res.Response="Terminate";
		ROS_INFO("Terminated");
		terminateService();
		ROS_INFO("closing connection");
		spo2.closeConnection();
	}
	 return true;
 }


bool SPDriver::is_Started(){
	if(__provide_Service==true){
		__provide_Service=false;
		__processing=true;
		return true;
	}else{
		return false;
	}
}


bool SPDriver::terminateService(){
	__processing=false;
	return true;
}


bool SPDriver::initialisingDevice(){
	//initialise the device
	spo2.initialiseDevice();
	if(!spo2.Connected){
		ROS_INFO("error-please reconnect the device");
		return false;
	}
	ROS_INFO("initialization success");
	return true;
}

void* SPDriver::updateReadingsThread(void* threadid){
	ROS_INFO("start reading");
	SPDriver* cm=(SPDriver*)threadid;
	cm->updateReadings();
	ROS_INFO("finished reading");
	pthread_exit(NULL);
}

void SPDriver::updateReadings(){
	ROS_INFO("entering loop");
	while(__processing){
		spo2.updateReadings();
		isFresh = true;
	}
}

void SPDriver::stopMeasurement(){
	terminateService();
}
