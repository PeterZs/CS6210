#PBS -q cs6210
#PBS -l walltime=00:20:00
#PBS -l nodes=8:sixcore
#PBS -N Combined_Test

export PBS_O_WORKDIR=$HOME/final/Combined
EXE=$PBS_O_WORKDIR/MPI_OMP_Test
RESULT=$PBS_O_WORKDIR/Combined_result.txt

for round in 1 2 3 4 5 6 7 8 9 10
do
	echo "============= Round $round ================" >> $RESULT
	for mpi in 2 3 4 5 6 7 8
	do
		echo "========== mpi node number: $mpi ===========" >> $RESULT
		for omp in 2 3 4 5 6 7 8 9 10 11 12
		do 
			echo "======= openmp thread number: $omp ========" >> $RESULT
			OMPI_MCA_mpi_yield_when_idle=0 mpirun --hostfile $PBS_NODEFILE -np $mpi $EXE $omp >> $RESULT
			echo " " >> $RESULT
		done
	done
done
