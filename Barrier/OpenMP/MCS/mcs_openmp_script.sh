
echo "======== OpenMP MCS standalone test ========" >> Temp_OpenMP_MCS_result.txt
for((num = 1; num < 21; ++num));
do
	echo "========== Round $num test ==========" >> Temp_OpenMP_MCS_result.txt
	for n in 2 3 4 5 6 7 8
	do
		echo "result for $n threads" >> Temp_OpenMP_MCS_result.txt
		./test $n  >> Temp_OpenMP_MCS_result.txt
		echo " " >> Temp_OpenMP_MCS_result.txt
	done
done
