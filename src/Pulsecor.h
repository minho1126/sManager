#include "SerialPort.h"
#include <string>
#include <sstream>
#include "pthread.h"
#include <vector>

class bpResults{
	public:
		bpResults(void);
		~bpResults(void);
		int measurementID;
		int systolicPres;
		int meanPres;
		int diastolicPres;
		int pulseRate;
		int augmentationIndex;
		int signalNoise;
		int pulseRateSS;
		int reflectionTime;
		int systolicEjectionPeriod;
		int reflectedWaveTransitTime;
		int contractility;
		int suprasysOscPulsePres;
		int suprasysOscPulsePresVar;
		int pulseRateVar;
		std::string supraSystolicWave;
		std::string supraSystolicPres;
		std::string beatStartIndex;
		std::string featureStartIndex;
};


class currentStatus{
	public:
		currentStatus(void);
		~currentStatus(void);
		int finishedMode;
		int measurementCompleted;
		int measurementError;
		int currentMode;
		int currentPressure;
		std::string errorDetails;
		std::string logDetails;
};

class Pulsecor
{
public:
	//int Pulsecor::eraseNextResultString(std::string* string);
	int eraseNextResultString(std::string* string);
	//std::string Pulsecor::getNextResultString(std::string* string);
	std::string getNextResultString(std::string* string);
	std::string strValue;
	Pulsecor(void);
	SerialPort serial;
	bpResults result;
	currentStatus status;
	int connectionStatus;
	char* Mid(char *Sentence, int FromNumber, int ToNumber);
	int Instr(char *SearchString, char *SearchTerm);
	int connectToDevice(int dir);
	int startMeasurement(int targetPressure);
	int retrieveMeasurement(int measurementNumber);
	int requestDeviceLog(void);
	int cancelMeasurement(void);
	int setReportingDetail(int setting);
	int readMeasurement(void);
	int closeConnection();
private:
	static void* threadFunction(void *ptr);
	std::stringstream out;
	std::string incomingData;
	std::string tmp;
	pthread_t read_thread;
	pthread_mutex_t serialMutex;
	std::vector<std::string> friendlyNames;
	std::vector<unsigned int> ports;
public:
	~Pulsecor(void);



};
