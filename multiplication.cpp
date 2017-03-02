#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstring>
using namespace std;

#define BASE 32767    // base of the numerical notation
#define MIN_LENGTH_FOR_KARATSUBA 4    // if the number is shorter, it won't be multiplied using Karatsuba method
typedef int digit;    // type of one digit in the chosen numerical notation to accomodate tha max value of BASE
typedef long long int double_digit;    // type to accomodate tha max value of BASE*BASE


// Measures execution time
static inline uint64_t get_cycles() {
  uint64_t t;
  __asm__ __volatile__ ("rdtsc" : "=A"(t));
  return t;
}


/* Struct to store long numbers in the chosen numerical notation */
struct LongNumber { 
  digit* val; // array to store the "digits" where [0] element stores the right-most (low-order) digit of the number
  size_t len; // number of "digits" (size of the array)

  // Constructor
  LongNumber(size_t length) {
    len = length;
    val = new digit[len];
    memset(val, 0, sizeof(val)*len); // fill number with zeros
  }

  friend LongNumber naive_multiply(LongNumber x, LongNumber y);
  friend LongNumber karatsuba_multiply(LongNumber x, LongNumber y);
  friend void print(LongNumber number);


  // Increase the number of digits in the long number by adding heading zeros
  void increaseDigits(size_t new_length) {
    if (new_length > len) {
      digit* temp = new digit[new_length];  // to store the new number
      memcpy(temp, val, len * sizeof(val)); // copy the value
      memset(temp + len, 0, (new_length - len) * sizeof(val)); // add zeros
      val = temp;
      len = new_length;
    }
  }

  // Normalize the number by deleting heading zeros
  void normalizeDigits() {
    size_t i = len - 1;
    while (val[i] == 0 && i > 0) --i;
    len = i + 1;
  }

  // Multiplies the number by BASE^n:
  // 1. Increases the length by n
  // 2. "Shifts" value by n digits
  // 3. Fills n empty digits (val[0...n-1]) with zeros
  void shift(size_t n) {
    digit* temp = new digit[len + n];
    memset(temp, 0, n * sizeof(val));
    memcpy(temp + n, val, len * sizeof(val));
    val = temp;
    len += n;
  }


  // Operator + to sum two long numbers
  // Returns (this + A)
  LongNumber operator + (LongNumber A) {
    // Bring two long number (this and A) to the same length
    if (len > A.len) {
      A.increaseDigits(len);
    } else if (len < A.len) {
      this->increaseDigits(A.len);
    }

    LongNumber R(len); // to store the result
    digit carryover = 0;
    double_digit temp;

    for (size_t i = 0; i < len; ++i) {
      temp = val[i] + A.val[i] + carryover;
      R.val[i] = temp % BASE;
      carryover = temp / BASE;
    }

    if (carryover != 0) {
      R.increaseDigits(len + 1);
      R.val[len] = carryover;
    }

    return R;
  }


  // Operator - to subtract two long numbers: A from this 
  // Returns (this - A)
  LongNumber operator - (LongNumber A) {
    // Bring two long number (this and A) to the same length
    if (len > A.len) {
      A.increaseDigits(len);
    } else if (len < A.len) {
      this->increaseDigits(A.len);
    }

    LongNumber R(len); // to store the result
    digit carryover = 0;
    double_digit temp;  

    for (size_t i = 0; i < len; ++i) {
      temp = val[i] - A.val[i] - carryover;
      if (temp < 0) {
        R.val[i] = temp + BASE;
        carryover = 1;
      } else {
        R.val[i] = temp;
        carryover = 0;        
      }
    }

    if (carryover != 0) {
      R.increaseDigits(len + 1);
      R.val[len] = carryover;
    }
    
    return R;
  }


  // Operator * to multiply two long numbers: this and A
  LongNumber operator * (LongNumber A) {
    this->normalizeDigits();
    A.normalizeDigits();
    if ((len == 1 && val[0] == 0) || (A.len == 1 && A.val[0] == 0)) {
      LongNumber result(1);
      return result;
    }
    if (len == 1 && val[0] == 1) return A;
    if (A.len == 1 && A.val[0] == 1) return *this;

    if (len < MIN_LENGTH_FOR_KARATSUBA || A.len < MIN_LENGTH_FOR_KARATSUBA) {
      return (naive_multiply(*this, A));
    } else {
      return (karatsuba_multiply(*this, A));
    }
  }

};



