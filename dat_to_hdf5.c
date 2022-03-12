/******************************************************************************
 *
 * dat_to_hdf5.c - Takes a binary datafile (.dat) produced by the Elements ic
 *                 EDR3 application, and converts it into an hdf5 file that is
 *                 compatible with the pypore analysis package from the Drndic
 *                 Group at University of Pennsylvania.
 *
 * Author: Michael J. A. Hore <hore@case.edu>, Case Western Reserve University
 *
 * Date: 12 Mar 2022
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <hdf5.h>

#define MAX_ARR 100000000

int main(int argc, char* argv[]) {
	FILE *current, *output;
        hid_t file_id, dataset_id, dataspace_id, attribute_id;
        herr_t status;
	hsize_t dims;
	float t_val, i_val[MAX_ARR], v_val[MAX_ARR], sample_rate;
        char h5_filename[255];
	int i = 0;
        int pos = 0;

	if (argc < 4) {
		printf("Usage: dat_to_hdf5 [filename] [output file] [sample rate in kHz]\n");
		return 1;
	}

        current = fopen(argv[1], "rb");
        //output  = fopen(argv[2], "w");

        // Enforce the h5 file extension:
	sprintf(h5_filename, "%s.h5", argv[2]);
 	printf("Writing data to %s\n", h5_filename);

	// Convert sample rate for determining time.
	sample_rate = atof(argv[3]);
	sample_rate = sample_rate * 1000;

        // Create the hdf5 file:
        file_id = H5Fcreate(h5_filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        if (!current) {
   		printf("Cannot open current file.\n");
                return 1;
        }

        while (1) {
                // The Elements datafile is alternating floats of
                // current (nA) and voltage (mV).
		if (i % 2 == 0) {
        		fread(&i_val[pos], sizeof(float), 1, current);
		} else {
        		fread(&v_val[pos], sizeof(float), 1, current);
			pos++;
		}

		// Check for end of file:
		if (feof(current)){
			break;
		} 
		i++;
	}
	
	// Create the dataset. pyPore expects this to be called 'data':
	dims = pos;
	dataspace_id = H5Screate_simple(1, &dims, NULL);

        // Our current data is 32-bit little endian floats.
	dataset_id = H5Dcreate(file_id, "/data", H5T_NATIVE_FLOAT, dataspace_id, H5P_DEFAULT);
	status     = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, i_val);
        status     = H5Sclose(dataspace_id);

	// Write the necessary attributes for pyPore:
	dims         = 1;
	dataspace_id = H5Screate_simple(1, &dims, NULL);
        attribute_id = H5Acreate(dataset_id, "n_points", H5T_NATIVE_INT, dataspace_id, H5P_DEFAULT);
        status       = H5Awrite(attribute_id, H5T_NATIVE_INT, &pos);
	status       = H5Aclose(attribute_id);
	status       = H5Sclose(dataspace_id);
	
	dataspace_id = H5Screate_simple(1, &dims, NULL);
        attribute_id = H5Acreate(dataset_id, "sample_rate", H5T_NATIVE_INT, dataspace_id, H5P_DEFAULT);
        status       = H5Awrite(attribute_id, H5T_NATIVE_INT, &sample_rate);
	status       = H5Aclose(attribute_id);
	status       = H5Sclose(dataspace_id);

        // End access to the dataset.
        status = H5Dclose(dataset_id);

        // Close files:
        status = H5Fclose(file_id);
        fclose(current);
	return 0;
}
