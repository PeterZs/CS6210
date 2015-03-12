
echo "======== OpenMP Centrial standalone test ========" >> OpenMP_Centrial_result.txt
for((num = 1; num < 21; ++num));
do
	echo "========== Round $num test ==========" >> OpenMP_Centrial_result.txt
	for n in 2 3 4 5 6 7 8
	do
		echo "result for $n threads" >> OpenMP_Centrial_result.txt
		./test $n >> OpenMP_Centrial_result.txt
		echo " " >> OpenMP_Centrial_result.txt
	done
done
