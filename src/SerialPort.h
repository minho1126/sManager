#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <termios.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <dirent.h>


#define EXAMPLE_PORT "/dev/ttyUSB0"

using namespace std;

class SerialPort
{
public:
	SerialPort(void);
	bool initialiseSerialPort(int baudRate);
	int	openComPort(int comPort);
	int readThread();
	int close();
	int Write(std::string input);
	std::string inputString;
	std::string outputString;
	//HANDLE hComm;
	int hComm;//file descriptor
	//DCB dcb;//= {0};
	struct termios oldtio, newtio;
	char input[10000]; //buffer for debug 
	string findtty();
	unsigned long dwRead, dwWritten;
	unsigned long lpEvtMask;
	//COMMTIMEOUTS cto;
	//COMMPROP prop;

public:
	~SerialPort(void);
};
