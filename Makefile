#
# Simpel makefile to build matrix
#

# Build AND RUN program
matrix: matrix.c matrix.h matrix_demo.c
	gcc matrix.c matrix_demo.c -o matrix
	@echo To run the program type: sudo ./matrix pattern1.txt
