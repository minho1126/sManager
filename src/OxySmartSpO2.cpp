#include "OxySmartSpO2.h"
#include <sys/time.h>
#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
//#include <windows.h>
#include <cstdio>
//#include <conio.h>
#include <ctime>
#include <cmath>
#include <strings.h>
#include <vector>
//#include "enumser.h"
#include <termios.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <dirent.h>

#define EXAMPLE_PORT "/dev/ttyUSB0"

using namespace std;

int OxySmartSpO2::updateReadings()
{
	// Fill readBuf[] with fresh data
	// Then run parseReadings()

	Connected = false;
	for (int x = 0; x < READ_BUF_SIZE; x++)
	{
		readBuf[x] = 0;
	}
	dwRead = 0;

	// Read while we haven't filled readBuf[]
	while(dwRead < READ_BUF_SIZE)
	{
		int emptyReadCount = 0; // keep track of how many empty reads 
		//DWORD thisRead = 0;
		unsigned int thisRead = 0;
		//ReadFile (hComm, readBuf + dwRead, READ_BUF_SIZE - dwRead, &thisRead, &ovlr);
		thisRead=read(hComm,readBuf+dwRead,READ_BUF_SIZE-dwRead);
		//cout << dwRead << endl;
		if (thisRead == 0)
		{
			emptyReadCount++;
			if (emptyReadCount > 5 ) // give up after 5 empty reads 
			{
				cout << "giving up after 5" << endl;
				return -1;
			}
			continue;
		}
		dwRead += thisRead;

		//sync reading first byte must have a 1 in MSB, otherwise we are not in SYNC
		// condition not needed but stops loop running 
		if((readBuf[0] & 0x80) == 0)
		{
			//check if sync bit is present
			int foundPos = -1;
			for(int i = 1; i < READ_BUF_SIZE; i++)
			{
				if((readBuf[i] & 0x80) != 0)
				{
					foundPos = i;
				}
				// move backwards so already read bytes are in correct place
				if(foundPos != -1)
				{
					readBuf[i - foundPos] = readBuf[i];
				}
			}
			// rewind dwRead so the extra bytes are read
			if(foundPos != -1){
				dwRead -= foundPos;
			}
		}
		//cout<<"readbuff1:"<<readBuf[0]<<endl;
		//cout<<"readbuff2:"<<readBuf[1]<<endl;
		//cout<<"readbuff3:"<<readBuf[2]<<endl;
		//cout<<"readbuff4:"<<readBuf[3]<<endl;
		//cout<<"readbuff5:"<<readBuf[4]<<endl;

	}

	if ( dwRead > 0 )
		gotReading = true;

	parseReading();


	Connected = true;
	return 0;
}

void OxySmartSpO2::parseReading()
{
	/*
	The data is transferred 60 packs every second the 7th bit is SYNC bits.
	Byte Bit Meaning

	Byte 1:
	0~3	Signal Strength in the range 0…8 
	4 	1=searching too long time 0=OK 
	5	1 SPO2 decrease 0=OK 
	6	1=Pulse beep singnal
	7	SYNC bit =1 

	Byte 2:
	0~6	Real time plethysmogram waveform in the range 0…100 
	7	SYNC bit =0 

	Byte 3:
	0~3	Real time Bar Graph (indicates the pulse beep) 
	4	1=probe error 0=OK 
	5	1=searching for pulse 0=OK 
	6	Pulse rate most significant bit( bit 7) 
	7	SYNC =0 

	Byte 4:
	0~6	Bits 0…6 of Pulse Rate(note: bit 7 is located in byte 3). Pulse range is 30…254bpm, 255-invalid Pulse Rate. 
	7	SYNC =0

	Byte 5:
	0~6	SPO2%
	7	SYNC =0
	*/

	// Byte 1
	signalStrength = getInt(0, 0, 4);
	searchingToLong = getBool(0, 4);
	SPO2Decrease = getBool(0, 5);
	pulseBeepSignal =getBool(0, 6);

	// Byte 2
	plethysmogramWave = getInt(1, 0, 7);

	// Byte 3
	realTimeBarGraph = getInt(2, 0, 4);
	probeError = getBool(2, 4);
	searchingForPulse = getBool(2, 5);

	// Byte 4
	pulseRate = getInt(3, 0, 7);
	// MSB is located in byte 3 so we need to add it manually
	if(getBool(2, 7)){
		pulseRate += 1 << 7;
	}

	// Byte 5
	spo2Percent = getInt(4, 0, 7);

	/*
	cout << pulseRate << "," << spo2Percent << "," << plethysmogramWave << " ";
	if(searchingForPulse)
		cout << "searchingForPulse ";
	if(searchingToLong)
		cout << "searchingToLong ";
	if(probeError)
		cout << "probeError ";
	if(pulseBeepSignal)
		cout << "pulseBeepSignal ";
	if(SPO2Decrease)
		cout << "SPO2Decrease ";
	//cout << plethysmogramWave << endl;
	cout << endl; //*/


}

