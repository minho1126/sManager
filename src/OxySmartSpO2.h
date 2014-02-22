#pragma once

#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
//#include <windows.h>
#include <stdio.h>
#include <ctime>

#define READ_BUF_SIZE 5



class OxySmartSpO2{
public:
	void initialiseDevice();
	//int initialiseDeviceCOM();
	int updateReadings();
	void parseReading();
	int getInt(int byte, int offset, int len);
	bool getBool(int byte, int offset);
	std::string findtty();
	// device status
	bool gotReading;
	bool Connected;
	// readings
	int signalStrength; // 0 - 8
	bool searchingToLong;
	bool SPO2Decrease;
	bool pulseBeepSignal;
	int plethysmogramWave; // 0-100
	int realTimeBarGraph; //
	bool probeError;
	bool searchingForPulse;
	int pulseRate; // 30-254, 255=invalid pulse
	float spo2Percent; // percentage

	

	char readBuf[READ_BUF_SIZE];

	//DCB dcb ;
	int hComm;
	//COMMTIMEOUTS cto;

	unsigned long dwRead, dwWritten;
	////DWORD lpEvtMask;
	////HANDLE hComm;
	//OVERLAPPED ovlr, ovlw;
	long BAUD;
	long DATABIT;
	long STOPBIT;
	long PARITYON;
	long PARITY;

};
