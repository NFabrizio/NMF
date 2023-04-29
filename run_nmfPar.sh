#!/bin/bash

ARGS=("$@")
length=${#ARGS[@]}
PROGRAM1=${ARGS[0]}
PROGRAM2=${ARGS[1]}
# T=5
RESULT_FILE="results_$PROGRAM2.txt"

# Clean up workspace by removing file from previous run
rm -f $RESULT_FILE

# Set up file headers
echo "program,data_set,n,i,t,num_threads,iter_0,iter_1,iter_2,iter_3,iter_4,iter_5,iter_6" >> $RESULT_FILE

echo "Running $PROGRAM1"

for (( n=2 ; n<length ; n+=4 )); do
  data_set=${ARGS[n]}
  N=${ARGS[n+1]}
  I=${ARGS[n+2]}
  T=${ARGS[n+3]}
  B=1

    # Reset ITER for next round of iterations
  # for p in {2..6}; do
  printf "$PROGRAM1,$data_set,$N,$I,$T,1" >> $RESULT_FILE

  ITER1=0
  if [ $ITER1 == 0 ]; then
    while [ $ITER1 -lt 7 ]; do
      # echo "ITER=$ITER"

      echo "ITER: $ITER1 for $PROGRAM1 for data_set=$data_set, n=$N, i=$I, t=$T"
      result1=$(eval "./$PROGRAM1 -N $N -I $I -T $T -B $B $data_set")

      # echo $result1
      # echo $result1 | rev | cut -d " " -f8 | rev
      # printf $result1 | rev | cut -d " " -f8 | rev
      new_result1=($result1)
      new_result1_length=${#new_result1[@]}-8
      # echo ${new_result1[$new_result1_length]}
      printf ",${new_result1[$new_result1_length]}" >> $RESULT_FILE

      ((ITER1++))
    done

    # Echo empty char to add line break between log sets
    echo '' >> $RESULT_FILE
  fi
  # done
# done

  echo "Running $PROGRAM2"

# printf "$PROGRAM2,${ARGS[n]}" >> $RESULT_FILE

# Reset ITER for next round of iterations
# ITER=0

# while [ $ITER -lt 7 ]; do
  # echo "ITER=$ITER"

  # echo "ITER: $ITER for $PROGRAM2 parallel for n_limit=${ARGS[n]}, blk_size=${ARGS[n+1]}"

  # printf "$PROGRAM1,${ARGS[n]},${ARGS[n+1]},1" >> $RESULT_FILE


# for (( n=2 ; n<length ; n+=4 )); do
#   data_set=${ARGS[n]}
#   N=${ARGS[n+1]}
#   I=${ARGS[n+2]}
#   T=${ARGS[n+3]}
#   B=1


  for i in {1..8}; do
    ITER=0
    # echo "num_threads=$i"

    # Set new number of threads
    export OMP_NUM_THREADS=$i

    printf "$PROGRAM2,$data_set,$N,$I,$T,$i" >> $RESULT_FILE
    # printf "$PROGRAM2,$DEGREE,$BLK_SIZE,$LEAF_SIZE,1" >> $RESULT_FILE

    # Run 7 times with this many threads
    while [ $ITER -lt 7 ]; do
      # echo "ITER=$ITER"
      # echo "ITER: $ITER for $PROGRAM2 parallel for n_limit=${ARGS[n]}, blk_size=${ARGS[n+1]}, threads=$i"
      echo "ITER: $ITER for $PROGRAM2 for data_set=$data_set, n=$N, i=$I, t=$T, num_threads=$i"
      # echo "ITER: $ITER for $PROGRAM2 parallel for degree=$DEGREE, blk_size=$BLK_SIZE, leaf_size=$LEAF_SIZE"

      # echo "mat_vec"
      # result=$(mat_vec 25000 10000)
      result2=$(eval "./$PROGRAM2 -N $N -I $I -T $T -B $B $data_set")

      # new_result=($result)
      new_result2=($result2)
      new_result2_length=${#new_result2[@]}-8
      # echo ${new_result1[$new_result1_length]}
      printf ",${new_result2[$new_result2_length]}" >> $RESULT_FILE

      ((ITER++))
    done

    # Echo empty char to add line break between log sets
    echo '' >> $RESULT_FILE
  done
done