/* Naive multiplication of two long numbers */
LongNumber naive_multiply(LongNumber x, LongNumber y) {

  LongNumber interim(x.len + y.len), result(x.len + y.len);
  digit carryover;
  double_digit temp;

  for (int i = 0; i < y.len; ++i) {
    carryover = 0;
    memset(interim.val, 0, sizeof(interim.val)*interim.len); // initialize interim as 0

    // multiply y[i] and x[0...x.len] (one digit of y to all digits of x) 
    // and save the result in interim
    for (int j = 0; j < x.len; ++j) {
      temp = y.val[i] * x.val[j] + carryover;
      interim.val[i + j] = temp % BASE;
      carryover = temp / BASE;
    }
    interim.val[i + x.len] = carryover;

    result = result + interim;
  }
  result.normalizeDigits();
  return result;
}



/* Multiplication of two long numbers using Karatsuba algorithm */
LongNumber karatsuba_multiply(LongNumber a, LongNumber b) {

  // Bring two long number to the same length
  if (a.len > b.len) {
    b.increaseDigits(a.len);
  } else if (a.len < b.len) {
    a.increaseDigits(b.len);
  }

  // Make length of a and b even
  if (a.len % 2 == 1) {
    a.increaseDigits(a.len + 1);
    b.increaseDigits(b.len + 1);
  }

  // Split a and b both in two parts: a0 and a1, b0 and b1
  size_t m = a.len / 2;
  LongNumber a0(m), a1(m), b0(m), b1(m);
  a0.val = a.val;
  a1.val = a.val + m;
  b0.val = b.val;
  b1.val = b.val + m;

  LongNumber mul_a0_b0 = a0 * b0;
  LongNumber mul_a1_b1 = a1 * b1;

  LongNumber middle_part = (a0 + a1) * (b0 + b1) - mul_a0_b0 - mul_a1_b1;
  middle_part.shift(m);  // = middle_part * BASE^m

  LongNumber last_part = mul_a1_b1;
  last_part.shift(2 * m);  // = last_part * BASE^2m

  return mul_a0_b0 + middle_part + last_part;
}



// prints LongNumber 
void print(LongNumber number) {
  digit temp;
  bool metSignificant = false;
  
  // go through all digits of the number
  for (int i = number.len - 1; i >= 0; --i) {
    
    if (!metSignificant) {  // if it's a heading digit of the number
      if (i == 0 || number.val[i] != 0) {
        cout << number.val[i];
        metSignificant = true;
      }

    } else { // if it's not a heading digit of the number
      cout << ",";

      // print heading zeros in the current digit, if any
      if (number.val[i] == 0) {
        temp = (BASE - 1) / 10;
      } else {
        temp = (BASE - 1) / number.val[i] / 10;
      }
      while (temp > 0) { 
        cout << "0";
        temp /= 10;
      }

      cout << number.val[i];
    }

  }
  cout << " (length " << number.len << ")\n";
}



// Returns a LongNumber where all digits are filled with random numbers not greater than BASE
// The max value of random numbers equals to the max return value of rand(), which is is 32767
LongNumber generateRandom(size_t length) {  
  LongNumber result(length);
  for (int i = 0; i < length; ++i) {
    result.val[i] = rand() % BASE;
  }
  return result;
}




int main(int argc, char** argv) {
  
  LongNumber number1(1000), number2(1000);

  srand(time(NULL)); // initialize random numbers generator
  number1 = generateRandom(number1.len);
  number2 = generateRandom(number2.len);

  cout << "BASE = " << BASE << "\n";
  cout << "Length 1 = " << number1.len << "; length 2 = " << number2.len << "\n";
  // print(number1);
  // cout << " * \n";
  // print(number2);
  // cout << " = \n";

  cout << "Method 1. Naive multiplication.\n";
  uint64_t start = get_cycles();
  naive_multiply(number1, number2);
  uint64_t naive_elapsed = get_cycles() - start;
  cout << "Elapsed time: " << naive_elapsed << "\n";

  cout << "Method 2. Karatsuba multiplication.\n";
  start = get_cycles();
  karatsuba_multiply(number1, number2);
  uint64_t karats_elapsed = get_cycles() - start;
  cout << "Elapsed time: " << karats_elapsed << "\n";

  if (naive_elapsed > karats_elapsed) {
    cout << "Karatsuba algorithm is " << (float) naive_elapsed / karats_elapsed << " times faster\n";
  } else {
    cout << "Naive algorithm is " << (float) karats_elapsed / naive_elapsed << " times faster\n";
  }

  return 0;
}