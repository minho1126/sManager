#include <iostream>
#include <cstdio>
#include "ros/ros.h"
#include <cstdlib>
#include "BPDrivers.hpp"
#include "SPDriver.hpp"
#include "MasimoDriver.hpp" 	
#include "std_msgs/String.h"
#include "smanager/RegisterMessage.h"
#include "smanager/Checker.h"
#include "smanager/ServiceRequest.h"
#include "smanager/data.h"
#include <csignal>
using namespace std;

unsigned int generateID(){
	srand(time(NULL));
	unsigned int a=rand();
	unsigned int b=rand();
	while(a== 0)a=rand()%1000;//if random is 0 loop
	while(b== 0)b=rand()%1000;//if random is 0 loop
	ROS_INFO("id is %d",a*b);
	return a*b;
}


string portScanner(){
	string bbb;
	char buff[1000];
	FILE *fp = popen("lsusb|grep ATEN","r");
	if ( fgets( buff, BUFSIZ, fp ) != NULL ) {
		//printf("%s", buff );
		//string aaa(buff);
		bbb="BP";
		fclose(fp);
		//cout<<"BP"<<endl;
		return bbb;
	}
	fclose(fp);
	//usleep(500000);
	fp = popen("lsusb|grep CP210x","r");
	if ( fgets( buff, BUFSIZ, fp ) != NULL ) {
		//printf("%s", buff );
		//string aaa(buff);
		bbb="SPO2";
		//cout<<strlen(buff)<<endl;
		//cout<<"SPO2"<<endl;
		fclose(fp);
		return bbb;
	}
	fclose(fp);
	//usleep(500000);
	fp = popen("lsusb|grep Prolific","r");
	if ( fgets( buff, BUFSIZ, fp ) != NULL ) {
		//printf("%s", buff );
		//string aaa(buff);
		bbb="Masimo";
		fclose(fp);
		//cout<<"Masimo"<<endl;
		return bbb;
	}
	if(bbb.length()==0){
		cout<<"nothing"<<endl;
		fclose(fp);
		return "";
	}
}


void threadCloser(int signum){
	cout<<"closing the thread"<<endl;
	pthread_exit(NULL);
	exit(0);
}

void *fake_call(void* conman){
	ROS_INFO("Starting fake call");
	signal(SIGINT,threadCloser);
	ConnectionManager* cm=(ConnectionManager*)conman;
	cm->StartService();
	pthread_exit(NULL);
}


int main(int argc,char **argv){
	ConnectionManager* conman;
	//enter main while loop
	stringstream ss;
	ss<<"devicedriver_"<<generateID();
	string sensor=portScanner();
	cout<<sensor<<endl;
	ros::init(argc,argv,ss.str());
	ros::NodeHandle n,m;
	string bbb;
	ros::ServiceClient client;
	ros::ServiceServer services;
	string currentSensor;
	ros::Rate loop_rate(10);
	while(true){
		if(sensor.compare("BP")==0){
			cout<<"BP"<<endl;
			//initialise BP sensor here
			conman=new BPDriver();
			client=n.serviceClient<smanager::RegisterMessage>("driver_request");
			if(!conman->Register(client)){
				exit(0);
			}
			string requestName=conman->getRequestname();
			ROS_INFO("client registering BP_request:%s", requestName.c_str());
			services=n.advertiseService(requestName,&BPDriver::InitialiseService,(BPDriver*)conman);
			
			
			
			
		}else if(sensor.compare("SPO2")==0){
			cout<<"SPO2"<<endl;
			//init SPO2 sensor here
			conman=new SPDriver();
			client=n.serviceClient<smanager::RegisterMessage>("driver_request");
			if(!conman->Register(client)){
				exit(0);
			}
			string requestName=conman->getRequestname();
			ROS_INFO("client registering SPO2_request:%s", requestName.c_str());
			services=n.advertiseService(requestName,&SPDriver::InitialiseService,(SPDriver*)conman);
			
			
			
		}else if(sensor.compare("Masimo")==0){
			cout<<"Masimo"<<endl;
			//init Masimo sensor here
			conman= new MasimoDriver();
			client=n.serviceClient<smanager::RegisterMessage>("driver_request");
			if(!conman->Register(client)){
				exit(0);
			}
			string requestName=conman->getRequestname();
			ROS_INFO("client registering Masimo_request:%s", requestName.c_str());
			services=n.advertiseService(requestName,&MasimoDriver::InitialiseService,(MasimoDriver*)conman);
			
			
			
		}else{
			cout<<"nothing"<<endl;
			sleep(1);
			sensor=portScanner();
			continue;
		}
		//start function here
		while(conman!=NULL){
			if(conman->is_Started()){
				ROS_INFO("ONLY ONCE");
				pthread_t threads[1];
				pthread_create(&threads[0], NULL, fake_call,(void*)conman);
			}
			currentSensor=portScanner();
			//if current sensor is not same as one registered then break;
			if(currentSensor.compare(sensor)!=0){
				ROS_INFO("shutdown");
				delete conman;
				services.shutdown();
				conman=NULL;
				sensor="";
				break;
			}
			//smanager::data msg;
			//msg.data="aaa";
			ros::spinOnce();
			//sleep(1);
			//ROS_INFO("sleeping");
		}
		sleep(1);
	}
}
