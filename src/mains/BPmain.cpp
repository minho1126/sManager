#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include "Pulsecor.h"

unsigned int BP_Thread =0;
void *doPressureTest(void* threadid);

int ThreadData = 1;
int x=-1;
//End of thread stuff

bool isConnected = false;
//bool InitProc();
char userTag[16] = {0,};
unsigned int uTagNum = 0;

//RIMLClient* RAPIX_Client;
Pulsecor pulsecor;


void ConnProc(std::string addr)
{
	isConnected = true;
}


void* doPressureTest(void* lpParam)
{
	int PrevReading = -10;
	bool simaphore = false;

	int connectionStatus = 0;
	if (pulsecor.connectToDevice(1)>0)
	{
		connectionStatus = 1;		
	}
	else
	{
		printf("Connection Failed...\n");
		connectionStatus = 0;
	}
	
	int previousStatusCode = -1;
	while(connectionStatus == 1)
	{
		sleep(3);
		//update current Mode
//		printf("this is current status %d\n",pulsecor.status.currentMode);
//		pulsecor.status.currentMode=x;
		if (pulsecor.status.measurementCompleted == 1)
		{
			//results
			cout<<"Results are as follows, systolic pressure is "<<pulsecor.result.systolicPres;
			cout<<", diastolic pressure is "<<pulsecor.result.diastolicPres;
			cout<<", pulse rate is "<<pulsecor.result.pulseRate;
			cout<<" and augmentation index is "<<pulsecor.result.augmentationIndex<<endl;
			
//			argList.addIntArg("systolicPres",pulsecor.result.systolicPres);
//			argList.addIntArg("diastolicPres",pulsecor.result.diastolicPres);
//			argList.addIntArg("pulseRate",pulsecor.result.pulseRate);
//			argList.addIntArg("augmentationIndex",pulsecor.result.augmentationIndex);
			
//			CCommandSet csEvt = CCommandSet("PulseCoreResult",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
			
			break;
		}
		
		//chandimal
		if (pulsecor.status.measurementError == 1)
		{
			printf("Measurement Error");
			if (pulsecor.status.finishedMode == 0) {
				printf("Error code (#%d). \n", pulsecor.status.finishedMode);
				pulsecor.cancelMeasurement();
//				argList.addStringArg("Status", "Error");
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
				break;
			}
		}

		if (pulsecor.status.currentMode >2 && pulsecor.status.currentMode<6)
		{
			printf("The current pressure is %d\n", pulsecor.status.currentPressure);
			if(PrevReading == -10) PrevReading = pulsecor.status.currentPressure;
			if(PrevReading < pulsecor.status.currentPressure && !simaphore)
			{
				simaphore = true;
				printf("Inflating...\n");
			}

			if(PrevReading > pulsecor.status.currentPressure && simaphore)
			{
				simaphore = false;
				printf("Deflating...\n");
			}

			PrevReading = pulsecor.status.currentPressure;
		}

		if (pulsecor.status.currentMode == 2) {
			printf("Error code (#2). Canceling measurement\n");
			pulsecor.cancelMeasurement();
			
//			argList.addStringArg("Status", "Error");
//			CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
			break;
		}
		else if (pulsecor.status.currentMode == 3) {
			//Begin inflation
			printf("Base measurement (#3)\n");

			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "Measuring");
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
				printf("currently measuring\n");
			}
		}
		else if (pulsecor.status.currentMode == 4) {
			//Deflating - ready for second inflation
			printf("Deflating (#4)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "Deflating");
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
			}
		}
		else if (pulsecor.status.currentMode == 5) {
			//Begin second inflation
			printf("Inflating to measure suprasystolic (#5)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "SecondInflation");
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
				printf("currently measuring-second inflation\n");
			}
		}
		else if (pulsecor.status.currentMode == 6) {
			//Hold second inflation
			printf("Measuring suprasystolic results (#6)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "Suprasystolic");
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
			}
		}
		else if (pulsecor.status.currentMode == 7) {
			//Measurement finished. Leading up to pulsecore result
			printf("Measurements done - calculating (#7)\n");
			
			if (pulsecor.status.currentMode != previousStatusCode) {
//				argList.addStringArg("Status", "Calculating");
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
			}
		}
		else
		{
			printf("Unknown current status - %d\n", pulsecor.status.currentMode);
			printf("Unknown finished status - %d\n", pulsecor.status.finishedMode);

			if (pulsecor.status.currentMode != previousStatusCode) {
//				CArgumentList argList;
//				argList.addStringArg("Status","Unknown");
//				CCommandSet csEvt = CCommandSet("PulseCoreStatus",&argList,"PulseCor","NA",EVENT_MESSAGE,0,0,0,NULL);
//				RAPIX_Client->outbound(&csEvt);
			}
		}

		previousStatusCode = pulsecor.status.currentMode;
	}

	//exit(0);
	return 0;
}

int main()
{
	//BP_Thread = CreateThread(NULL, 0, doPressureTest, &ThreadData, 0, NULL);  
	pthread_t threads[1];  
	pthread_create(&threads[0],NULL,doPressureTest,(void *)1);

	//added by chandimal
	while(true){
		printf("Please enter any number..");
		std::cin >> x;
		printf("You entered %d ",x);
	}
	return 0;
}

