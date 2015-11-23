build:
	for number in 1 2 3 5 6 8 9 ; do \
		gcc ./src/chol_$$number.c -o bin/chol_$$number -O0 -lpapi -lm -static ; \
	done

run:	
	for number in 1 2 3 5 6 8 9 ; do \
		for event in 0 1 2 3 4 ; do \
			for size in 500 750 1000 ; do \
				./bin/chol_$$number matrices/$$size.txt $$size $$event >> result/$$event-$$size.txt ; \
			done \
		done \
	done

