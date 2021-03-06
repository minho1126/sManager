#pragma once
#include <iostream>
#include <string.h>
#include <vector>	
#include "ros/ros.h"
#include "OxySmartSpO2.h"
#include "smanager/Checker.h"
#include "smanager/ServiceRequest.h"
#include "Jsoncpp/json/writer.h"
using namespace std;

class ConnectionManager{
	public:
		ConnectionManager();
		bool Register(ros::ServiceClient client);//register to the manager
		bool RenewRegisteration(smanager::Checker::Request &req, smanager::Checker::Response &res);
		virtual void StartService()=0;
		virtual bool checkDeviceConnection()=0;//checks device connection
		unsigned int generateID();
		string getServiceName();
		virtual bool InitialiseService(smanager::ServiceRequest::Request &req,smanager::ServiceRequest::Response &res)=0;
		virtual bool is_Started()=0;
		int pid;
		virtual bool terminateService()=0;
		virtual bool initialisingDevice()=0;
		void jsonWriter(string name,double d,Json::Value& val);
		void jsonWriter(string name,string s,Json::Value& val);
		void jsonWriter(string name,int i,Json::Value& val);
		string jsonOutput(Json::Value val);
		virtual void stopMeasurement()=0;
		string portScanner();
		string getRequestname();
	protected:
		bool isFresh;
		bool isConnected;
		ros::Publisher __service_provider;
		string __name;
		string __type;
		unsigned int __id;
		unsigned int __bid;
		string __port;
		string __manager_Id;
		string __ipAddress;
		bool __provide_Service;
		bool __processing;
		ros::ServiceServer checker;
		bool __pause;
};
