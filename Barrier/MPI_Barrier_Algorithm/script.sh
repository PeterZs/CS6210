#PBS -q cs6210
#PBS -l walltime=00:20:00
#PBS -l nodes=jinx2+jinx3+jinx4+jinx5+jinx6+jinx7+jinx8+jinx9+jinx10+jinx11+jinx12+jinx13
#PBS -N MPI_Test

etxport PBS_O_WORKDIR=$HOME/final/MPI_Barrier_Algorithm

for algo in test_tournament test_dissemination
do
	EXE=$PBS_O_WORKDIR/$algo
	RESULT=$PBS_O_WORKDIR/mpi_barrier_result.txt
	echo "============== MPI $algo Test =============" >> $RESULT
	for ((num = 1; num < 11; ++num));
	do
		echo "========== Round $num test ==============" >> $RESULT
		for node in 2 3 4 5 6 7 8 9 10 11 12
		do
			echo "result for $node nodes" >> $RESULT
			OMPI_MCA_mpi_yield_when_idle=0 mpirun --hostfile $PBS_NODEFILE -np $node $EXE >> $RESULT
			echo " " >> $RESULT
		done
	done
done


