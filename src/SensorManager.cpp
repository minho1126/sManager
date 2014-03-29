#include <iostream>
#include "SensorManager.hpp"
#include <cstdlib>
#include <cstdio>
#include <ros/ros.h>
#include "smanager/Checker.h"
#include "smanager/RegisterMessage.h"
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include "smanager/ServiceRequest.h"
#include "smanager/data.h"

using namespace std;

SensorManager::SensorManager(){
	//to be assigned via file --later
	manager_Id=1;//default case--change later
}
//-----------------------------------------------------registering sensors-------------------------------------------------------
unsigned int SensorManager::RegisterSensor(smanager::RegisterMessage::Request req){
	if(type_map.find(req.type)==type_map.end()){
		type_map[req.type]=map<unsigned int,deviceInfo>();
	}
	if(type_map[req.type].find(req.id)==type_map[req.type].end()){
		//create device info and register
		deviceInfo dv=deviceInfo();
		dv.stat=READY;
		dv.Servicename=req.type;
		dv.id=req.id;
		dv.bid=req.bid;
		type_map[req.type][req.id]=dv;
		ROS_INFO("registered %s sensor service id: %d",req.type.c_str(),req.id);
		ROS_INFO("the size now is %d", type_map[req.type].size());
		//done and return true
		return 1;
	}else{
		//sensor exists
		return 0;
	}
	return 1;
}

bool SensorManager::foo(smanager::RegisterMessage::Request &req,smanager::RegisterMessage::Response &res){
	if(RegisterSensor(req)){
		//if true
		res.manager_Id=1;
		return true;
	}else{
		//if false
		res.manager_Id=-1;
		return true;
	}
}
//-----------------------------------------------------registering sensors-------------------------------------------------------
//handles the requests from the client
bool SensorManager::listenForRequest(smanager::ServiceRequest::Request &req, smanager::ServiceRequest::Response &res){
	ROS_INFO("listening to requests");
	//request for sensor
	if(req.Request.compare("Request")==0){
		ROS_INFO("REQUESTING");
		ros::NodeHandle n;
		deviceInfo& dv=initialiseService(n,req.type,req.bid);
		if(dv.stat==REQUESTED){
			ROS_INFO("request suceesss");
			res.bid=dv.bid;
			usedBID.push_back(dv.bid);
			res.id=dv.id;
			res.uid=req.uid;
		}else{
			ROS_INFO("request failed...");
			res.bid=-1;
			res.id=-1;
			res.uid=-1;
		}
		return true;
	//user starts the sensor
	}else if(req.Request.compare("Start")==0){
		ROS_INFO("STARTING");
		int success=startService(type_map[req.type][req.id]);
		if(success==1){
			res.Response="Success";
			res.type=req.type;
		}else{
			res.Response="Failure";
			res.type=req.type;
			//res.type=;
		}
		return true;
	//user is stopping the service and ends the session
	}else if(req.Request.compare("Terminate")==0){
		ROS_INFO("terminating");
		contains(type_map[req.type][req.id].bid,true);
		terminate(type_map[req.type][req.id]);
		return true;
	//user stops the service but not ending the session
	}else if(req.Request.compare("Stop")==0){
		ROS_INFO("Stopping");
		pause(type_map[req.type][req.id]);
		return true;
	}else{
		ROS_INFO("HELLO HOW YOU REACHED HERE");
		return false;
	}
}
//-----------------------------------------------------termination-------------------------------------------------------
void SensorManager::terminate(deviceInfo& dv){
	startService(dv,1);
	service_map[dv.id].shutdown();
	service_map.erase(dv.id);
	dv.stat=READY;
}
//-----------------------------------------------------start service-------------------------------------------------------
//deviceinfo& a =map...
//sm.startService(a)
//0--start
//1-terminate
//2-stop
//3-other function
//
//return 1 for good and return 0 for bad
int SensorManager::startService(deviceInfo& dv,int start,string command){
	if(dv.stat==REQUESTED && start==0)
		dv.stat=IN_USE;
	ros::NodeHandle m;
	//create call to driver
	ROS_INFO("%s is device info",dv.Servicename.c_str());
	if(dv.Servicename.length()==0){
		ROS_INFO("nothing selected");
		return 0;
	}
	stringstream ss;
	ss<<dv.Servicename<<"_request_"<<dv.id;
	string requestname=ss.str();
	ros::ServiceClient ServiceRequest=m.serviceClient<smanager::ServiceRequest>(requestname);
	ROS_INFO("registering to \"%s\"",requestname.c_str());
	smanager::ServiceRequest srv;
	//what is the request
	if(start==0){
		ROS_INFO("request is request");
		srv.request.Request="Request";
	}else if(start==1){
		ROS_INFO("request is terminate");
		srv.request.Request="Terminate";
	}else if(start==2){
		ROS_INFO("request is pause");
		srv.request.Request="Stop";
	}else if(start==3 && command.length()!=0){
		ROS_INFO("request is default");
		srv.request.Request=command;
	}else{
		ROS_INFO("unidetified request");
		return 0;
	}
	//must be in use to call (redundant)
//	if(dv.stat==IN_USE){
		ROS_INFO("calling");
		if(ServiceRequest.call(srv)){
			ROS_INFO("starting request id:%d",dv.id);
			if(srv.response.Response.compare("Ready")==0)
				return 1;
			else{
				ROS_INFO("failed reason: %s",srv.response.Response.c_str());
				return 0;
			}
		}else{
			ROS_INFO("connection failed id:%d",dv.id);
			return 0;
		}
//	}
	ROS_INFO("error may not be allocated");
	return 0;
}

