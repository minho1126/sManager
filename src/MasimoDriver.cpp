#include "MasimoDriver.hpp"
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

//#include "jsoncpp-src-0.50/"
using namespace std;

MasimoDriver::MasimoDriver(){
	__name="OxySmartSp02";
	__type="Masimo";//type can only be SPO2, BP
	__ipAddress="127.0.0.1";//example
	__id =generateID();
	__bid=1;//example
	__provide_Service=false;
	__pause=false;
	isFresh=false;
	isConnected=false;
}

void MasimoDriver::StartService(){
	smanager::data msg;
	pthread_t t; 
	int* i;
	pthread_create(&t,NULL,&MasimoDriver::updateReadingsThread,this);
	ROS_INFO("ENTERING WHILE LOOP");
	while(__processing){
		if(!isFresh){
				//ROS_INFO("Sleepping...");
				usleep(1000);
				continue;
		}
		//sleep(1);
		if(spo2.bpm > 0 && spo2.SPO2 > 0.0 && ((int) spo2.SPO2) < 126){
			//sleeping
			Json::Value val;
			jsonWriter("spo2",spo2.SPO2,val);
			jsonWriter("bpm",spo2.bpm,val);
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

bool MasimoDriver::checkDeviceConnection(){
	//stub
	return true;
}


bool MasimoDriver::InitialiseService(smanager::ServiceRequest::Request &req,
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


bool MasimoDriver::is_Started(){
	if(__provide_Service==true){
		__provide_Service=false;
		__processing=true;
		return true;
	}else{
		return false;
	}
}


bool MasimoDriver::terminateService(){
	__processing=false;
	return true;
}


bool MasimoDriver::initialisingDevice(){
	//initialise the device
	spo2.InitialiseDevice1();
	if(!spo2.Connected){
		ROS_INFO("error-please reconnect the device");
		return false;
	}
	ROS_INFO("initialization success");
	return true;
}

void* MasimoDriver::updateReadingsThread(void* threadid){
	ROS_INFO("start reading");
	MasimoDriver* cm=(MasimoDriver*)threadid;
	cm->updateReadings();
	ROS_INFO("finished reading");
	pthread_exit(NULL);
}

void MasimoDriver::updateReadings(){
	ROS_INFO("entering loop");
	while(__processing){
		spo2.updateReadings();
		isFresh = true;
	}
}

void MasimoDriver::stopMeasurement(){
	terminateService();
}
