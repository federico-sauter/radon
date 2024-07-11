#include <stdio.h>
#include <stdlib.h>

/* Taken from:
 * https://gitlab.com/sortix/sortix/blob/master/kernel/x86-family/time.cpp
 */
unsigned short DivisorOfFrequency(long frequency)
{
  // The value we send to the PIT is the value to divide it's input clock
  // (1193180 Hz) by, to get our required frequency. Note that the divisor
  // must be small enough to fit into 16 bits.
  return 1193180 / frequency;
}

long FrequencyOfDivisor(unsigned short divisor)
{
  return 1193180 / divisor;
}

long RealFrequencyOfFrequency(long frequency)
{
  return FrequencyOfDivisor(DivisorOfFrequency(frequency));
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <frequency in Hz>\n\n", *argv);
    fprintf(
      stderr,
      "Outputs the divisor that should be passed to the PIT (Programmable \n"
      "Interval Timer) of the CPU for it to trigger in that time interval.\n");
    exit(2);
  }
  long frequency_in_hz = atoi(argv[1]);
  long tick_frequency = RealFrequencyOfFrequency(frequency_in_hz);
  unsigned short divisor = DivisorOfFrequency(tick_frequency);
  printf("%hu\n", divisor);
}
