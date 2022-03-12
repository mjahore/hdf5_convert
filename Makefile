all:
	gcc -O3 -DH5_USE_16_API -mcmodel=large -o dat_to_hdf5 dat_to_hdf5.c -lhdf5

debug:
	gcc -g -DH5_USE_16_API -mcmodel=large -o dat_to_hdf5 dat_to_hdf5.c -lhdf5

clean:
	rm dat_to_hdf5
