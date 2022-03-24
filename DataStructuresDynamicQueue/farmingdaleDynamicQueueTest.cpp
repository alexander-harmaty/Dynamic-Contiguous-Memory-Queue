// (c) 2013-2020 David Gerstl, all rights reserved
// For the use of my C++ students to use as a base to implement
// dynamic arrays, exceptions and operator overloading, and templates


// Class queue: General integer queue implementation. We will add dynamic arrays (for
// unlimited size), templates (to allow multiple types), operator overloading
// (for queue comparisons). 


// Please see the accompanying Powerpoint for some explanation of the 
// design and algorithms. If you come to office hours for explanations, 
// I will expect that you have read them

// Note: I've ommitted test code to check the proper operation of some methods--don't doubt that they exist

#include <iostream>
#include <string>
#include "farmingdaleDynamicContiguousMemoryQueue.h"

// using  farmingdale::statusCode::FAILURE;
// using  farmingdale::statusCode::SUCCESS;
// using  farmingdale::queue;



#ifndef TEMPLATED_QUEUE



void printTestMessages(std::ostream& theStream) {
	theStream << "This is the non-templated version of the queue tests: Make sure you do the template conversion and pass both tests" << std::endl;
	theStream << "You can switch these on line " << TEMPLATED_HEADER_LINE << " of " << TEMPLATED_HEADER_FILE << std::endl;
}

int normalTest1() {
	// some of my setup for tracking memory usage. 
	// See https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
	_CrtMemState sB4, sMiddle, sEnd;
	// Set a memory Checkpoint
	_CrtMemCheckpoint(&sB4);
	// Note that this code is put into its own scope so the dtor gets called BEFORE the end of the method
	// so we can test if the dtor is leaking memory
	{
		farmingdale::queue myq;
		const int some_large_number = 100000;
		for (int i = 0; i < some_large_number; ++i) {
			if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i + 1000))) {
				std::cerr << "Error on line " << __LINE__ << std::endl;
				return __LINE__;
			}
		}

		// Set a memory Checkpoint
		_CrtMemCheckpoint(&sMiddle);

		// push should never fail
		std::string j;
		std::string t;
		for (int i = 0; i < some_large_number; ++i) {
			if (farmingdale::statusCode::FAILURE == myq.peek(t) ||
				farmingdale::statusCode::FAILURE == myq.dequeue(j) ||
				t != j ||
				j != std::to_string(1000 + i)) {
				std::cerr << "Error on line " << __LINE__ << " j is " << j << " and i is " << i << std::endl;
				return __LINE__;
			}
		}
	} // extra scope to call dtor for stack
	  // Set a memory Checkpoint
	_CrtMemCheckpoint(&sEnd);

	// Now validate that you successfully cleaned up the memory in your destructor
	if ((sMiddle.lCounts[_NORMAL_BLOCK] - sB4.lCounts[_NORMAL_BLOCK]) < 1) {
		std::cerr << "It doesn't seem like you allocated any memory" << std::endl;
		return __LINE__;
	}
	if ((sEnd.lCounts[_NORMAL_BLOCK] - sB4.lCounts[_NORMAL_BLOCK]) != 0) {
		std::cerr << "It seems you are leaking memory in your descructor" << std::endl;
		return __LINE__;
	}

	return 0;
}

int normalTest2() {
	// a more curated version of the test. Comments inline to tell you what I'm doing.
	farmingdale::queue myq;
	for (int i = 0; i < myq.initialQueueCapacity(); ++i) {
		if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i + 1000))) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}
	std::string j;
	std::string t;
	// at this point the queue is full of (1000),(1001)...(1015) [assuming capacity is 15] 
	// Now, empty the first 5 elements. 
	for (int i = 0; i < 5; ++i) {
		if (farmingdale::statusCode::FAILURE == myq.peek(t) ||
			farmingdale::statusCode::FAILURE == myq.dequeue(j) ||
			t != j ||
			j != std::to_string(1000 + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and t is " << t << std::endl;
			return __LINE__;
		}
	}
	// Now put 4 more in
	std::string nextString;
	for (int i = 0; i < 4; ++i) {
		nextString = std::to_string(i + 1000 + myq.initialQueueCapacity());
		if (farmingdale::statusCode::FAILURE == myq.enqueue(nextString)) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}
	// so now the queue is (1005), (1006), ... , (1015), (1016), (1017), (1018), (1019)
	// but in memory this is 
	// (1017), (1018), (1019), ....(1005), ..., (1015), (1016)
	// Now, we're going to go do something different. Let's start with (1020) and add some
	// large number of items, causing a reallocation
	const int some_large_number = 100000;
	// Note that j was the last number we saw above. Convert to string and add one
	int expected_next_value = std::stoi(j) + 1;
	// note that nextString was the last string we inserted. Add one to that to start
	int startingInt = std::stoi(nextString) + 1;
	for (int i = startingInt; i < some_large_number + startingInt; ++i) {
		if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i))) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}
	std::string e;
	// make sure they come out correctly.... (we have one less than when we started (-5+4)
	for (int i = 0; i < some_large_number; ++i) {
		// shift the expected value up by 5....
		if (farmingdale::statusCode::FAILURE == myq.peek(t) ||
			farmingdale::statusCode::FAILURE == myq.dequeue(j) ||
			t != j ||
			j != std::to_string(expected_next_value + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and t is " << t <<
				" and i is " << i << " and I expected " << expected_next_value + i << std::endl;
			return __LINE__;
		}
	}
	return 0;
}

