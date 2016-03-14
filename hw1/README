# DIP Homework Assignment #1
# March 15, 2016
# Name: Steven Yu-Chih Lin
# ID #: R04922170
# email: r04922170@ntu.edu.tw
# compiled on Mac OSX El Capitan (10.11.3)

all: warm-up problem1 problem2

warm-up:
	@echo "----- WARM-UP -----"
	@echo "compiling for warm-up question..."
	g++ -w source/warm_up.cpp -o warm_up.out
	@echo "running the program, usage: ./warm_up.out <inputPath> <outputPath1> <outputPath2>"
	./warm_up.out input/sample1.raw output/sample1_hflip.raw output/sample1_vflip.raw

problem1:
	@echo "----- Problem 1 -----"
	@echo "compiling for problem 1..."
	g++ -w source/problem_1.cpp -o problem_1.out
	@echo "running the program, usage: ./problem_1.out <inputPath1> <inputPath2>"
	./problem_1.out input/sample1.raw input/sample2.raw
	mv p1_*.raw output/
	@echo "the resulted images are inside the output/ folder"

problem2:
	@echo "----- Problem 2 -----"
	@echo "compiling for problem 2..."
	g++ -w source/problem_2.cpp -o problem_2.out
	@echo "running the program, usage: ./problem_2.out <inputPath> > <outputFileName>"
	./problem_2.out input/sample1.raw > p2_PSNR_result.txt
	mv p2_*.raw p2_PSNR_result.txt output/
	@echo "the resulted images and text file are inside the output/ folder"

clean:
	rm -rf warm_up.out
	rm -rf problem_1.out
	rm -rf problem_2.out
	rm -rf output/*