string SensorManager::XmlParser(){
	TiXmlDocument doc("deviceList.xml");
	if(!doc.LoadFile()) return ""; 
	TiXmlHandle docHandle( &doc );
	TiXmlElement* child = docHandle.FirstChild( "devices" ).FirstChild( "measure" ).ToElement();//.FirstChild( "ports" ).FirstChild( "port" ).ToElement();
	//int port;
	//string state;
	for( child; child; child=child->NextSiblingElement() )
	{
		//port = atoi(child->Attribute( "value"));
		string type=child->Attribute( "type");
		if (type=="BP"){
			//cout << "port: " << port << " is filtered! " << endl;
			cout<<"BP"<<endl;
			return "BP";
		}else{
			//cout << "port: " << port << " is unfiltered! " << endl;
			cout<<"oxysmart"<<endl;
			return "oxysmart";
		}
	}
	return "";
}

bool deviceInfo::operator== (deviceInfo dI){
	if(Servicename.compare(dI.Servicename)==0 && id == dI.id)
		return true;
	else
		return false;
}
//-----------------------------------------------------listing sensors-------------------------------------------------------
void SensorManager::listSensors(){
	map<string,map<unsigned int,deviceInfo> >::iterator iter;
	map<unsigned int,deviceInfo>::iterator iter2;
	for(iter=type_map.begin();iter !=type_map.end();++iter){
		ROS_INFO("for sensor %s",iter->first.c_str());
		for(iter2=iter->second.begin();iter2 != iter->second.end();++iter2){
			ROS_INFO("service id:%d, sensor bid:%d",iter2->second.id, iter2->second.bid);
		}
	}
}

