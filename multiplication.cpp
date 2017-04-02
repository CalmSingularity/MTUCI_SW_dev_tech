#define _CRT_RAND_S // rand_s
#include <stdlib.h> // rand_s
#include <iostream> // cout
#include <ctime>    // clock
#include <cstdlib>
using namespace std;

#define BASE UINT_MAX    // base of the numerical notation. UINT_MAX == 4,294,967,295
#define MIN_LENGTH_FOR_KARATSUBA 4    // if the number is shorter, it won't be multiplied using Karatsuba method
#define N_TESTS 3
#define NUMBERS_LENGTH 10000
typedef unsigned int digit;    // type of one digit in the chosen numerical notation to accomodate tha max value of BASE
typedef unsigned long long int double_digit;    // type to accomodate tha max value of BASE*BASE



/* Struct to store long numbers in the chosen numerical notation */
struct LongNumber {
  digit* val; // array to store the "digits" where [0] element stores the right-most (low-order) digit of the number
  size_t len; // number of "digits" (size of the array)

  // Constructors
  LongNumber() {
    len = 0;
    val = nullptr;
  }


  LongNumber(size_t length) {
    len = length;
    val = new digit[len];
    memset(val, 0, sizeof(val) * len); // fill number with zeros
  }

  // Copy-constructor
  LongNumber(const LongNumber &obj) {
    this->len = obj.len;
    this->val = new digit[len];
    memcpy(this->val, obj.val, sizeof(val) * len);
  }

	// Move-constructor
	LongNumber(LongNumber &&obj) {
		this->len = obj.len;
		this->val = obj.val;
		obj.len = 0;
		obj.val = nullptr;
	}

  LongNumber& operator = (LongNumber& obj) {
    if (val) delete[] val;
    len = obj.len;
    val = new digit[len];
    memcpy(val, obj.val, sizeof(val) * len);
    return *this;
  }

  ~LongNumber() {
    if (val) {
      delete[] val;
      val = nullptr;
    }
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
      digit* old_value = val;
      val = temp;
      delete[] old_value;
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
  // For example, if BASE == 10 && x.val == {3,2,1}, then x.shift(3) makes x.val == {0,0,0,3,2,1}
  void shift(size_t n) {
    digit* temp = new digit[len + n];
    memset(temp, 0, n * sizeof(val));
    memcpy(temp + n, val, len * sizeof(val));
    digit* old_value = val;
    val = temp;
    delete[] old_value;
    len += n;
  }


  // Operator + to sum two long numbers
  // Returns (this + A)
  LongNumber operator + (LongNumber A) {
    // Bring two long number (this and A) to the same length
    if (len > A.len) {
      A.increaseDigits(len);
    }
    else if (len < A.len) {
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
    }
    else if (len < A.len) {
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
      }
      else {
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
    }
    else {
      return (karatsuba_multiply(*this, A));
    }
  }

};



/* Naive multiplication of two long numbers */
LongNumber naive_multiply(LongNumber x, LongNumber y) {

  LongNumber interim(x.len + y.len), result(x.len + y.len);
  digit carryover;
  double_digit temp;

  for (size_t i = 0; i < y.len; ++i) {
    carryover = 0;
    memset(interim.val, 0, sizeof(interim.val)*interim.len); // initialize interim as 0

    // multiply y[i] and x[0...x.len] (one digit of y to all digits of x) 
    // and save the result in interim
    for (size_t j = 0; j < x.len; ++j) {
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
  }
  else if (a.len < b.len) {
    a.increaseDigits(b.len);
  }

  // Make length of a and b even
  if (a.len % 2 == 1) {
    a.increaseDigits(a.len + 1);
    b.increaseDigits(b.len + 1);
  }

  // Split a and b both in two parts: a0 and a1, b0 and b1 and copy values
  size_t m = a.len / 2;
  LongNumber a0(m), a1(m), b0(m), b1(m);
  memcpy(a0.val, a.val, sizeof(a.val) * m);
  memcpy(a1.val, a.val + m, sizeof(a.val) * m);
  memcpy(b0.val, b.val, sizeof(b.val) * m);
  memcpy(b1.val, b.val + m, sizeof(b.val) * m);

  LongNumber mul_a0_b0 = a0 * b0;
  LongNumber mul_a1_b1 = a1 * b1;

  LongNumber middle_part = (a0 + a1) * (b0 + b1) - mul_a0_b0 - mul_a1_b1;
  middle_part.shift(m);  // = middle_part * BASE^m

  LongNumber last_part = mul_a1_b1;
  last_part.shift(2 * m);  // = last_part * BASE^2m

  LongNumber result = mul_a0_b0 + middle_part + last_part;
  return result;
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

    }
    else { // if it's not a heading digit of the number
      cout << ",";

      // print heading zeros in the current digit, if any
      if (number.val[i] == 0) {
        temp = (BASE - 1) / 10;
      }
      else {
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
LongNumber generateRandom(size_t length) {
  LongNumber result(length);
  digit random_number;
  for (size_t i = 0; i < length; ++i) {
    rand_s(&random_number);
    result.val[i] = random_number % BASE;
  }
  return result;
}




int main(int argc, char** argv) {

  LongNumber numberA(NUMBERS_LENGTH), numberB(NUMBERS_LENGTH);

  numberA = generateRandom(numberA.len);
  numberB = generateRandom(numberB.len);

  cout << "BASE = " << BASE << "\n";
  cout << "N_TESTS = " << N_TESTS << "\n";

  cout << "Length A = " << numberA.len << "; length B = " << numberB.len << "\n";
  //print(numberA);
  //cout << " * \n";
  //print(numberB);
  //cout << " = \n";

  LongNumber temp;
  cout << "Method 1. Naive multiplication.\n";
  clock_t start = clock();
  for (size_t i = 0; i < N_TESTS; ++i)
    naive_multiply(numberA, numberB);
  double naive_elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
  cout << "Elapsed time: " << naive_elapsed << "\n";

  cout << "Method 2. Karatsuba multiplication.\n";
  start = clock();
  for (size_t i = 0; i < N_TESTS; ++i)
    karatsuba_multiply(numberA, numberB);
  double karats_elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
  cout << "Elapsed time: " << karats_elapsed << "\n";

  if (naive_elapsed > karats_elapsed) {
    cout << "Karatsuba algorithm is " << (float)naive_elapsed / karats_elapsed << " times faster\n";
  }
  else {
    cout << "Naive algorithm is " << (float)karats_elapsed / naive_elapsed << " times faster\n";
  }

  return 0;
}