int copyCtorTest() {
	farmingdale::queue myq;
	const int some_large_number = 100000;
	for (int i = 0; i < some_large_number; ++i) {
		if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i + 1000))) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}

	farmingdale::queue myOtherQ(myq);
	farmingdale::queue aThirdQueue = myq;
	if (myOtherQ != myq) {
		std::cerr << "Error on line " << __LINE__ << std::endl;
		return __LINE__;
	}
	if (aThirdQueue != myq) {
		std::cerr << "Error on line " << __LINE__ << std::endl;
		return __LINE__;
	}
	std::string j;
	std::string t;
	for (int i = 0; i < some_large_number; ++i) {
		if (farmingdale::statusCode::FAILURE == myOtherQ.peek(t) ||
			farmingdale::statusCode::FAILURE == myOtherQ.dequeue(j) ||
			t != j ||
			j != std::to_string(1000 + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and i is " << i << std::endl;
			return __LINE__;
		}
	}
	for (int i = 0; i < some_large_number; ++i) {
		if (farmingdale::statusCode::FAILURE == aThirdQueue.peek(t) ||
			farmingdale::statusCode::FAILURE == aThirdQueue.dequeue(j) ||
			t != j ||
			j != std::to_string(1000 + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and i is " << i << std::endl;
			return __LINE__;
		}
	}



	return 0;
}


#else


void printTestMessages(std::ostream& theStream) {
	theStream << "This is the templated version of the queue tests: Make sure you pass the non-templated also. You must pass both for full credit." << std::endl;
	theStream << "You can switch these on line " << TEMPLATED_HEADER_LINE << " of " << TEMPLATED_HEADER_FILE << std::endl;
}

int normalTest1() {
	// some of my setup for tracking memory usage. 
	// See https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
	_CrtMemState sB4, sMiddle, sEnd;
	// Set a memory Checkpoint
	_CrtMemCheckpoint(&sB4);
	// Note that this code is put into its own scope so the dtor gets called BEFORE the end of the method
	// so we can test if the dtor is leaking memory
	{
		farmingdale::queue<std::string> myq;
		const int some_large_number = 100000;
		for (int i = 0; i < some_large_number; ++i) {
			if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i + 1000))) {
				std::cerr << "Error on line " << __LINE__ << std::endl;
				return __LINE__;
			}
		}
		// push should never fail
		std::string j;
		std::string t;
		for (int i = 0; i < some_large_number; ++i) {
			if (farmingdale::statusCode::FAILURE == myq.peek(t) ||
				farmingdale::statusCode::FAILURE == myq.dequeue(j) ||
				t != j ||
				j != std::to_string(1000 + i)) {
				std::cerr << "Error on line " << __LINE__ << " t is " << t << " and i is " << i << std::endl;
				return __LINE__;
			}
		}

		farmingdale::queue<int> myIntQ;
		for (int i = 0; i < some_large_number; ++i) {
			if (farmingdale::statusCode::FAILURE == myIntQ.enqueue(i + 1000)) {
				std::cerr << "Error on line " << __LINE__ << std::endl;
				return __LINE__;
			}
		}

		// Set a memory Checkpoint
		_CrtMemCheckpoint(&sMiddle);

		// push should never fail
		int x;
		int y;
		for (int i = 0; i < some_large_number; ++i) {
			if (farmingdale::statusCode::FAILURE == myIntQ.peek(x) ||
				farmingdale::statusCode::FAILURE == myIntQ.dequeue(y) ||
				y != x ||
				x != (1000 + i)) {
				std::cerr << "Error on line " << __LINE__ << " x is " << x << " and i is " << i << std::endl;
				return __LINE__;
			}
		}
	} // extra scope to call dtor for stack
	  // Set a memory Checkpoint
	_CrtMemCheckpoint(&sEnd);

	// Now validate that you successfully cleaned up the memory in your destructor
	if ((sMiddle.lCounts[_NORMAL_BLOCK] - sB4.lCounts[_NORMAL_BLOCK]) < 1) {
		std::cerr << "It doesn't seem like you allocated any memory" << std::endl;
		return __LINE__;
	}
	if ((sEnd.lCounts[_NORMAL_BLOCK] - sB4.lCounts[_NORMAL_BLOCK]) != 0) {
		std::cerr << "It seems you are leaking memory in your descructor" << std::endl;
		return __LINE__;
	}

	return 0;
}

