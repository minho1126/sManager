#include <ros/ros.h>
#include "smanager/Checker.h"
#include "smanager/RegisterMessage.h"
#include "smanager/ServiceRequest.h"
#include "smanager/data.h"
#include <map>
#include <string>

using namespace std;

enum status{READY,IN_USE,REQUESTED,ERROR,PAUSE,INVALID};

struct deviceInfo{
	enum status stat;
	string Servicename;
	unsigned int id; 				//service id
	unsigned int bid;				//beaglebone id
	bool operator ==(deviceInfo dI);
};

class SensorManager{
	public:
		SensorManager();
		unsigned int RegisterSensor(smanager::RegisterMessage::Request req);
		bool foo(smanager::RegisterMessage::Request &req,smanager::RegisterMessage::Response &res);
		string XmlParser();
		bool listenForRequest(smanager::ServiceRequest::Request &req, smanager::ServiceRequest::Response &res);
		int startService(deviceInfo& dv,int start=0);
		void terminate(deviceInfo& dv);
		deviceInfo& findAvailable(string type);
		void listSensors();
		void listActiveServices();
		deviceInfo& initialiseService(ros::NodeHandle n,string type);
		void Resultcallback(smanager::data msg);
		void Ping();
		void Deregister(string type,unsigned int id);
		void pause(deviceInfo& dv);
	private:
		int manager_Id;
		map<string, map<unsigned int,deviceInfo> >type_map;//<BP ,<id,device>>
		map<unsigned int,ros::Subscriber> service_map;
		ros::Subscriber tempsub;
};


