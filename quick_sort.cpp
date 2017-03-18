#include <iostream>
using namespace std;

void quick_sort(int array[], int left, int right) {
  int i = left, j = right;
  int temp;
  int center = array[(left + right) / 2];
  while (i <= j) {
    while (array[i] < center)
      i++;
    while (array[j] > center)
      j--;

    if (i <= j) {
      temp = array[i];
      array[i] = array[j];
      array[j] = temp;
      i++;
      j--;
    }
  }

  if (left < j)
    quick_sort(array, left, j);
  if (right > i)
    quick_sort(array, i, right);
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

  quick_sort(array, 0, size - 1);

  cout << "Sorted array: ";
  for (size_t i = 0; i < size; ++i) {
    cout << array[i] << " ";
  }
  cout << endl;

  return 0;
}
