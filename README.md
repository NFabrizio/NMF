# NMF  

This is a C implementation of Nonnegative Matrix Factorization using Itakura-Saito (IS) divergence criterion.
Multiplicative update rules are used for minimizing IS divergence.  

Input file: tab-delimited text with row headers and column headers.  

The Mersenne twister are used for a pseudorandom number generator. C codes are taken from http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c.  

This repository was originally forked from https://github.com/khigashi1987/NMF.  

## Installation and Set Up  

Below are the instructions for installing this application.  
*These instructions are valid as of 2023.04.03*  

### Environment Set Up
1. On your local machine, navigate to the directory to which this repository has been cloned, and run the following command.  
   `make`  

### Application Use  
1. To run the application using the test data, run the following command.  
   `./nmf -N 4 -I 500 -B 50 -T 1 ./test_data/iris_test_data.data`  
   This will create two files, one for W and one H named W.dat and H.dat respectively.  

2. To run test.sh script, run the following command.  
   `./test.sh 4 500 ./test_data/iris_test_data.data 1 0 50 nmf`  
   First Param is N, second is Iterations, third is data file name, fourth is threshold, fith is 0=no threading, 1=threading, 6th block size and last is algorithm
   This will generate or add to a test_output.txt file with results for 7 runs of each of 1-8 threads (if threading is activated)
   Default values are 2, 50, iris_test_data.data and 0 if none are passed in. We have also created a test_all script that will loop through all algorithms and
   generate results for all sequential and threaded solutions.

3. 3 data files have been generated and stored in test_data folder. Below are the command lines to run each test and the expected output with the seed   
   LARGE DATA FILE RUN  
   `./nmf -N 10 -I 10000 -T 15 -B 250 ./test_data/large_data.dat`  
   ```
   Number of words      10000  
   Number of samples    10  
   Number of classes    10  

   IS Divergence = 6267531.91859795  

   IS Divergence = 59.33167726  

   IS Divergence = 40.80568387...  

   IS Divergence = 1.68638146  

   IS Divergence = 1.56581928  
   elapsed time = 75.944988 (sec)  
   WH and originalData are the same
   ```  

   MED DATA FILE RUN  
   `./nmf -N 6 -I 10000 -T 5 -B 500 ./test_data/med_data.dat`  
   ```
   Number of words      5000  
   Number of samples    6  
   Number of classes    6  

   IS Divergence = 1805581.09459682  

   IS Divergence = 15.76550055  

   IS Divergence = 13.15114621  

   IS Divergence = 10.04007297  

   IS Divergence = 5.13743033...  

   IS Divergence = 0.21918876  

   IS Divergence = 0.19042569  

   IS Divergence = 0.16445100  
   elapsed time = 13.260365 (sec)  
   WH and originalData are the same  
   ```


   SMALL DATA FILE RUN  
   `./nmf -N 3 -I 10000 -T 1 -B 50 ./test_data/small_data.dat`  
   ```
   Number of words      1000  
   Number of samples    3  
   Number of classes    3  

   IS Divergence = 186297.79663714  

   IS Divergence = 1.05189921  

   IS Divergence = 0.04644241  

   IS Divergence = 0.01716420  

   IS Divergence = 0.00804131...  

   IS Divergence = 0.00000004  

   IS Divergence = 0.00000002  

   IS Divergence = 0.00000001  

   IS Divergence = 0.00000000  

   IS Divergence = 0.00000000  
   converged.  
   elapsed time = 0.670529 (sec)  
   WH and originalData are the same  
   ```

4. Other scripts include generateData.sh which was run once for each of the data sizes. We re-used the same datasets for each algorithm, so you only need to execute this if you'd like to generate a dataset of a different size. First param is number of rows, second is number of features, and 3rd is the file name the dataset will be written to once generated. `./generateData.sh 10000 50 xtra_large_data.dat`

5. To execute the python version of NMF for each dataset. This was used to help measure our speedup vs an already tuned library.
   `python3 pythonNMF.py test_data/small_data.dat 10000 1 3`
   `python3 pythonNMF.py test_data/med_data.dat 10000 5 6`
   `python3 pythonNMF.py test_data/large_data.dat 10000 15 10`