#pragma once

//#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
//#include <windows.h>
#include <stdio.h>
#include <ctime>



class masimo_rad8
{
public:
	masimo_rad8(void);

	bool Connected;

	char szBuffer[256];
	//functions
	void getReadings();
	int SetupDevice();
	int ResetDevice();
	void closeConnection();
	//LPTSTR ConnectToMasimo();	

	void beginInitialiseDevice();	
	void InitialiseDevice1();
	void InitialiseDevice2();
	void endInitialiseDevice();
	std::string findtty();
	
	void decodeString(std::string serialInput);
	int updateReadings();
	//variables	
	time_t t;
	int serial_no;
	float SPO2; //represented in percentage, integer only
	 int bpm;
	float pi;
	float spco; //not used
	float spmet; //not used
	unsigned int desat; //not used
	int pidelta; //signed //not used
	int alarm;
	int exc_mode;

	//DCB dcb ;//= {0};
	unsigned long dwRead, dwWritten;
	//DWORD lpEvtMask;
	//HANDLE hComm;
	//OVERLAPPED ovlr, ovlw;
	//COMMTIMEOUTS cto;
	//COMMPROP prop;
	long BAUD;
	long DATABIT;
	long STOPBIT;
	long PARITYON;
	long PARITY;
	int hComm;

public:
	~masimo_rad8(void);
};
