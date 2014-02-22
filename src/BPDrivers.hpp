#include <iostream>
#include <string.h>
#include <vector>	
#include "ros/ros.h"
#include "Pulsecor.h"
#include "ConnectionManager.hpp"
#include "Drivers/Checker.h"
#include "Drivers/ServiceRequest.h"
#include "Jsoncpp/json/writer.h"
using namespace std;

class BPDriver:public ConnectionManager{
	public:
		BPDriver();
		void StartService();//needed
		bool checkDeviceConnection();//needed
		int pid;
		static void* updateReadingsThread(void* threadid);
		void updateReadings();
		bool terminateService();//needed
		bool is_Started();
		bool InitialiseService(Drivers::ServiceRequest::Request &req,Drivers::ServiceRequest::Response &res);//needed
		bool initialisingDevice();//needed
		void stopMeasurement();//needed
	private:
		Pulsecor pulsecor;
};
