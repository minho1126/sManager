#include "Pulsecor.h"
#include "pthread.h"
#include "semaphore.h"
#include "stdio.h"
#include <string>
#include <string.h>
#include <iostream>
#include <cstdlib>

//#include "enumser.h"

// Initialise all variables to -1, so if variable is not changed after initialisation, then it can be tested.
bpResults::bpResults(void){

measurementID= -1;
systolicPres= -1;
meanPres= -1;
diastolicPres= -1;
pulseRate= -1;
augmentationIndex= -1;
signalNoise= -1;
pulseRateSS= -1;
reflectionTime= -1;
systolicEjectionPeriod= -1;
reflectedWaveTransitTime= -1;
contractility= -1;
suprasysOscPulsePres= -1;
suprasysOscPulsePresVar= -1;
pulseRateVar= -1;

}

bpResults::~bpResults(void)
{
}


// Initialise all variables to -1, so if variable is not changed after initialisation, then it can be tested.
currentStatus::currentStatus(void)
{
	finishedMode = -1;
	currentMode = -1;
	currentPressure = -1;
}


currentStatus::~currentStatus(void)
{
}

Pulsecor::Pulsecor(void)
{
//	serialMutex = PTHREAD_MUTEX_INITIALIZER;
	status.measurementCompleted = 0;
	status.measurementError = 0;
}

Pulsecor::~Pulsecor(void)
{
}




char* Mid(char *Sentence, int FromNumber, int ToNumber)
{
    char* newSentence = (char *) malloc(1000);

    if (ToNumber == 0)
    {
        ToNumber = strlen(Sentence);
        FromNumber = strlen(Sentence) - FromNumber;
    }

    ToNumber = ToNumber+FromNumber;

    for (int i = (FromNumber--); i < ToNumber; i++)
    {
        *(newSentence+(i-FromNumber)) = *(Sentence+i);
    }
    *( newSentence + (ToNumber-FromNumber)) = 0;

    return newSentence+1;
}



int Instr(char *SearchString, char *SearchTerm)
{
    int ReturnValue = -1;
    for (int i = 0 ; i <= strlen(SearchString)-strlen(SearchTerm); i++)
    {
        if (SearchTerm == Mid(SearchString, i, strlen(SearchTerm)))
        {
            ReturnValue ++;
        }
    }
    return ReturnValue;
}

/*
This function initiates the connection to the serial port. If the connection is successful return 1, otherwise return -1
*/
int Pulsecor::connectToDevice(int dir)
{
//	std::vector<std::string> friendlyNames;
//	std::vector<unsigned int> ports;
//	if (CEnumerateSerial::UsingSetupAPI1(ports, friendlyNames))
//	{
//		for (size_t i=0; i<ports.size(); i++)
//		  _tprintf(_T("COM%d <%s>\n"), ports[i], friendlyNames[i].c_str());
//	}
	bool Done = serial.initialiseSerialPort(9600);
	if(!Done){
		return -1;
	}
	bool Connected  = false;
	char* zComNum = new char[20];
	std::string zComName = "";
	unsigned long len = 1024;
//    LPTSTR zbuffer = (LPTSTR)malloc(len * sizeof(TCHAR));

//	if(dir == 1)
//	{
//		int comPort = 0;
//		for ( size_t i = 0; i < ports.size(); i++) 
		//while(!Connected && comPort <= 20)
//		{			
//				comPort = ports[i];
				/*printf("Now checking port Number:  %d \n",comPort);
			
				sprintf(zComNum,"COM%d",comPort);
				DWORD result = QueryDosDevice((LPCSTR)zComNum, zbuffer, len);
				if(result == 0)
				{ // failed 
					DWORD err = ::GetLastError();
					if(err == ERROR_INSUFFICIENT_BUFFER)
					{ // reallocate 
						 len *= 2;
						 zbuffer = (LPTSTR)realloc(zbuffer, len * sizeof(TCHAR));
						 continue;
					} // reallocate 
				}
				// Convert a TCHAR string to a LPCSTR
				//CT2CA pszConvertedAnsiString (sPort);
				//zComName = std::string(pszConvertedAnsiString);					
				zComName = zComNum;					
				printf("Now checking port Name (1) :  %s \n",zbuffer);								
				//std::string str2 ("ProlificSerial");*/
//				std::string str2 ("ATEN");
//				size_t found;
				//found=zComName.find(str2);
//				found=friendlyNames[i].find(str2);
				
//				if (found!=std::string::npos)
	if(true){	
		int comPort=0;
		int res = serial.openComPort(comPort);
		if (res != -1) {
			usleep(600000);
			startMeasurement(170);
			//status.currentMode=3;
			sleep(3);
			int ret1 = pthread_create(&read_thread,NULL, &Pulsecor::threadFunction, this);
			sleep(6);								
			if(!(status.currentMode >2 && status.currentMode<6))
			{
				cout<<"canceled"<<endl;
				cancelMeasurement();
				cout<<"error finding comport"<<endl;							
				//comPort += 1;
			}
			else {
				std::cout << " connected to port " <<endl;// comPort << std::endl;
				Connected = true;
			}
		}
	}else{
		Connected = false;
	}  
				//if (Connected)
				//	break;
		//}
	//}
	/*else
	{
		int comPort = 20;
		while(!Connected && comPort >= 3)
		{
				printf("Now checking port Number:  %d \n",comPort);
				sprintf(zComNum,"COM%d",comPort);
				DWORD result = QueryDosDevice((LPCSTR)zComNum, zbuffer, len);
				if(result == 0)
				{ // failed 
					DWORD err = ::GetLastError();
					if(err == ERROR_INSUFFICIENT_BUFFER)
					{ // reallocate 
						 len *= 2;
						 zbuffer = (LPTSTR)realloc(zbuffer, len * sizeof(TCHAR));
						 continue;
					} // reallocate 
				}
				zComName = zbuffer;					
				printf("Now checking port Name (1) :  %s \n",zbuffer);								
				std::string str2 ("ProlificSerial");
				size_t found;
				found=zComName.find(str2);
				if (found!=std::string::npos)
				{		
					int res = serial.openComPort(comPort);
					if (res != -1 ) {
						int ret1 = pthread_create(&read_thread,NULL, &Pulsecor::threadFunction, this);
						Connected = true;
					}
					else comPort -= 1;
				}
				else
				{  
					//free(zbuffer);
					Connected = false;
					comPort -= 1;
				}  
		}

	}*/

	//free(zbuffer);
	//delete[] zComNum;
	Connected =true;
	if(Connected) return 1; else return -1;
}

