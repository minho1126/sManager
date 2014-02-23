#include <iostream>
#include <string.h>
#include <vector>	
#include "ros/ros.h"
#include "OxySmartSpO2.h"
#include "ConnectionManager.hpp"
#include "smanager/Checker.h"
#include "smanager/ServiceRequest.h"
#include "Jsoncpp/json/writer.h"
using namespace std;

class SPDriver:public ConnectionManager{
	public:
		SPDriver();
		void StartService();
		bool checkDeviceConnection();//checks device connection
		int pid;
		static void* updateReadingsThread(void* threadid);
		void updateReadings();
		bool terminateService();
		bool is_Started();
		bool InitialiseService(smanager::ServiceRequest::Request &req,smanager::ServiceRequest::Response &res);
		bool initialisingDevice();
		void stopMeasurement();
	private:
		OxySmartSpO2 spo2;
};
