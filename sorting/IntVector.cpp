/*
 * Implementation of simple dynamic array of integers
 */


#include "IntVector.h"
#include <iostream>
#include <stdexcept>


using namespace std;

/* constructor */
IntVector::IntVector()
{
	array = new int[0];
	used = 0;
	cap = 0;
}




/* destructor */
IntVector::~IntVector()
{
	delete [] array;
}

/*
 * deletes array sets new array with zero 
 * size and capacity
 */
void IntVector::destroy()
{
	delete [] array;
	array = new int[0];
	used =0;
	cap = 0;
}

/* copy constructor */
IntVector:: IntVector(const IntVector &source)
{
	cap = source.cap;
	used = source.used;
	array = new int[cap];
	for(int i=0; i < used; i++){
		array[i] = source.array[i];
	}
}

/*
 * overload assignment operator(=)
 */
IntVector &IntVector::operator=(const IntVector &rhs)
{
	if(this != &rhs){
		delete [] array;
		cap = rhs.cap;
		used = rhs.used;
		array = new int[cap];
		for(int i=0; i < used; i++){
			array[i] = rhs.array[i];
		}
		
	}
	return *this;
}



/*
 * overload assignment operator []
 *  used to access easily any indexed element without using get function
 */
int &IntVector::operator[](int index) const
{ 
	if(index > used -1) 
		throw runtime_error("out of bounds");
	else return array[index];
}
	
/* returns value at index  */
int IntVector::get(int index) const
{
	if(index > used -1)
		throw runtime_error( "received negative value" );
	else return array[index];
}

/*
 * sets value at some index to new value
 * old value is lost
 */
void IntVector::set(int index, int newVal)
{
	if(index < used && used != cap)
		array[index] = newVal;
}

/* adds new value at end of array */
void IntVector::add(int newVal)
{
	if(used >= cap) expand();
	array[used] = newVal;
	used++;
}

/* returns number of elements */
int IntVector::size() const
{
	return used;
}

/* returns array size */
int IntVector::capacity() const
{
	return cap;
}

/*
 * helper function to expand array
 * if needed douvles size of array
 * copies elements to new array
 */ 
void IntVector::expand()
{
	int *new_ar;
	if(cap == 0){
		new_ar = new int[1];
		cap = 1;
	}
	else new_ar = new int[cap*2];
	
	for(int i = 0; i < used; i++){
		new_ar[i] = array[i];
	}
	delete [] array;
	array = new_ar;
	cap *=2;
}