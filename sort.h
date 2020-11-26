#ifndef sort_h__
#define sort_h__


// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r);

/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r);

/* UTILITY FUNCTIONS */
/* Function to print an array */
void printArray(int A[], int size);


#endif // sort_h__