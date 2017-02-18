#include <iostream>
// #include <ctime>


// Measures execution time
static inline uint64_t get_cycles() {
  uint64_t t;
  __asm__ __volatile__ ("rdtsc" : "=A"(t));
  return t;
}


// Raises x to power n using recursion (method 1):
long long power_recur(int x, int n) {
  if (n == 0) return 1;
  if (n % 2 == 0) {
    long long temp = power_recur(x, n/2);
    return temp * temp;
  } else {
    return power_recur(x, n - 1) * x;
  }
}

// Raises x to power n using a for cycle from 1 to n (method 2):
long long power_cycle(int x, int n) {
  if (n == 0) return 1;
  long long result = 1;
  for (int i = 0; i < n; ++i) {
    result = result * x;
  }
  return result;
}



// Tests method 1 or 2 and measures time spent on execution
// n_iter - number of iterations
// x - base, n - exponent
// if recursion is true, uses power_recur function
// if recursion is false, uses power_cycle function
// Returns time spent on all iterations
long long test(int n_iter, int x, int n, bool recursion) {
  long long temp;
  
  // clock_t start = clock();
  uint64_t start = get_cycles();
  
  for (int i = 0; i < n_iter; ++i) {
    // std::cout << "Test #" << i << ", result = " << power(x, n) << ", elapsed = " << get_cycles() - start << "\n";
    if (recursion) {
      temp = power_recur(x, n);
    } else {
      temp = power_cycle(x, n);
    }
  }

  uint64_t elapsed = get_cycles() - start;
  // clock_t elapsed = clock() - start;

  std::cout << "X ^ N = " << temp << "\n";
  return elapsed;
}



int main(int argc, char** argv) {

  int x, n, n_iter;
  std::cout << "Enter x (base) and n (exponent): ";
  std::cin >> x >> n;

  std::cout << "Enter number of iterations: ";
  std::cin >> n_iter;

  long long cycle_elapsed, recur_elapsed;
  cycle_elapsed = test(n_iter, x, n, false);
  recur_elapsed = test(n_iter, x, n, true);
  std::cout << "Method 1 - recursion. Elapsed time: " << recur_elapsed << "\n";
  std::cout << "Method 2 - cycled. Elapsed time: " << cycle_elapsed << "\n";
  if (cycle_elapsed > recur_elapsed) {
    std::cout << "Recurcion method is " << (float) cycle_elapsed / recur_elapsed << " times faster\n";
  } else {
    std::cout << "Cycled method is " << (float) recur_elapsed / cycle_elapsed<< " times faster\n";
  }
 
  return 0;
}