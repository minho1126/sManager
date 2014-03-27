#include "masimo_rad8.h"

//using namespace System;
//using namespace System::IO::Ports;
//using namespace System::ComponentModel;
#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
//#include <windows.h>
//#include <stdio.h>
//#include <conio.h>
#include <cstring> 
#include <ctime>
#include <termios.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <dirent.h>
#include <sys/time.h>
#include <cmath>

using namespace std;

masimo_rad8::masimo_rad8(void)
{
}

masimo_rad8::~masimo_rad8(void)
{
}

int masimo_rad8::updateReadings()
{
	Connected = false;
	for (int x = 0; x<256; x++)
	{
		szBuffer[x] = 0;
	}
	dwRead = 0; 
	int i=0;
	while (i <256)
	{	
		//ReadFile (hComm, szBuffer, sizeof(szBuffer), &dwRead, &ovlr);
		dwRead=read(hComm,szBuffer+i,sizeof(szBuffer)-i);
		i+=dwRead;
	}
	decodeString(szBuffer);
	Connected = true;
	return 0;
}

void masimo_rad8::beginInitialiseDevice()
{
	dwRead = 0;
	//ovlr.Internal= 0;
	//ovlr.InternalHigh = 0;
	//ovlr.Offset = 0;
	//ovlr.OffsetHigh = 0;
	//ovlr.Pointer = 0;

	//ovlw.Internal = 0;
	//ovlw.InternalHigh = 0;
	//ovlw.OffsetHigh = 0;
	//ovlw.Offset = 0;
	//ovlr.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

void masimo_rad8::InitialiseDevice1()
{
	Connected  = false;
//	char* zComNum = new char[20];
//	std::string zComName = "";

	 //DWORD len = 1024;
     //LPTSTR zbuffer = (LPTSTR)malloc(len * sizeof(TCHAR));
	 ////LPTSTR name = (LPTSTR)malloc(len * sizeof(TCHAR));

//	int comPort = 3;
//	while(!Connected && comPort <= 20)
//	{
//		//if(comPort == 19)
//			//Connected =+ false;
//
//			printf("Now checking port Number:  %d \n",comPort);
//			sprintf(zComNum,"COM%d",comPort);
//
//			DWORD result = QueryDosDevice((LPCSTR)zComNum, zbuffer, len);
//			if(result == 0)
//			{ /* failed */
//				DWORD err = ::GetLastError();
//				if(err == ERROR_INSUFFICIENT_BUFFER)
//				{ /* reallocate */
//					 len *= 2;
//					 zbuffer = (LPTSTR)realloc(zbuffer, len * sizeof(TCHAR));
//					 continue;
//				} /* reallocate */
//			}
//			zComName = zbuffer;	
//			string str2 ("ProlificSerial");
//			size_t found;
//			found=zComName.find(str2);
//			if (found!=string::npos)
//			{		
//				sprintf(zComNum,"\\\\.\\COM%d",comPort);
//				hComm = CreateFile(zComNum, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);	
//				Connected = true;
//			}
//			else
//			{  
//				//free(zbuffer);
//				Connected = false;
//				comPort += 1;
//			}  
//	}

	//free(zbuffer);
//	delete[] zComNum;

	endInitialiseDevice();
	updateReadings();
}

void masimo_rad8::InitialiseDevice2()
{
	Connected  = false;
//	char* zComNum = new char[20];
//	std::string zComName = "";
//
//	 DWORD len = 1024;
//    LPTSTR zbuffer = (LPTSTR)malloc(len * sizeof(TCHAR));
//	 //LPTSTR name = (LPTSTR)malloc(len * sizeof(TCHAR));
//
//	int comPort = 20;
//	while(!Connected && comPort >= 3)
//	{
//		if(comPort == 19)
//			Connected = false;
//
//			printf("Now checking port Number:  %d \n",comPort);
//			sprintf(zComNum,"COM%d",comPort);
//
//			DWORD result = QueryDosDevice((LPCSTR)zComNum, zbuffer, len);
//			if(result == 0)
//			{ /* failed */
//				DWORD err = ::GetLastError();
//				if(err == ERROR_INSUFFICIENT_BUFFER)
//				{ /* reallocate */
//					 len *= 2;
//					 zbuffer = (LPTSTR)realloc(zbuffer, len * sizeof(TCHAR));
//					 continue;
//				} /* reallocate */
//			}
//			zComName = zbuffer;	
//			string str2 ("ProlificSerial");
//			size_t found;
//			found=zComName.find(str2);
//			 if (found!=string::npos)
//			{		
//				sprintf(zComNum,"\\\\.\\COM%d",comPort);
//				hComm = CreateFile(zComNum, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);	
//				Connected = true;
//			}
//			else
//			{  
//				//free(zbuffer);
//				Connected = false;
//				comPort -= 1;
//			}  
//	}
//
//	//free(zbuffer);
//	delete[] zComNum;

	endInitialiseDevice();
	updateReadings();

}

void masimo_rad8::endInitialiseDevice()
{
	/*dcb.DCBlength = sizeof(dcb);
	GetCommState(hComm, &dcb);

	dcb.ByteSize = 8;
	dcb.BaudRate = CBR_9600;
	//dcb settings

	for (int x = 0; x<256; x++){
		szBuffer[x] = 0;
	}

	cto.ReadIntervalTimeout = 50;
	cto.ReadTotalTimeoutConstant = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 0;

	SetCommState(hComm, &dcb);
	SetCommTimeouts(hComm, &cto);*/
	for (int x = 0; x<256; x++){
		szBuffer[x] = 0;
	}
	struct termios oldtio, newtio;
	BAUD = B9600;
	DATABIT=CS8;
	STOPBIT=CSTOPB;
	PARITYON=PARENB;
	PARITY=PARODD;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	string devicename=findtty();
	if(devicename.compare("")==0){
		return;
	}else{
		devicename="/dev/"+devicename;
	}
	//open up the connection check if available, then save the old status and replace with new status
	hComm=open(devicename.c_str(), O_RDWR|O_NOCTTY);
	if(hComm<0){perror(devicename.c_str());cout<<"connection doesnt exist"<<endl;exit(-1);}
	tcgetattr(hComm,&oldtio);
	bzero(&newtio,sizeof(newtio));
	newtio.c_cflag=BAUD | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL |INLCR;
	newtio.c_cc[VINTR] =0;
	newtio.c_cc[VQUIT] =0;
	newtio.c_cc[VERASE] =0;
	newtio.c_cc[VKILL] =0;
	newtio.c_cc[VEOF] =4;
	newtio.c_cc[VTIME] =0;
	newtio.c_cc[VMIN] =1;
	newtio.c_cc[VSWTC] =0;
	newtio.c_cc[VSTART] =0;
	newtio.c_cc[VSTART] =0;
	newtio.c_cc[VSTOP] =0;
	newtio.c_cc[VSUSP] =0;
	newtio.c_cc[VEOL] =0;
	newtio.c_cc[VREPRINT] =0;
	newtio.c_cc[VDISCARD] =0;
	newtio.c_cc[VWERASE] =0;
	newtio.c_cc[VLNEXT] =0;
	newtio.c_cc[VEOL2] =0;
	Connected  = false;
	//import settings
	tcflush(hComm,TCIFLUSH);
	cfsetospeed(&newtio, B9600);
	tcsetattr(hComm,TCSANOW,&newtio);
	Connected=true;	
}

void masimo_rad8::decodeString(std::string serialInput){ 
	alarm = 0;
	int check;
	int tempPosition;
	std::string temp;
	std::stringstream ss(temp);
	tempPosition = serialInput.find("SPO2", 0); //look for spo2 string
	temp = serialInput.substr(tempPosition+5, 3);
	check = temp.find("-");
	
	if (check!=std::string::npos)
	{
		SPO2= 0;
	}
	else
	{
		ss<<temp;
		ss>>SPO2;
	}

	//Look for BPM
	tempPosition = serialInput.find("BPM", 0); //look for bpm string
	temp = serialInput.substr(tempPosition+4, 3);
	ss.clear(); //clear error flags
	check = temp.find("-");
	
	if (check!=std::string::npos)
	{
		bpm = 0;
	}
	else
	{
		ss<<temp;
		ss>>bpm;
	}
	
	//Look for PI
	tempPosition = serialInput.find("PI", 0); //look for pi string
	temp = serialInput.substr(tempPosition+3, 5);
	ss.clear(); //clear error flags
	check = temp.find("-");
	
	if (check!=std::string::npos)
	{
		pi = 0;
	}
	else
	{
		ss<<temp;
		ss>>pi;
	}


	//Look for SPCO, currently not active on device
	tempPosition = serialInput.find("SPCO", 0); //look for spco string
	temp = serialInput.substr(tempPosition+5, 4);
	ss.clear(); //clear error flags

	check = temp.find("-");
	
	if (check!=std::string::npos)
	{
		spco = 0;
	}
	else
	{
		ss<<temp;
		ss>>spco;
	}

	//Look for SPMET, currently not active/used
	tempPosition = serialInput.find("SPMET", 0); //look for spmet string
	temp = serialInput.substr(tempPosition+6, 4);
	ss.clear(); //clear error flags
	check = temp.find("-");
	
	if (check!=std::string::npos)
	{
		spmet = 0;
	}
	else
	{
		ss<<temp;
		ss>>spmet;
	}
	
	ss.clear(); //clear error flags

	//Look for DESAT, currently not active/used
	tempPosition = serialInput.find("DESAT", 0); //look for desat string
	temp = serialInput.substr(tempPosition+6, 2);	
	ss.clear(); //clear error flags
	
	check = temp.find("-");
	
	if (check!=std::string::npos)
	{
		desat= 0;
	}
	else
	{
		ss<<temp;
		ss>>desat;	
	}


	//Look for PIDELTA, currently not active/used
	tempPosition = serialInput.find("PIDELTA", 0); //look for pidelta string
	temp = serialInput.substr(tempPosition+9, 2);
	ss.clear(); //clear error flags
	

	if (check!=std::string::npos)
	{		
		pidelta = 0;
	}
	else
	{
		ss<<temp;
		ss>>pidelta;
	}

	tempPosition = serialInput.find("ALARM", 0); //look for alarm string
	temp = serialInput.substr(tempPosition+6, 4);
	ss.clear(); //clear error flags

	if (check!=std::string::npos)
	{
		alarm = 0;
	}
	else{
		ss<<temp;
		ss>>alarm;
	}
	/*Look for EXC String. This tells us what mode the device is currently in
	 * e.g. is it trying to get a value, or is the sensor connected...
	 */
	tempPosition = serialInput.find("EXC", 0); //look for exc_mode string
	temp = serialInput.substr(tempPosition+4, 6);
	ss.clear(); //clear error flags
} 

string masimo_rad8::findtty(){
	string name="ttyUSB";
	DIR* dirp=opendir("/dev");
	while(dirp){
		int errno=0;
		dirent* dp=readdir(dirp);
		if(dp!=NULL){
			cout<<dp->d_name<<endl;
			string fileName(dp->d_name);
			if(fileName.find(name)==0 && access(("/dev/"+fileName).c_str(),F_OK)!=-1){
				//cout<<"found"<<endl;
				closedir(dirp);
				return fileName;
			}
		}else{
			if(errno==0){
				closedir(dirp);
				return "";
			}
			closedir(dirp);
			return "";
		}
	}
}

void masimo_rad8::closeConnection(){
	::close(hComm);
}