bool OxySmartSpO2::getBool(int byte, int offset)
{
	return (readBuf[byte] & (int) pow(2.0, offset)) != 0;
}

int OxySmartSpO2::getInt(int byte, int offset, int len)
{
	char c = readBuf[byte];
	c >> offset;
	return c & ((int) pow(2.0, len) - 1);
}

void OxySmartSpO2::initialiseDevice()
{

	/*dcb.DCBlength = sizeof(dcb);
	GetCommState(hComm, &dcb);

	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = ODDPARITY;
	dcb.BaudRate = CBR_4800;
	//dcb settings*/
    //the serial connection settings
    struct termios oldtio, newtio;
	BAUD = B4800;
	DATABIT=CS8;
	STOPBIT=CSTOPB;
	PARITYON=PARENB;
	PARITY=PARODD;
	//string devicename="/dev/ttyO2";
	for (int x = 0; x<READ_BUF_SIZE; x++){
		readBuf[x] = 0;
	}
    /*set timeout
	cto.ReadIntervalTimeout = 50;
	cto.ReadTotalTimeoutConstant = 10;
	cto.ReadTotalTimeoutMultiplier = 1;
	cto.WriteTotalTimeoutConstant = 10;
	cto.WriteTotalTimeoutMultiplier = 1;

	// Setup COM in one function
	//initialiseDeviceCOM();
	//}
	//int OxySmartSpO2::initialiseDeviceCOM()
	//{
	*/
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	/*
	// find COM port?
	std::vector<std::string> friendlyNames;
	std::vector<UINT> ports;
	if (CEnumerateSerial::UsingSetupAPI1(ports, friendlyNames))
	{
		for (size_t i=0; i<ports.size(); i++)
			_tprintf(_T("COM%d <%s>\n"), ports[i], friendlyNames[i].c_str());
	}
	*/
	
	
	string devicename=findtty();
	if(devicename.compare("")==0){
		return;
	}else{
		devicename="/dev/"+devicename;
	}
	//open up the connection check if available, then save the old status and replace with new status
	hComm=open(devicename.c_str(), O_RDWR|O_NOCTTY);
	
	if(hComm<0){perror(EXAMPLE_PORT);cout<<"connection doesnt exist"<<endl;exit(-1);}
	tcgetattr(hComm,&oldtio);
	bzero(&newtio,sizeof(newtio));
	newtio.c_cflag=BAUD | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
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
	gotReading = false;
	Connected  = false;
	char* zComNum = new char[20];
	std::string zComName = "";
	
	//import settings
	tcflush(hComm,TCIFLUSH);
	tcsetattr(hComm,TCSANOW,&newtio);
	
	//sizeof(char));

	/*
	int comPort = 0;
	for ( size_t i = 0; i < ports.size(); i++) 
	{
		comPort = ports[i];

		printf("Now checking port Number:  %d \n",comPort);

		string str2 ("Silicon Labs CP210x");
		size_t found;
		cout << "FRIENDLY NAME: " << friendlyNames[i] << endl;
		found=friendlyNames[i].find(str2);
		if (found!=string::npos)
		{		
			sprintf(zComNum,"\\\\.\\COM%d",comPort);
			std::cout << "zComNum" << zComNum << "\n";
			hComm = CreateFile(zComNum, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
			std::cout << " handle no. " << hComm << std::endl;

			if (hComm == (HANDLE)INVALID_HANDLE_VALUE)
			{
				std::cout << " invalid value " << std::endl;
				CloseHandle(hComm);
			} 
			else if (hComm == (HANDLE)ERROR_ACCESS_DENIED)
			{
				std::cout << " access denied " << std::endl;
				//close connection
				CloseHandle(hComm);
			} 
			else {				
				SetCommState(hComm, &dcb);					
				if (!SetCommTimeouts(hComm, &cto))
					std::cout << "unable to set time outs" << std::endl;

				for ( int i = 0; i < 5 ; i++) {				
					if (updateReadings() == -1 )
						continue;
					if (gotReading){
						std::cout << "connected to port " << comPort << std::endl;
						Connected = true;
						break;
					}
				}
				if (!gotReading){
					//close connection
					CloseHandle(hComm);
				}

			}
		}
		else
		{  
			Connected = false;
		}  
		if (Connected)
			break;
	}

	delete[] zComNum;

	if ( Connected ){
		updateReadings();
	}
	*/
	updateReadings();
	Connected=true;	
}


string OxySmartSpO2::findtty(){
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

void OxySmartSpO2::closeConnection(){
	::close(hComm);
}
