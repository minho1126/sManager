#include "ConnectionManager.hpp"
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

ConnectionManager::ConnectionManager(){
	//__name="OxySmartSp02";
	//__type="SPO2";//type can only be SPO2, BP
	//__ipAddress="127.0.0.1";//example
	//__id =generateID();
	//__bid=1;//example
	//__provide_Service=false;
	//__pause=false;
	//isFresh=false;
	//isConnected=false;
}

unsigned int ConnectionManager::generateID(){
	srand(time(NULL));
	unsigned int a=rand();
	unsigned int b=rand();
	while(a== 0)a=rand()%1000;//if random is 0 loop
	while(b== 0)b=rand()%1000;//if random is 0 loop
	ROS_INFO("id is %d",a*b);
	return a*b;
}

bool ConnectionManager::Register(ros::ServiceClient client){
	smanager::RegisterMessage srv;
	srv.request.Name=__name;
	srv.request.type=__type;
	srv.request.ipAddress=__ipAddress;
	srv.request.id=__id;
	srv.request.bid=__bid;
	bool result=true;
	while(1){
		if(client.call(srv)){
			//recieved reply back from the sensor manager
			if(srv.response.manager_Id==-1){
				//invalid id-regenerate id
				__id=generateID();
				srv.request.id=__id;
				continue;
			}else{
				//valid id
				ROS_INFO("itworked");
				__manager_Id=srv.response.manager_Id;
				result=true;
				//setup ping checker
				ros::NodeHandle n;
				stringstream aaa;
				aaa<<__type<<"_"<<__id<<"_check";
				checker=n.advertiseService(aaa.str(),&ConnectionManager::RenewRegisteration,this);
				break;
			}
		}else{
			//trouble with connecting to sensor manager
			ROS_INFO("trouble with connection-please try again");
			result=false;
			break;
		}
		
	}
	return result;
	
}

bool ConnectionManager::RenewRegisteration(smanager::Checker::Request &req,
 smanager::Checker::Response &res){
	//check if device is connected
	if(checkDeviceConnection()){
		res.check=1;
	}else{
		res.check=0;
	}
	return true;
}

string ConnectionManager::getServiceName(){
	stringstream convert;
	convert<<__id;
	string result= __type+"_"+__name+"_"+convert.str();
	return result;
}

void ConnectionManager::jsonWriter(string name,double d,Json::Value& val){
	val[name]=d;
}

void ConnectionManager::jsonWriter(string name,int i, Json::Value& val){
	val[name]=i;
}

void ConnectionManager::jsonWriter(string name, string s, Json::Value& val){
	val[name]=s;
}

//output
string ConnectionManager::jsonOutput(Json::Value val){
	// write in a nice readible way
	Json::StyledWriter styledWriter;
	return styledWriter.write(val);
}
