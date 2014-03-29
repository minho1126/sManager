#include "BPDrivers.hpp"
//#include "Pulsecor.h"
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

BPDriver::BPDriver(){
	__name="Pulsecor";
	__type="BP";//type can only be SPO2, BP
	__ipAddress="127.0.0.1";//example
	__id =generateID();
	//read bid
	std::ifstream infile;
	infile.open("~/settings.txt");
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

void BPDriver::StartService(){
	//intialise
	sleep(1);
	smanager::data msg;
	int PrevReading = -10;
	bool simaphore = false;
	stringstream out;
	int previousStatusCode = -1;
	Json::Value val;
	jsonWriter("Status","PulseCoreConnectionOK",val);
	msg.data=jsonOutput(val);
	ROS_INFO("%s",msg.data.c_str());
	__service_provider.publish(msg);
	while(__processing)
	{
		if (pulsecor.status.measurementCompleted == 1)
		{
			//results
			//out<<"Results are as follows, systolic pressure is "<<pulsecor.result.systolicPres;
			//out<<", diastolic pressure is "<<pulsecor.result.diastolicPres;
			//out<<", pulse rate is "<<pulsecor.result.pulseRate;
			//out<<" and augmentation index is "<<pulsecor.result.augmentationIndex<<endl;
			jsonWriter("Status","PulseCoreResult",val);
			jsonWriter("systolicPres",pulsecor.result.systolicPres,val);
			jsonWriter("diastolicPres",pulsecor.result.diastolicPres,val);
			jsonWriter("pulseRate",pulsecor.result.pulseRate,val);
			jsonWriter("augmentationIndex",pulsecor.result.augmentationIndex,val);
			string mesg=jsonOutput(val);
			ROS_INFO("%s",mesg.c_str());
			msg.data=mesg;
			__service_provider.publish(msg);			
			break;
		}
		
		//chandimal
		if (pulsecor.status.measurementError == 1)
		{
			ROS_INFO("Measurement Error");
			if (pulsecor.status.finishedMode == 0) {
				printf("Error code (#%d). \n", pulsecor.status.finishedMode);
				pulsecor.cancelMeasurement();
				//argList.addStringArg("Status", "Error");
				jsonWriter("Status","Error",val);
				string out=jsonOutput(val);
				msg.data=out;
				__service_provider.publish(msg);
				break;
			}
			pulsecor.status.measurementError = 0;
		}

		if (pulsecor.status.currentMode >2 && pulsecor.status.currentMode<6)
		{
			//printf("The current pressure is %d\n", pulsecor.status.currentPressure);
			if(PrevReading == -10) PrevReading = pulsecor.status.currentPressure;
			if(PrevReading < pulsecor.status.currentPressure && !simaphore)
			{
				simaphore = true;
				ROS_INFO("Inflating...\n");
			}

			if(PrevReading > pulsecor.status.currentPressure && simaphore)
			{
				simaphore = false;
				ROS_INFO("Deflating...\n");
			}

			PrevReading = pulsecor.status.currentPressure;
		}

		if (pulsecor.status.currentMode == 2) {
			ROS_INFO("Error code (#2). Canceling measurement\n");
			pulsecor.cancelMeasurement();
			jsonWriter("Status","Error",val);
//			argList.addStringArg("Status", "Error");
			msg.data=jsonOutput(val);
			__service_provider.publish(msg);
			break;
		}
		else if (pulsecor.status.currentMode == 3) {
			//Begin inflation
			//ROS_INFO("Base measurement (#3)\n");

			if (pulsecor.status.currentMode != previousStatusCode) {
				jsonWriter("Status","Measuring",val);
				msg.data=jsonOutput(val);
				__service_provider.publish(msg);
//				argList.addStringArg("Status", "Measuring");
				ROS_INFO("currently measuring\n");
			}
		}
		else if (pulsecor.status.currentMode == 4) {
			//Deflating - ready for second inflation
			ROS_INFO("Deflating (#4)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "Deflating");
				jsonWriter("Status","Deflating",val);
				msg.data=jsonOutput(val);
				__service_provider.publish(msg);
			}
		}
		else if (pulsecor.status.currentMode == 5) {
			//Begin second inflation
			//ROS_INFO("Inflating to measure suprasystolic (#5)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "SecondInflation");
				jsonWriter("Status","SecondInflation",val);
				msg.data=jsonOutput(val);
				__service_provider.publish(msg);
				ROS_INFO("currently measuring-second inflation\n");
			}
		}
		else if (pulsecor.status.currentMode == 6) {
			//Hold second inflation
			//ROS_INFO("Measuring suprasystolic results (#6)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "Suprasystolic");
				jsonWriter("Status","Suprasystolic",val);
				msg.data=jsonOutput(val);
				__service_provider.publish(msg);
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
			}
		}
		else if (pulsecor.status.currentMode == 7) {
			//Measurement finished. Leading up to pulsecore result
			//ROS_INFO("Measurements done - calculating (#7)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "Calculating");
				jsonWriter("Status","Calculating",val);
				msg.data=jsonOutput(val);
				__service_provider.publish(msg);
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
			}
		}
		else
		{
			ROS_INFO("Unknown current status - %d\n", pulsecor.status.currentMode);
			ROS_INFO("Unknown finished status - %d\n", pulsecor.status.finishedMode);

			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status","Unknown");
				jsonWriter("Status","Unknown",val);
				msg.data=jsonOutput(val);
				__service_provider.publish(msg);
			}
		}

		previousStatusCode = pulsecor.status.currentMode;
	}

	//exit(0);
	return;
}

bool BPDriver::checkDeviceConnection(){
	//stub
	return true;
}


bool BPDriver::InitialiseService(smanager::ServiceRequest::Request &req,
 smanager::ServiceRequest::Response &res){
	if(req.Request.compare("Request")==0){
		//initialise goes here
		if(!initialisingDevice()){
			ROS_INFO("failed to initialise");
			res.Response="Failed";
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
	}else if(req.Request.compare("Pause")==0){
		stopMeasurement();
	}else{
		res.Response="Terminate";
		ROS_INFO("Terminated");
		terminateService();
	}
	return true;
 }


bool BPDriver::is_Started(){
	if(__provide_Service==true){
		__provide_Service=false;
		__processing=true;
		return true;
	}else{
		return false;
	}
}


bool BPDriver::terminateService(){
	stopMeasurement();
	__processing=false;
	return true;
}


bool BPDriver::initialisingDevice(){
	//initialise the device
	if(pulsecor.connectToDevice(1)>0)
		return true;
	else
		return false;
}

void BPDriver::stopMeasurement(){
	pulsecor.cancelMeasurement();
}
