all:
	gcc -o dat_to_hdf5 dat_to_hdf5.c -lhdf5

clean:
	rm *.o dat_to_hdf5
