This package includes:

-Coordinator.c [This is the driver program that will create four memory segments to store results from each instance of the Checker.]
-Checker.c [Decide and display whether or not the second argument (divisor) is divisible by the first (dividend), store the result in shared memory]
-Checker.h [Header file declaring the function exposed from Checker]
-README.txt [This]

To compile:
    make all
    
To run:
    ./Coordinator <divisor> <dividend> <dividend> <dividend> <dividend>
    
Example;
    ./Coordinator 6 75 23 43 192
    


