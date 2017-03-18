#include <iostream>
using namespace std;

template <typename T>
int binarySearch(T array[], size_t size, T value) {
  size_t middle;
  size_t left = 0;
  size_t right = size - 1;
  while (left < right) {
    middle = (left + right) / 2;
    if (value < array[middle]) {
      right = middle - 1;
    } else if (value > array[middle]) {
      left = middle + 1;
    } else {
      return middle;
    }
    return -1;
  }
}

int main() {
  size_t size;
  int value;

  cout << "Enter the size of array: ";
  cin >> size;
  int* array = new int[size];

  cout << "Enter " << size << " integer elements of array: ";
  for (size_t i = 0; i < size; ++i) {
    cin >> array[i];
  }

  cout << "Enter a key to search: ";
  cin >> value;

  int index_found = binarySearch(array, size, value);
  if (index_found < 0)
    cout << "Value is not found!" << endl;
  else 
    cout << "Value is found in position " << index_found << endl;

  return 0;
}