int normalTest2() {
	// a more curated version of the test. Comments inline to tell you what I'm doing.
	farmingdale::queue<std::string> myq;
	for (int i = 0; i < myq.initialQueueCapacity(); ++i) {
		if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i + 1000))) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}
	std::string j;
	std::string t;
	// at this point the queue is full of (1000),(1001)...(1015) [assuming capacity is 15] 
	// Now, empty the first 5 elements. 
	for (int i = 0; i < 5; ++i) {
		if (farmingdale::statusCode::FAILURE == myq.peek(t) ||
			farmingdale::statusCode::FAILURE == myq.dequeue(j) ||
			t != j ||
			j != std::to_string(1000 + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and t is " << t << std::endl;
			return __LINE__;
		}
	}
	// Now put 4 more in
	std::string nextString;
	for (int i = 0; i < 4; ++i) {
		nextString = std::to_string(i + 1000 + myq.initialQueueCapacity());
		if (farmingdale::statusCode::FAILURE == myq.enqueue(nextString)) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}
	// so now the queue is (1005), (1006), ... , (1015), (1016), (1017), (1018), (1019)
	// but in memory this is 
	// (1017), (1018), (1019), ....(1005), ..., (1015), (1016)
	// Now, we're going to go do something different. Let's start with (1020) and add some
	// large number of items, causing a reallocation
	const int some_large_number = 100000;
	// Note that j was the last number we saw above. Convert to string and add one
	int expected_next_value = std::stoi(j) + 1;
	// note that nextString was the last string we inserted. Add one to that to start
	int startingInt = std::stoi(nextString) + 1;
	for (int i = startingInt; i < some_large_number + startingInt; ++i) {
		if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i))) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}
	std::string e;
	// make sure they come out correctly.... (we have one less than when we started (-5+4)
	for (int i = 0; i < some_large_number; ++i) {
		// shift the expected value up by 5....
		if (farmingdale::statusCode::FAILURE == myq.peek(t) ||
			farmingdale::statusCode::FAILURE == myq.dequeue(j) ||
			t != j ||
			j != std::to_string(expected_next_value + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and t is " << t <<
				" and i is " << i << " and I expected " << expected_next_value + i << std::endl;
			return __LINE__;
		}
	}
	return 0;
}

int copyCtorTest() {
	farmingdale::queue<std::string> myq;
	const int some_large_number = 100000;
	for (int i = 0; i < some_large_number; ++i) {
		if (farmingdale::statusCode::FAILURE == myq.enqueue(std::to_string(i + 1000))) {
			std::cerr << "Error on line " << __LINE__ << std::endl;
			return __LINE__;
		}
	}

	farmingdale::queue<std::string> myOtherQ(myq);
	farmingdale::queue<std::string> aThirdQueue = myq;
	if (myOtherQ != myq) {
		std::cerr << "Error on line " << __LINE__ << std::endl;
		return __LINE__;
	}
	if (aThirdQueue != myq) {
		std::cerr << "Error on line " << __LINE__ << std::endl;
		return __LINE__;
	}
	std::string j;
	std::string t;
	for (int i = 0; i < some_large_number; ++i) {
		if (farmingdale::statusCode::FAILURE == myOtherQ.peek(t) ||
			farmingdale::statusCode::FAILURE == myOtherQ.dequeue(j) ||
			t != j ||
			j != std::to_string(1000 + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and i is " << i << std::endl;
			return __LINE__;
		}
	}
	for (int i = 0; i < some_large_number; ++i) {
		if (farmingdale::statusCode::FAILURE == aThirdQueue.peek(t) ||
			farmingdale::statusCode::FAILURE == aThirdQueue.dequeue(j) ||
			t != j ||
			j != std::to_string(1000 + i)) {
			std::cerr << "Error on line " << __LINE__ << " j is " << j << " and i is " << i << std::endl;
			return __LINE__;
		}
	}

	return 0;
}



#endif TEMPLATED_QUEUE
