/*
 * Georgios Pesmazoglou
 *
 * Sorting exercise
 * implementation of basic sortin algorithms: bubble sort, insertionsort
 * and quicksort
 */

#include <iostream>
#include <cstdlib>

#include "IntVector.h"

using namespace std;

string algorithmFromCommandLine(int argc, char *argv[]);
void   usageAbort (string progname, string message);
void   readNumbers(istream &input, IntVector &data);
void   sortNumbers(string algorithm, IntVector &data);
void   printNumbers(const IntVector &data);
void   sort2(IntVector &data);
void   sort3(IntVector &data, int left, int right);
void   swap(IntVector &vector, int i, int j);
void   bubbleSort(IntVector &data);

/*
 * Uses given sorting algorithm
 * reads in data from standard input
 * prints the sorted list
 */
int main(int argc, char *argv[])
{
        IntVector data;
        string sortAlgorithm;
        sortAlgorithm = algorithmFromCommandLine(argc, argv);
        readNumbers(cin, data);
	    sortNumbers(sortAlgorithm, data);
        printNumbers(data);

        return 0;
}

/*
 * Abort the program with a message on standard error
 * explaining how to run the program correctly from the 
 * command line.
 */
void usageAbort(string progname, string message)
{
        cerr << message << endl
             << "Usage:  " << progname << " algorithm" << endl
             << "     where algorithm is one of "
             << "bubble, sort2, or sort3"
             << endl;
        exit(1);
}

/*
 * string algorithmFromCommandLine(int argc, char *argv[]);
 *
 * Return sorting algorithm specified on command line.
 *
 * Abort program if no algorithm is specified or if specified
 * algorithm is not supported.
 *
 */
string algorithmFromCommandLine(int argc, char *argv[])
{
	string sort = argv[1];
	
	if(argc <= 1) usageAbort(argv[0],"No Algorithm");
	
	if((sort == "bubble") ||  (sort == "sort2") ||
		(sort == "sort3")) return argv[1];
	else usageAbort(argv[0],"Wrong algorithm");
	
	return " ";
		
}
/*
 * void readNumbers(istream &input, IntVector &data);
 *
 * Read in a collection of integers from the given input stream
 * and add them to the end of the data vector.  Input numbers are
 * separated by whitespace (they don't have to be one per line).
 *
 * My implementation does not distinguish between failure due to end
 * of file or any other failure of the input device.  Yours may do
 * likewise. 
 *
 */

/* 
 * uses two variables when reading,
 * there is a variable storing the last input and one that
 * stores the current as to avoid end of file error
 */
void readNumbers(istream &input, IntVector &data)
{
	int curr = 0;
	int prev;
	int i =0;
	input >> prev;
	while(!input.eof()){
		input >> curr;
		data.add(prev);
		prev = curr;
		i++;	
	}
}

/*
 * void sortNumbers(string algorithm, IntVector &data);
 *
 * Updates contents of data so items are sorted using the chosen
 * algorithm.
 */
void sortNumbers(string algorithm, IntVector &data)
{
		if(algorithm == "bubble")
			bubbleSort(data);
		else if(algorithm == "sort2")
			sort2(data);
		else if(algorithm == "sort3")
			sort3(data,0,data.size()-1);
	
}
/*
 * void   printNumbers(const IntVector &data);
 *
 * Print contents of data one item per line.
 */
void   printNumbers(const IntVector &data)
{
	for(int i = 0; i < data.size(); i++){
		cout << data[i] << endl;
	}
}
	
	
	
/*
 * void bubbleSort(IntVector &data);
 *
 * In-place sort of contents of data using the bubble sort algorithm.
 */

//use of nested loop, (n squared complexity)
// see readme file
void bubbleSort(IntVector &data)
{
	for(int i = 0; i < data.size() -1; i++){
		for(int n = 0; n < data.size() - i-1; n++){
			if(data[n] > data[n+1])
				swap(data, n, n+1);
		}
	}

}

/* insertionsort */
void sort2(IntVector &data)
{
	for(int i =0; i < data.size(); i++){
		for(int n = i;n > 0 && data[n-1] > data[n]; n--)
				swap(data, n, n-1);
	}
	
}

/* quicksort */
void sort3(IntVector &data, int left, int right)
{
	int min = left;
	int max = right;
	int pivot = data[(min + max)/2];
	
	while(min <=max){
		while (data[min] < pivot)
			min++;
		while(data[max] > pivot)
			max--;
		if(min <= max){
			swap(data, min, max);
			min++;
			max--;
		}
	}	
	if(left < max)
		sort3(data, left, max);
	if(min < right)
		sort3(data, min, right);

}
/*
 * void swap(IntVector &vector, int i, int j);
 *
 * Updates the given vector so that the items at position i and j
 * are swapped.
 */
void swap(IntVector &vector, int i, int j)
{
	int temp;
	temp = vector[i];
	vector[i] = vector[j];
	vector[j] = temp;
}