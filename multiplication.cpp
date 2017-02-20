#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

#define BASE 32767; // base of the numerical notation



/* Naive multiplication of two numbers: (x of size len_x) and (y of size len_y)
 * Saves the result into result[] (initializes it with 0 beforehand) */
void naive_multiply(size_t x[], size_t y[], size_t len_x, size_t len_y, size_t result[]) {

  size_t interim[len_x + 1];
  size_t carryover;
  unsigned long long temp;

  // initialize result[] as 0
  for (int i = 0; i < len_x + len_y + 1; ++i) {
    result[i] = 0;
  } 

  for (int i = 0; i < len_y; ++i) {
    
    // multiply and save the result in interim[]
    carryover = 0;  
    for (int j = 0; j < len_x; ++j) {
      temp = y[i] * x[j] + carryover;
      interim[j] = temp % BASE;
      carryover = temp / BASE;
    }
    interim[len_x] = carryover;

    // sum interim[] and result[]
    carryover = 0;
    for (int j = 0; j <= len_x; ++j) {
      temp = interim[j] + result[i + j] + carryover;
      result[i + j] = temp % BASE;
      carryover = temp / BASE;
    }
    result[i + len_x + 1] += carryover;
  }

}




/* Multiplication of two numbers: (x of size len_x) and (y of size len_y)
 * using Karatsuba's method
 * Saves the result into result[] (initializes it with 0 beforehand) */
void karatsuba_multiply(size_t x[], size_t y[], size_t len_x, size_t len_y, size_t result[]) {

}




// prints a number stored in array number[] of size length
void print(size_t number[], size_t length) {
  for (int i = length - 1; i >= 0; --i) {
    cout << number[i];
  }
  cout << "\n";
}


// fills array[] of size "length" with random numbers not greater than BASE
void generateRandom(size_t array[], size_t length) {  
  for (int i = 0; i < length; ++i) {
    array[i] = rand() % BASE;
  }
}




int main(int argc, char** argv) {
  
  // size_t == unsigned int
  const size_t LEN1 = 100, LEN2 = 100; // max number of orders (number positions) in numbers

  size_t number1[LEN1], number2[LEN2];  // arrays representing the numbers to multiply
  size_t result[LEN1 + LEN2 + 1]; // array to store the result

  srand(time(NULL)); // initialize random numbers generator
  generateRandom(number1, LEN1);
  generateRandom(number2, LEN2);

  print(number1, LEN1);
  cout << " * \n";
  print(number2, LEN2);
  cout << " = \n";
  
  naive_multiply(number1, number2, LEN1, LEN2, result);
  cout << "Naive multiply result:\n";
  print(result, LEN1 + LEN2);

  // karatsuba_multiply(number1, number2, LEN1, LEN2, result);
  // cout << "karatsuba multiply result:\n";
  // print(result, LEN1 + LEN2);

  return 0;
}