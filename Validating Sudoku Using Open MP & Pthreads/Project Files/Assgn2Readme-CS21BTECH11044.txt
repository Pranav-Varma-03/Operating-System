In the zip,
Assgn2SrcOpenMp-CS21BTECH11044.cpp,Assgn2Srcpthread-CS21BTECH11044.cpp are 2 cpp files to use OpenMp and Pthreads respectively.
File Input.txt is read by above cpp files to produce Output Files.

i)the file input.txt contain:
	a)2 integers in 1st line (Num of Threads and Order of Sudoku respectively).
	b)futher lines contain the Sudoku Grid.
 Always, give input according to the above order.

ii)in Terminal, for compiling Cpp code Assgn2Srcpthread_CS21BTECH11044.cpp:
	line 1:g++ Assgn2Srcpthread-CS21BTECH11044.cpp -lpthread
	line 2:./a.out.
			
				(or)

		for compiling Cpp code Assgn2SrcOpenMp_CS21BTECH11044.cpp:
	line 1:g++ Assgn2SrcOpenMp-CS21BTECH11044.cpp -fopenmp -lpthread
	line 2:./a.out.

This takes input from "Input.txt" and generates "Output.txt" consists:

1) Log file:
	with thread number, row/col/grid validity

2) Overall Sudoku Grid Validity.
3) Time Taken