//SEND "s 170\n" signal to the driver
int Pulsecor::startMeasurement(int targetPressure){
	if (targetPressure>100 && targetPressure<300){
		out<<"s "<<targetPressure<<"\n";
		tmp = out.str();
 		serial.Write(tmp);
		status.measurementCompleted = 0;
		out.str("");
	}
	else {
		return -1;
	}
	return 1;
}

int Pulsecor::retrieveMeasurement(int measurementNumber){
	out<<"r "<<measurementNumber<<"\n";
	tmp = out.str();
	serial.Write(tmp);
	out.str("");
	return 1;
}


void* Pulsecor::threadFunction(void *ptr){
	Pulsecor* context = (Pulsecor*)ptr;
	while(1){
		context->readMeasurement();
	}
}

int Pulsecor::readMeasurement(){
	/* new algorithm for reading, any input from device gets appended to a string, code then picks apart based on \n\r */
	// read the physical data
	serial.readThread();
	incomingData.append(serial.input);
	//tmp = serial.input;
	//extract the first bit with a carriage return following it
	int location = incomingData.find_first_of('\x0a')  + 1;
	//for some reason failure code always has a space ahead of it.
	//assume that the first character is correct, worst case scenario means that the first packet received is discarded.
	tmp = incomingData.substr(0,location);
	// wipe out incoming information
	incomingData.erase(0,location);
	location = 0;
	//cout<<"this is incoming data"<<tmp<<endl;
	int indexOfData= 0;

	if (tmp.length()>0)
	{
		//hack for fixing F's space issue
		if(tmp[0] == ' ' && tmp[1] == 'F')
		{
			tmp = tmp.substr(1, tmp.length()-1);
		}

//		cout<<"signal length is"<<strlen(tmp.c_str())<<endl;
		char hello[20];
		int number;
		sscanf(tmp.c_str(),"%s %d",hello,&number );
//		cout<<hello<<"and"<<number<<endl;
		switch (hello[0])
		{
		case 'P':
			//strValue= tmp.substr(2,3);
			status.currentPressure = number;//atoi(strValue.c_str());		
			break;

		case 'S':
//			cout<<"what is this"<<tmp.c_str()[1]<<endl;
			tmp=tmp.substr(1);
//			cout<<tmp<<endl;
			while(getNextResultString(&tmp).length() > 0)
			{
				switch(indexOfData)
				{				
				case 0:
					result.measurementID = atoi(getNextResultString(&tmp).c_str());
				break;
				case 1:
					result.systolicPres = atoi(getNextResultString(&tmp).c_str());
					break;
				case 2:
					result.meanPres = atoi(getNextResultString(&tmp).c_str());
					break;
				case 3:
					result.diastolicPres = atoi(getNextResultString(&tmp).c_str());
					break;
				case 4:
					result.pulseRate = atoi(getNextResultString(&tmp).c_str());
					break;
				case 5:
					result.augmentationIndex = atoi(getNextResultString(&tmp).c_str());
					break;
				case 6:
					result.signalNoise = atoi(getNextResultString(&tmp).c_str());
					break;
				case 7:
					result.pulseRateSS = atoi(getNextResultString(&tmp).c_str());
					break;
				case 8:
					result.reflectionTime = atoi(getNextResultString(&tmp).c_str());
					break;
				case 9:
					result.systolicEjectionPeriod = atoi(getNextResultString(&tmp).c_str());
					break;
				case 10:
					result.reflectedWaveTransitTime = atoi(getNextResultString(&tmp).c_str());
					break;
				case 11:
					result.contractility = atoi(getNextResultString(&tmp).c_str());
					break;
				case 12:
					result.suprasysOscPulsePres = atoi(getNextResultString(&tmp).c_str());
					break;
				case 13:
					result.suprasysOscPulsePresVar = atoi(getNextResultString(&tmp).c_str());
					break;
				case 14:
					result.pulseRateVar = atoi(getNextResultString(&tmp).c_str());
					break;
				case 15:
					result.supraSystolicWave = getNextResultString(&tmp).c_str();
					break;
				case 16:
					result.supraSystolicPres = getNextResultString(&tmp).c_str();
					break;
				case 17:
					result.beatStartIndex = getNextResultString(&tmp).c_str();
					break;
				case 18:
					result.featureStartIndex = getNextResultString(&tmp).c_str();
					break;
				default: 
					printf("not recognised");
				break;
			}
			indexOfData++;
//			cout<<"calculating results"<<endl;
			eraseNextResultString(&tmp);
		}
		
		// indicate that the full measurement string has been extracted
		status.measurementCompleted = 1;
		break;

		case 'F':
			status.measurementError = 1;
			//strValue= tmp.substr(2,2);	
			status.finishedMode = number;//atoi(strValue.c_str());
		break;

		case 'M':
			//strValue= tmp.substr(2,2);
			//cout<<"recieved new mode: "<<strValue<<endl;
			status.currentMode = number;//atoi(strValue.c_str());
		break;

		default:
			std::cout<<"signal not recognised: "<<tmp<<std::endl;
		break;
		}
		tmp.clear();
	}

	return 1;
}

