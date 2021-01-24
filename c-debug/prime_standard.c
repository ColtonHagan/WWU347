/* 
Name : Colton Hagan 
Date : 1/11/2021
Class : cs 347
Program : Will report a list of all primes which are less than
or equal to the user-supplied upper bound.
*/

/* prime-number finding program
Originally by Norman Matloff, UC Davis
http://wwwcsif.cs.ucdavis.edu/~davis/40/gdb_Tutorial.htm
Modified by Mark Ardis, RIT, 11/1/2006

Will report a list of all primes which are less than
or equal to the user-supplied upper bound.*/

#include <stdio.h>

int primes[15];  /* prime[i] will be 1 if i is prime, 0 otherwise */
int upper_bound; /* check all numbers up through this one for primeness */

/* Purpose : Checks to see if a given number is prime
Input parameter(s) : Takes in a number to check if it is prime
                        and an array containing numbers which are prime
                        (1 showing prime)
Output : Doesn't output instead modifies array */
void check_prime(int current_number, int prime[]) {
  int divisor;
  divisor = 2;

  /* Checks if the given number can be be divided by a prime number 
  (and that the number it is divided by is smaller then the 
  divisor square root as it would then have a smaller divisor) */
  while (divisor * divisor <= current_number) {
    if (primes[divisor] == 1){
      if (current_number % divisor == 0)  {
        primes[current_number] = 0;
        return;
      } 
    } 
    divisor++;
  } 

  // if we get here, then there were no divisors of current_number, so it is prime
  primes[current_number] = 1;

} 

int main() {
  int i;
  printf("Enter upper bound:\n");
  scanf("%d", &upper_bound);
  // this is a bug
  upper_bound = 50;
  // We know 1 and 2 are prime numbers
  primes[1] = 1;
  primes[2] = 1;
  // finds and prints all prime numbers below upper_bound
  for (i = 3; i <= upper_bound; i += 2) {
    check_prime(i, primes);
    if (primes[i]) {
      printf("%d is a prime\n", i);
    } 
  } 
  return 0;
} 
