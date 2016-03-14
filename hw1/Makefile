
warm-up:
	g++ source/warm_up.cpp -o warm_up.out
	./warm_up.out input/sample1.raw output/sample1_hflip.raw output/sample1_vflip.raw
	rm -f warm_up.out

problem1:
	g++ source/problem_1.cpp -o problem_1.out
	./problem_1.out input/sample1.raw input/sample2.raw
	mv p1_*.raw output/
	rm -rf problem_1.out

problem2:
	g++ source/problem_2.cpp -o problem_2.out
	./problem_2.out input/sample1.raw > p2_PSNR_result.txt
	mv p2_*.raw p2_PSNR_result.txt output/
	rm -rf problem_2.out

clean:
	rm -rf warm_up.out
	rm -rf problem_1.out
	rm -rf problem_2.out
	rm -rf output/*
