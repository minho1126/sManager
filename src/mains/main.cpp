#include <iostream>
#include "OxySmartSpO2.h"
#include <pthread.h>
#include <cstdlib>
#include <cstdio>
using namespace std;

using namespace std;

OxySmartSpO2 spo2;
bool isConnected = false;
bool isFresh = false;
bool InitProc();
char userTag[16] = {0,};
unsigned int uTagNum = 0;

// not sure if this is a problem
// but the threads solve the issue with Simbugger where it lags reading all the messages at the speed they are produced.
// by having threads the values are polled by a second thread and only sent sometimes.
// this means not all values are used.....
// however i do not know if ROCOS on the robots has the same lag issues as Simbugger 
//HANDLE oxySmartThread = 0;
void* updateReadingsThread(void* threadid);


int main(int argc, char * argv[])
{
	// hide window
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	
	//bool isInited = InitProc();
	//while(true)
	//{
	//	::Sleep(100);
	//	if(isConnected == true)
	//		break;
	//}

	//CCommandSet csWhoIam = CORDy::Shared::CCommandSet("WhoIam", NULL, "OxySmart", "RVM", CORDy::Shared::REQUEST_MESSAGE, 0,0,0,NULL);
	//sending message to client
	//RAPIX_Client->outbound(&csWhoIam);


	spo2.initialiseDevice();

	if(!spo2.Connected)
	{
		//CCommandSet csEvt = CCommandSet("OxySmartConnectionError", NULL, "OxySmart", "RCP", EVENT_MESSAGE, 0,0,0,NULL);
		//RAPIX_Client->outbound(&csEvt);
		cout<<"error-please reconnect the device"<<endl;
		exit(0);
		return 0;
	}
	
	// start the thread reading from the sensor
	//CreateThread(NULL, 0, updateReadingsThread, &spo2, 0, NULL);
	pthread_t threads[1];  
	pthread_create(&threads[0],NULL,updateReadingsThread,(void *)1);
	while(true){
		// replaced with thread
		// spo2.updateReadings();
		
		// slows the loop speed and stops calls to cout when no new data has been read.
		// because of a lack of locks there is a chance for some one off reading to be missed
		// this is not a problem as the next reading will be used.
		if(!isFresh){
			usleep(1000);
			continue;
		}
		cout << "OxySmart spo2 is " << spo2.spo2Percent <<  " bpm is " << spo2.pulseRate << endl;
		isFresh = false;
		if(spo2.pulseRate > 0 && spo2.spo2Percent > 0.0 && ((int) spo2.spo2Percent) < 126)
		{
			//CArgumentList argList;
			//argList.addDoubleArg("spo2",spo2.spo2Percent);
			//argList.addDoubleArg("bpm",spo2.pulseRate);
			cout<<"sleeping"<<endl;
			//CCommandSet csEvt = CCommandSet("OxySmartResult",&argList,"OxySmart","NA",EVENT_MESSAGE,0,0,0,NULL);
			//RAPIX_Client->outbound(&csEvt);
			// sleep to give ROCOS a break
			// if updating is not threaded do not sleep, will cause huge lag in reported values
			sleep(1);
		}
	}

	return 0;
}

void ConnectionManager::*updateReadingsThread(void* threadid){
	while(true){
		spo2.updateReadings();
		isFresh = true;
	}
}


/*
void RAPIxProc(CCommandSet *cmdset )
{
	if(cmdset != NULL) 
	{
		std::string cmdName = cmdset->getCmdName();
		CArgumentList * argListPtr = cmdset->getCmdArgumentListPtr();
		CORDy::Shared::MessageType msgType = cmdset->getMessageType();
					
		if(msgType == CORDy::Shared::EVENT_MESSAGE)
		{
			CORDy::Shared::CCommandSet* RespCmdSet = NULL;
			CORDy::Shared::CArgumentList* respArgList = NULL;
			unsigned int result = 0xffffffff;
		}
	}
}
*/
/*
void ConnProc(std::string addr)
{
	isConnected = true;
}
*//*
bool InitProc()
{
	//add your initial procedures.
	RAPIX_Client = RIML::Shared::RIMLClient::createInstance(4002);
	if(RAPIX_Client != NULL)
	{
		RAPIX_Client->regInboundProc(&RAPIxProc);
		RAPIX_Client->regConnProc(&ConnProc) ;
		RIML::Shared::Result result = RAPIX_Client->connect("127.0.0.1",6001);
		if(result == RIML::Shared::RIML_SUCCESS)
			isConnected = true;
	}
	else
		return false;
	
	return true;
}*/
