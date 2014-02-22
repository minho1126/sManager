#include <iostream>
#include <string.h>
#include <vector>	
#include "ros/ros.h"
#include "OxySmartSpO2.h"
#include "Drivers/Checker.h"
#include "Drivers/ServiceRequest.h"
#include "Jsoncpp/json/writer.h"
using namespace std;

class ConnectionManager{
	public:
		ConnectionManager();
		bool Register(ros::ServiceClient client);//register to the manager
		bool RenewRegisteration(Drivers::Checker::Request &req, Drivers::Checker::Response &res);
		virtual void StartService()=0;
		virtual bool checkDeviceConnection()=0;//checks device connection
		unsigned int generateID();
		string getServiceName();
		virtual bool InitialiseService(Drivers::ServiceRequest::Request &req,Drivers::ServiceRequest::Response &res)=0;
		bool is_Started();
		int pid;
		virtual bool terminateService()=0;
		virtual bool initialisingDevice()=0;
		void jsonWriter(string name,double d,Json::Value& val);
		void jsonWriter(string name,string s,Json::Value& val);
		void jsonWriter(string name,int i,Json::Value& val);
		string jsonOutput(Json::Value val);
		virtual void stopMeasurement()=0;
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
