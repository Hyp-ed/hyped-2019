#include "utils/concurrent/thread.hpp"
#include <thread>
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "./propulsion/main.hpp"
#include "./propulsion/can/can_sender_atomic.hpp"
#include <unistd.h>
#include "utils/io/can.hpp"
#include <iostream>

using hyped::System;
using hyped::utils::Logger;
using hyped::utils::io::can::Frame;
using hyped::motor_control::CanSenderAtomic;

CanSenderAtomic* can;
Logger* log_motor;

void callProcessNewData()
{
	while(true)
	{
		//log_motor->INFO("CALLPROCESSNEWDATA","Checking");
		if(can->getIsSending()) 
		{
			sleep(2);

			Frame fr;

			fr.id=2;
			log_motor->INFO("CALLPROCESSNEWDATA","Reseting");
			can->processNewData(fr);
			log_motor->INFO("CALLPROCESSNEWDATA","Reseted");
		}
	}
	
}

void producer()
{
	while(true)
	{
		//log_motor->INFO("PRODUCER","Waiting");
		sleep(1);
		log_motor->INFO("PRODUCER","PRODUCING");
		Frame fr;

		fr.id=1;

		can->pushSdoMessageToQueue(fr);
		log_motor->INFO("PRODUCER","Produced");
	}
}

int main(int argc, char* argv[]) {
	// System setup
  	hyped::utils::System::parseArgs(argc, argv);

	System& sys = System::getSystem();

	log_motor = new Logger(sys.verbose_motor, sys.debug_motor);

	can = new CanSenderAtomic(*log_motor);

	std::thread prod(producer);
	std::thread process(callProcessNewData);

	//Test Can_Sender
	
	//Thread* main = new hyped::motor_control::Main(1,log_motor);

	//main->start();

	while(true);

	return 0;
}