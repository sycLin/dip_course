# DIP Homework Assignment #2
# April 12, 2016
# Name: Steven Yu-Chih Lin
# ID #: R04922170
# email: r04922170@ntu.edu.tw
# compiled on Mac OSX El Capitan (10.11.3)

all: p1 p2 p3

p1:
	@echo "----- Problem 1 -----"
	@echo "compiling..."
	g++ -O3 -w source/problem_1.cpp -o problem_1
	@echo "running the program..."
	@echo "(Usage: ./problem_1 <sample1> <sample2> <sample3>)"
	./problem_1 raw/sample1.raw raw/sample2.raw raw/sample3.raw
	@echo "moving the results into results/"
	@if ! [ -d "results" ]; then mkdir results; fi
	@mv sample*_2nd.raw sample*_canny.raw sample*_sobel.raw results/
	@echo "Problem 1 Finished."
p2:
	@echo "----- Problem 2 -----"
	@echo "compiling..."
	g++ -O3 -w source/problem_2.cpp -o problem_2
	@echo "running the program..."
	@echo "(Usage: ./problem_2 <sample4> <sample5> <sample6> <sample7>)"
	./problem_2 raw/sample4.raw raw/sample5.raw raw/sample6.raw raw/sample7.raw
	@echo "moving the results into results/"
	if ! [ -d "results" ]; then mkdir results; fi
	@mv p2_G.raw p2_R.raw p2_S.raw results/
	@echo "Problem 2 Finished."
p3:
	@echo "----- Problem 3 -----"
	@echo "compiling..."
	g++ -O3 -w source/problem_3.cpp -o problem_3
	@echo "running the program..."
	@echo "(Usage: ./problem_3 <sample8>)"
	./problem_3 raw/sample8.raw
	@echo "moving the results into results/"
	if ! [ -d "results" ]; then mkdir results; fi
	@mv p3_L.raw p3_C.raw results/
	@echo "Problem 3 Finished."

clean:
	@echo "cleaning up..."
	@echo "removing executables..."
	@rm -rf problem_1 problem_2 problem_3
	@echo "removing results/"
	@rm -rf results/