void SensorManager::listActiveServices(){
	map<unsigned int,ros::Subscriber>::iterator iter;
	for(iter=service_map.begin();iter != service_map.end();++iter){
		ROS_INFO("service id:%d",iter->first);
	}
}
//--------------------------------------------------finding available sensors-------------------------------------------------------
deviceInfo& SensorManager::findAvailable(string type,int bid){
	//map<unsigned long,deviceInfo> temp=type_map[type];
	listSensors();
	map<unsigned int,deviceInfo>::iterator iter;
	//ROS_INFO("map size is %d for %s", type_map[type].size(),type.c_str());
	ROS_INFO("Requesting for available %s sensor with %d",type.c_str(),bid);
	for(iter=type_map[type].begin();iter !=type_map[type].end();++iter){
		ROS_INFO("aaa");
		//if status is ready to use and not int the usedbid list
		if(iter->second.stat==READY && !contains(bid)){//
			//if any or as specified sensor
			if((bid==0 && iter->second.bid!=0)|| (iter->second.bid==bid && iter->second.bid!=0)){
				deviceInfo& dv=iter->second;
				ROS_INFO("allocating sensor id:%u",dv.id);
				return dv;
			}
		}
		ROS_INFO("bid may be in use");
	}
	deviceInfo inv;
	inv.stat=INVALID;
	deviceInfo& dv=inv;
	ROS_INFO("could not allocate an available sensor"); 
	return dv;
}
//-------------------------------------------------------mark test to be available-------------------------------------------------------
deviceInfo& SensorManager::initialiseService(ros::NodeHandle n,string type, int bid){
	//alocateService
	deviceInfo& dv=findAvailable(type,bid);
	if(dv.stat==INVALID){
		dv.stat=INVALID;
		ROS_INFO("couldnt find anything failed to init");
		return dv;
	}
	dv.stat=REQUESTED;
	stringstream aaa;
	aaa<<dv.id;
	string name=dv.Servicename+"_"+aaa.str()+"_data";
	//create a subscriber
	ros::Subscriber sub=n.subscribe(name,1000,&SensorManager::Resultcallback,this);
	ROS_INFO("%s",name.c_str());
	//store the subsriber to active service list
	service_map[dv.id]=sub;
	return dv;
}
//---------------------------------------------------------------------------------------------------------------------
//for testing purpose
void SensorManager::Resultcallback(smanager::data msg){
	ROS_INFO("called");
	ROS_INFO("%s",msg.data.c_str());
	ROS_INFO("string length %d",msg.data.length());
}

//removes the device from the map
void SensorManager::Deregister(string type,unsigned int id){
	type_map[type].erase(id);
}
//--------------------------------------checking if a device is still connected to the manager------------------------------------------
void SensorManager::Ping(){
	ros::NodeHandle n;
	map<string,map<unsigned int,deviceInfo> >::iterator iter;
	map<unsigned int,deviceInfo>::iterator iter2;
	ROS_INFO("typemap size %d",type_map.size());
	for(iter=type_map.begin();iter !=type_map.end();++iter){
		if(iter->second.empty()){
			ROS_INFO("there are no %s device connected to the manager",iter->first.c_str());
			type_map.erase(iter->first);
			continue;
		}
		for(iter2=iter->second.begin();iter2 != iter->second.end();++iter2){
			ROS_INFO("pinging service id:%d, sensor bid:%d",iter2->second.id, iter2->second.bid);
			stringstream aaa;
			aaa<<iter->first<<"_"<<iter2->second.id<<"_check";//spo2_23840432_check
			ros::ServiceClient alive=n.serviceClient<smanager::Checker>(aaa.str());
			smanager::Checker ch;
			if(alive.call(ch)){
				//skip
				ROS_INFO("alive");
				continue;
			}else{
				ROS_INFO("nothere");
				//deregister
				contains(iter2->second.bid,true);
				Deregister(iter->first,iter2->second.id);
			}
		}
	}
}


void SensorManager::pause(deviceInfo& dv){
	if(dv.stat==PAUSE){
		dv.stat=IN_USE;
	}else{
		dv.stat=PAUSE;
	}
	ROS_INFO("stopping...part2 %s",dv.Servicename.c_str());
	startService(dv,2);
}

bool SensorManager::contains(int bid,bool deleteMode){
	ROS_INFO("current size is %d",usedBID.size());
	for(int i=0;i<usedBID.size();i++){
		if(usedBID[i]==bid){
			if(deleteMode){
				//ROS_INFO("contained");
				usedBID.erase(usedBID.begin()+i);
			}
			return true;
		}
	}
	//ROS_INFO("not contained");
	return false;
}

/*
bool SensorManager::RegisterUser(unsigned int uid){
	map<unsigned int, userInfo>::iterator iter;
	iter=userlist.find(uid);
	if(iter==userlist.end()){
		return false;
	}
	return true;
}


void SensorManager::deregisterUser(unsigned int uid){
	userlist.erase(uid);
	return;
}

void counter(userInfo ui){
	while(ui.ttl>0){
		usleep(100,000);
		ui.ttl=ui.ttl-0.1;
	}
	deregisterUser(ui.id);
	
}
*/
