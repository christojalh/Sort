// Questions
// How do I identify which overloaded function I want to use when I input it as a parameter? 

/* Christopher Lee	   
 * March 27th, 2018
 * 
 * This program contains implementations of several sort algorithms, as well as some optimizations: 
 * 1)Insertionsort
 * 2)Mergesort
 * 	-Uses insertionSort to sort subarrays <= 7 elements in order to avoid the high cost of recursion
 * 	   for small arrays
 * 	-Checks if the subarrays are already in sorted order before merging them. If they are, we 
 * 	   skip that iteration of merging the subarrays to avoid redundantly sorting
 * 3)Quicksort
 * 	-Shuffles the array to probabilistically lower the runtime. Certain orderings can cause the 
 * 	   algorithm to reach quadratic time. Shuffling reduces this probability significantly.
 * 	-Finds the median of the lo, mid, hi values of the subarray to use as the partitioning index.
 * 	   Having our partitioning index closer to the middle increases the efficiency of recursion 
 * 	   by making the subarrays closer to 1/2 the size of the parent.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream> 
#include <chrono>
#include <algorithm>
#include <functional>

class Timer {
private:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> m_beg;

public: 
	Timer() : m_beg(clock_t::now()) {

	}

	void reset() {
		m_beg = clock_t::now();
	}

	double elapsed() const {
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};

std::ostream& operator<< (std::ostream& os, const std::vector<int>& vec) {
	for (int ii = 0; ii < vec.size(); ++ii) {
		std::cout << vec[ii] << " \t";
		if ((ii + 1) % 7 == 0) {
			std::cout << std::endl;
		}
	}
	std::cout << "\n\n";

    return os;
}

int less(int x, int y) {
	if (x < y) { return x; }
	else       { return y; }
}

// generates a number in range [min,max]
int genRandInt(int min, int max) {
	// static used here so we only calculate the value once
	static const double fraction = 1.0 / (RAND_MAX + 1.0);  
	int range = max - min + 1;
	return min + static_cast<int>(range*(rand()*fraction));
}

// modifies vec to hold 100 random numbers in range [0,100]
void genRand( std::vector<int>& vec, int numInts ) {
	vec.resize(numInts);
	for (int ii = 0; ii < numInts; ++ii) {
		vec[ii] = genRandInt(0,100);
	}
}

// returns the median of lo, mid, high
int medianOf3(std::vector<int>& vec,int lo,int mid,int hi) {
	int x = vec[lo] - vec[mid];
	int y = vec[mid] - vec[hi];
	int z = vec[lo] - vec[hi];

	if (x*y >= 0) { return mid; };
	if (x*z >= 0) { return hi; };
	return lo; 

}

void swap( std::vector<int>& vec, int x, int y) {
	int temp = vec[x];
	vec[x] = vec[y]; 
	vec[y] = temp;
}

// durstenfield/knuth shuffle 
void shuffle( std::vector<int>& vec ) {
	for (int i = (vec.size() - 1); i > 0; --i) {
		int r = genRandInt(0, i);
		swap(vec, i, r);
	}
}

void insertionSort( std::vector<int>& vec ) {
	for (int i = 0; i < vec.size(); ++i) {
		for (int j = i; j > 0; --j) {
			if ( vec[j] < vec[j-1] ) {
				swap(vec, j, j-1);
			}
			else {
				break;
			}
		}
	}
}

// template <typename Iterator>
// void insertionSort( std::vector<int>& vec, Iterator begin, Iterator end) {
// 	int lo = 0; 
// 	int hi = end - begin; 
// 	for (int i = lo; i < hi; ++i) {
// 		for (int j = i; j > lo; --j) {
// 			if ( vec[j] < vec[j-1] ) {
// 				swap(vec, j, j-1);
// 			}
// 			else {
// 				break;
// 			}
// 		}
// 	}
// }

void merge(std::vector<int>& vec, std::vector<int>& aux, int lo, int mid, int hi) {

	// we have to update our temp array every time because 
	// the ordering of vec changes after each call to merge
	aux = vec; 

	int i = lo; // start index of first subarray
	int j = mid + 1; // start index of second subarray
	for (int k = lo; k <= hi; ++k) {

		if      ( i > mid )            { vec[k] = aux[j++]; }
		else if ( j > hi )             { vec[k] = aux[i++]; }
		else if ( aux[i] < aux[j] )    { vec[k] = aux[i++]; }
		else                           { vec[k] = aux[j++]; }
	}
}

void mergeSort(std::vector<int>& vec) {

	// first we have to make a temp array because we are copying 
	// indices over out of order
	std::vector<int> aux = vec;

	int N = vec.size();
	for (int sz = 1; sz < N; sz += sz) {
		for (int lo = 0; lo < N - sz; lo += sz + sz) {
			int mid = lo + sz - 1;
			// at the end of the array, lo + sz*2 might
			// overflow the array, so we choose the less() of 
			// the end of the array (N-1) or lo + sz*2			
			int hi = less(lo + sz + sz - 1, N-1);

			// if the subarrays are already in order, merging
			// changes nothing so we can skip it
			if (vec[mid] <= vec[mid + 1]) {
				continue;
			}
			else {
			merge(vec, aux, lo, mid, hi);
			}
		}
	}
}

// template <typename Iterator>
// void mergeSort( std::vector<int>& vec, Iterator begin, Iterator end) {

// 	// first we have to make a temp array because we are copying 
// 	// indices over out of order
// 	std::vector<int> aux = vec;

// 	int N = end - begin;
// 	for (int sz = 1; sz < N; sz += sz) {
// 		for (int lo = 0; lo < N - sz; lo += sz + sz) {
// 			int mid = lo + sz - 1;
// 			// at the end of the array, lo + sz*2 might
// 			// overflow the array, so we choose the less() of 
// 			// the end of the array (N-1) or lo + sz*2			
// 			int hi = less(lo + sz + sz - 1, N-1);

// 			// if the subarrays are already in order, merging
// 			// changes nothing so we can skip it
// 			if (vec[mid] <= vec[mid + 1]) {
// 				continue;
// 			}
// 			else {
// 			merge(vec, aux, lo, mid, hi);
// 			}
// 		}
// 	}
// }

void quick( std::vector<int>& vec, int lo, int hi ) {
	if (hi <= lo) { // this signals the end of our recursion
		return; 
	}

	// Our algorithm is most efficient if the partitioning 
	// index is in the middle, so we make the median of 
	// lo, mid, hi our partitioning index to increase the
	// likelihood of that occurring
	swap(vec, lo, medianOf3(vec,lo, lo + (hi-lo) / 2, hi));

	int lt = lo; // lt = less than. Everything to the left is 
				 //   less than the partitioning element
	int gt = hi; // gt = greater than. Everything to the right
				 //   is greater than the partitioning element
	int i = lo;
	while (gt >= i) {
		// lt is the partitioning index in this case
		if 		(vec[i] < vec[lt]) 	{ swap(vec, lt, i); ++i; ++lt; }
		else if (vec[i] > vec[lt]) 	{ swap(vec, gt, i); --gt; }
		else 						{ ++i; }
	}

	// Since our partitioning element(s) is now in the right place,
	// it is not included in the recursion beyond this point
	quick(vec, lo, lt-1);
	quick(vec, gt+1, hi);
}

void quickSort( std::vector<int>& vec ) {
	// certain orderings increase the time complexity to n^2.
	// Shuffling reduces the probability of one of these orderings
	shuffle(vec);

	quick( vec, 0, vec.size() - 1 );
}

// template <typename Iterator>
// void quickSort( std::vector<int>& vec, Iterator begin, Iterator end) {
// 	shuffle(vec);

// 	quick( vec, 0, (end - begin) - 1);
// }

// tests shuffle() by shuffling an array of [0,100)
void testShuffle() {
	std::cout << "Printing a shuffled array of values [0,99]\n";
	int numInts = 100; 
	std::vector<int> vec;
	vec.resize(numInts);
	for (int i = 0; i<numInts; ++i) {
		vec[i] = i; 
	}
	shuffle(vec);
	std::cout << vec;	
}


// tests genRandInt by printing 100 random numbers
void testRNG() {
	std::cout << "Printing an array of 100 randomly generated numbers from [0,100]\n";
	int numInts = 100;
	std::vector<int> vec;
	vec.resize(numInts);
	for (int i = 0; i<numInts; ++i) {
		vec[i] = genRandInt(0,numInts);
	}
	quickSort(vec);
	std::cout << vec;	
}

bool isSorted(std::vector<int>& vec) {
	for (int i=0; i<(vec.size() - 1); ++i) {
		if (!(vec[i] <= vec[i+1])) {
			return false;
		}
	}
	return true;
}

void testSort( std::function<void(std::vector<int>&)> sort, std::vector<int>& vec ) {
	Timer timer;
	sort(vec);
	std::cout << timer.elapsed() << " seconds\n";
}

void testStd( std::vector<int>& vec ) {
	Timer timer;
	std::sort(vec.begin(), vec.end());
	std::cout << timer.elapsed() << " seconds\n";	
}

void testAll( int numInts ) {
	std::vector<int> vec; 
	std::vector<int> vec2;

	genRand(vec,numInts);
	// we store the vector in vec2 to use as a standard input
	vec2 = vec; 
	std::cout << "The array size is " << numInts << "\n";

	std::cout << "mergeSort: ";
	testSort(mergeSort, vec);
	vec = vec2;

	std::cout << "quickSort: ";
	testSort(quickSort, vec);
	vec = vec2;		

	std::cout << "std::sort: ";
	testStd(vec);
	vec=vec2;

	// std::cout << "insertion: ";
	// testSort(insertionSort, vec);
	// vec = vec2;	

	std::cout << "\n";

}

int main() {
	srand(static_cast<unsigned int>(time(0)));
	testAll(100000);

	// testShuffle();
	// testRNG();
	return 0; 
}