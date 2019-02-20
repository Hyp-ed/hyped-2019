#include "utils/concurrent/thread.hpp"
#include <thread>
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "./propulsion/main.hpp"
#include "./propulsion/concurrent_queue.hpp"
#include <unistd.h>
#include "utils/io/can.hpp"
#include <iostream>


using hyped::utils::concurrent::Thread;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::motor_control::ConcurrentQueue;
using hyped::utils::io::can::Frame;


ConcurrentQueue* queue;
Logger* log_motor;

void producer()
{
	log_motor->INFO("PRODUCER","START PRODUCING");

	//uint32_t i = 0;
	int i = 2;

	while(true)
	{
		sleep(1);
		log_motor->INFO("PRODUCER","PRODUCING");

		Frame msg;

		msg.id = i;
		i++;

		queue->push(msg);

		log_motor->INFO("PRODUCER","PRODUCED: "+msg.id);
	}
}

void consumer()
{
	log_motor->INFO("CONSUMER","START CONSUMING");

	while(true)
	{
		sleep(3);

		

		log_motor->INFO("CONSUMER","CONSUMING");

		Frame msg = queue->pop();

		std::cout << "Consumed number: " << msg.id << std::endl;
	}
}


int main(int argc, char* argv[]) {
	// System setup
  	hyped::utils::System::parseArgs(argc, argv);

	System& sys = System::getSystem();

	log_motor = new Logger(sys.verbose_motor, sys.debug_motor);

	queue = new ConcurrentQueue();

	std::thread cons(consumer);
	std::thread prod(producer);
	
	//Thread* main = new hyped::motor_control::Main(1,log_motor);

	//main->start();

	while(true);

	return 0;
}