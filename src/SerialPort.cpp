#include "SerialPort.h"

SerialPort::SerialPort(void)
{
}

SerialPort::~SerialPort(void)
{
}


void SerialPort::initialiseSerialPort(int baudRate){

	dwRead = 0;

	
	//dcb.ByteSize = 8;
	//dcb.BaudRate = baudRate;9600
	//dcb.DCBlength = sizeof(dcb);	
	//cto.ReadIntervalTimeout = 50;
	//cto.ReadTotalTimeoutConstant = 100;
	//cto.ReadTotalTimeoutMultiplier = 0;
	//cto.WriteTotalTimeoutConstant = 100;
	//cto.WriteTotalTimeoutMultiplier = 0;
    hComm=open(EXAMPLE_PORT, O_RDWR|O_NOCTTY);
    cout<<endl<<EXAMPLE_PORT<<" "<<hComm<<endl;
    if(hComm<0){
		perror(EXAMPLE_PORT);
		cout<<"connection doesnt exist"<<endl;
		exit(-1);
	}
	
	tcgetattr(hComm,&oldtio);
	bzero(&newtio,sizeof(newtio));
	
	long BAUD = B9600;
	long DATABIT=CS8;
	//long STOPBIT=CSTOPB;
	long PARITYON=0;//PARENB; no parity
	long PARITY=0;//PARODD;
	
	struct timeval timeout;
	timeout.tv_sec = 50;
	timeout.tv_usec = 0;
	
	newtio.c_cflag=BAUD | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL |INLCR;
	//newtio.c_oflag = OPOST|OCRNL;
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
	tcflush(hComm,TCIFLUSH);
	cfsetospeed(&newtio, B9600);
	tcsetattr(hComm,TCSANOW,&newtio);
}


int	SerialPort::openComPort(int comPort)
{
	/*
	char tempComPort[100];
	sprintf(tempComPort, "\\\\.\\COM%d", comPort);
	hComm = CreateFile(tempComPort, GENERIC_READ|GENERIC_WRITE,0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetCommState(hComm, &dcb);
	SetCommTimeouts(hComm, &cto);
	if (hComm == INVALID_HANDLE_VALUE) 
    {
        // Handle the error. 
        printf("CreateFile failed with error %d.\n", GetLastError());
		CloseHandle(hComm);
        return -1;
    }
	else if (hComm == (HANDLE)ERROR_ACCESS_DENIED)
	{
		std::cout << " access denied " << std::endl;
		CloseHandle(hComm);
	}*/
	cout<<"done"<<endl;
	return 1; //if successful return 1
}

int SerialPort::readThread(){
	//ReadFile(hComm, input, 1, &dwRead, NULL);
	dwRead=read(hComm,input,1);
	input[dwRead] = NULL; //set last byte to null. allows code to recognise it as a string
	return 1; //return 1 when a string has been read
}


int SerialPort::Write(std::string output){
	//printf("attempting to write file\n");
	//DWORD byte;
	unsigned long byte;
	//WriteFile(hComm, output.c_str(), output.length(), &dwWritten, NULL);
	dwWritten = write(hComm,output.c_str(),output.length());
//	cout<<"bytes written: "<<dwWritten<<" "<<strlen(output.c_str())<<endl;
	//WriteFile(hComm, "hi", 2, &byte, NULL);
//	printf("file written %s \n",output.c_str());
	return 1; //return 1 when a string has been written
}

int SerialPort::close(){
	//CloseHandle(hComm);
	::close(hComm);
	return 1;
}
