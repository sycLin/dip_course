all: p1 p2 p3

p1:
	g++ -O3 -w source/problem_1.cpp -o problem_1
p2:
	g++ -O3 -w source/problem_2.cpp -o problem_2
p3:
	g++ -O3 -w source/problem_3.cpp -o problem_3

run1:
	./problem_1 raw/sample1.raw raw/sample2.raw raw/sample3.raw
	if ! [ -d "results" ]; then mkdir results; fi
	mv sample*_2nd.raw sample*_canny.raw sample*_sobel.raw results/
run2:
	./problem_2 raw/sample4.raw raw/sample5.raw raw/sample6.raw raw/sample7.raw
	if ! [ -d "results" ]; then mkdir results; fi
	mv p2_G.raw p2_R.raw p2_S.raw results/
run3:
	./problem_3 raw/sample8.raw
	if ! [ -d "results" ]; then mkdir results; fi
	mv p3_L.raw results/

clean:
	rm -rf problem_1 problem_2 problem_3
	rm -rf results/
