#include "utils/concurrent/thread.hpp"
#include <thread>
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "./propulsion/main.hpp"
#include "./propulsion/concurrent_queue.hpp"
#include <unistd.h>
#include "utils/io/can.hpp"
#include <iostream>
#include <atomic>

using hyped::utils::concurrent::Thread;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::motor_control::ConcurrentQueue;
using hyped::utils::io::can::Frame;


ConcurrentQueue* queue;
Logger* log_motor;

std::atomic<bool> ready;

void sender()
{
	log_motor->INFO("PRODUCER","START PRODUCING");

	//uint32_t i = 0;
	int i = 2;

	while(true)
	{
		sleep(1);
		log_motor->INFO("PRODUCER","PRODUCING");

		while(ready==false)
		{
			log_motor->INFO("PRODUCER","WAITING");
		}

		log_motor->INFO("PRODUCER","SENDING");

		ready=false;

		/*Frame msg;

		msg.id = i;
		i++;

		queue->push(msg);*/

		//log_motor->INFO("PRODUCER","PRODUCED: "+msg.id);
	}
}

void receiver()
{
	log_motor->INFO("CONSUMER","START CONSUMING");

	

	log_motor->INFO("CONSUMER","CONSUMED");

	while(true)
	{
		sleep(1);

		//while(ready==false);

		log_motor->INFO("CONSUMER","RECEIVED");

		ready=true;

		//Frame msg = queue->pop();

		//std::cout << "Consumed number: " << msg.id << std::endl;
	}
}


int main(int argc, char* argv[]) {
	// System setup
  	hyped::utils::System::parseArgs(argc, argv);

	System& sys = System::getSystem();

	log_motor = new Logger(sys.verbose_motor, sys.debug_motor);

	queue = new ConcurrentQueue();

	std::thread cons(sender);

	for(int i = 0;i<1000000;i++);

	std::thread prod(receiver);
	
	//Thread* main = new hyped::motor_control::Main(1,log_motor);

	//main->start();

	while(true);

	return 0;
}