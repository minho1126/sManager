#include <iostream>
#include <string.h>
#include <vector>	
#include "ros/ros.h"
#include "OxySmartSpO2.h"
#include "ConnectionManager.hpp"
#include "Drivers/Checker.h"
#include "Drivers/ServiceRequest.h"
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
		bool InitialiseService(Drivers::ServiceRequest::Request &req,Drivers::ServiceRequest::Response &res);
		bool initialisingDevice();
		void stopMeasurement();
	private:
		OxySmartSpO2 spo2;
};
