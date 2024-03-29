// (c) 2013-2020, David Gerstl, all rights reserved
// For the use of my C++ students to use as a base to implement
// dynamic arrays, exceptions and operator overloading, and templates


// Class farmingdale::queue: General std::string queue implementation based on circular array. 
// We will add dynamic arrays (for unlimited size), templates (to allow multiple types) etc.

// very minimal set of tests. 
#include <iostream>
#include <string>
#include "farmingdaleDynamicContiguousMemoryQueue.h"


#ifndef TEMPLATED_QUEUE
// Non Template Code

// test methods return 0 on success. Otherwise return the line number 
// on which the error occurred (For ease in finding it. Note that the 
// preprocessor directive __LINE__ will give you this line.
int studentTest() {
	return 0;
}


farmingdale::queue::queue()
//set oldest and nextInsert to 0
//Initialization list
	:
	oldestIndex(0),
	nextInsertIndex(0),
	currentCapacity(INITIAL_QUEUE_SIZE)
{
	data = new std::string[currentCapacity];
}

farmingdale::queue::queue(const queue& copyMe)
	:
	//Step 1 : Copy oldest index from copyMe 
	oldestIndex(copyMe.oldestIndex),
	//Step 2: Copy NII from copyMe 
	nextInsertIndex(copyMe.nextInsertIndex),
	//Step 3: copy the currentCapacity
	currentCapacity(copyMe.currentCapacity)
{
	//Step 4: allocate the buffer (using new) of size currentCapacity, no try catch
	std::string *copyData = new std::string[currentCapacity];
	data = copyData;

	//Step 5: Copy all of the items in the array from oldest index...NII into data.
	for (int i = copyMe.oldestIndex; i != copyMe.nextInsertIndex; i = nextIndexOf(i)) {
		data[i] = copyMe.data[i];
	}
}

farmingdale::statusCode farmingdale::queue::dequeue(std::string& removedValue) {
	//Step 1: check if empty if so return failure
	if (isEmpty())
	{
		return farmingdale::FAILURE;
	}
	//Step 2: put item [oldestIndex] into returnedElement
	removedValue = data[oldestIndex];

	//Step 3: Move index up
	oldestIndex = nextIndexOf(oldestIndex);
	//Step 4: Return Success
	return farmingdale::SUCCESS;
}

farmingdale::statusCode farmingdale::queue::peek(std::string& retrievedValue) const {

	//Step 1: check if empty if so return failure
	if (isEmpty())
	{
		return farmingdale::FAILURE;
	}
	//Step 2: put item [oldestIndex] into returnedElement
	retrievedValue = data[oldestIndex];
	//Step 3: Return Success
	return farmingdale::SUCCESS;
}

farmingdale::statusCode farmingdale::queue::enqueue(std::string addMe) {
	//Step 1: Check if queue is full, if so, grow the queue as follows....
	
	if (isFull()) {
		//Step 1a: allocate new memory
		int newCapacity = currentCapacity * 2; //aprox *2, size ~= 2x curentCapacity

		std::string* theNewDataMemory;
		try {
			theNewDataMemory = new std::string[newCapacity];
		}
		catch (std::bad_alloc) {
			return farmingdale::FAILURE;
		}
		
		//Step 1b: copy the data, deep copy
		int firstData = 0;
		for (int i = oldestIndex; i != nextInsertIndex; i = nextIndexOf(i)) {
			theNewDataMemory[firstData] = data[i];
			firstData++;
		}
		//Step 1c: Fix the indexes.
		oldestIndex = 0;
		nextInsertIndex = firstData;
		//Step 1d: call delete[] on the old memory
		delete[] data;
		//Step 1e: Set data to the new memory
		data = theNewDataMemory;
		//Step 1f: set the new capacity
		currentCapacity = newCapacity;
	}
	//Step 2: if not, we'll add the item in slot [nextInsert]
	data[nextInsertIndex] = addMe;
	//Step 3: Advance nextInsert to the next slot
	nextInsertIndex = nextIndexOf(nextInsertIndex);
	//Step 4: Return Sucess
	return farmingdale::SUCCESS;

}

bool farmingdale::queue::operator==(const queue& otherQueue) const {
	int myIterator = oldestIndex;
	int otherIterator = otherQueue.oldestIndex;
	while (myIterator != nextInsertIndex && otherIterator != otherQueue.nextInsertIndex) {
		//compare
		if (data[myIterator] != otherQueue.data[otherIterator]) {
			return false;
		}
		//advance 
		myIterator = nextIndexOf(myIterator);
		otherIterator = otherQueue.nextIndexOf(otherIterator);
	}
	if (myIterator != nextInsertIndex || otherIterator != otherQueue.nextInsertIndex) {
		return false;
	}
	return true;
}

void farmingdale::queue::printToStream(std::ostream& theStream) {
	theStream << "(oldest)";
	for (int i = oldestIndex; i != nextInsertIndex; i = nextIndexOf(i)) {
		theStream << data[i];
		if (nextIndexOf(i) != nextInsertIndex) {
			theStream << " ; ";
		}
	}
	theStream << "(newest)";
}

farmingdale::queue::~queue() {
	delete[] data;
}

#else // TEMPLATED_QUEUE
// Template cpp

// test methods return 0 on success. Otherwise return the line number 
// on which the error occurred (For ease in finding it. Note that the 
// preprocessor directive __LINE__ will give you this line.
int studentTest() {
	return 0;
}



#endif //  TEMPLATED_QUEUE