int Pulsecor::requestDeviceLog(void){
	out<<"l "<<"\n";
	tmp = out.str();
	serial.Write(tmp);
	out.str("");
	return 1;
}

int Pulsecor::cancelMeasurement(void){
	out<<"c "<<"\n";
	tmp = out.str();
	serial.Write(tmp);
	out.str("");
	return 1;
}

int Pulsecor::setReportingDetail(int setting){
	if (setting < 4 && setting >=0){
		out<<"d "<<setting<<"\n";
		tmp = out.str();
		serial.Write(tmp);
		out.str("");
	}
	else{
		return -1;
	}
	return 1;
}

std::string Pulsecor::getNextResultString(std::string* string){
	std::string result;
	//find leading, and trailing edge, subtract to get the first immediate result
	int	tempHeadLocationForResults = tmp.find_first_of(' '); //because we dont know the length of the results
	int	tempTailLocationForResults = tmp.find_first_of(' ', tempHeadLocationForResults + 1);
	if (tempTailLocationForResults == std::string::npos){	
		return "";
	}
	result = string->substr(tempHeadLocationForResults, tempTailLocationForResults - tempHeadLocationForResults); 	
	return result;
}


int Pulsecor::eraseNextResultString(std::string* string){
	//find leading, and trailing edge, subtract to get the first immediate result
	int	tempHeadLocationForResults = tmp.find_first_of(' '); //because we dont know the length of the results
	int	tempTailLocationForResults = tmp.find_first_of(' ', tempHeadLocationForResults + 1);
	string->erase(tempHeadLocationForResults, tempTailLocationForResults - tempHeadLocationForResults);
	if ((tempTailLocationForResults - tempHeadLocationForResults)== 0)
		return -1;
	else
		return 0;
}

int Pulsecor::closeConnection(){
	return serial.close();
}
