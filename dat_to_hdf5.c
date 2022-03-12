#include <stdio.h>
#include <stdlib.h>
#include <hdf5.h>

int main(int argc, char *argv[]) {

	FILE *current, *output;
        hid_t file_id, dataset_id, dataspace_id;
        herr_t status;
	hsize_t dims[2];
	float t_val, i_val[100000000], v_val[100000000], sample_rate;
        char h5_filename[255];
	int i = 0;
        int pos = 0;

	if (argc < 4) {
		printf("Usage: dat_to_hdf5 [filename] [output file] [sample rate in kHz]\n");
		return 1;
	}
	sprintf(h5_filename, "%s.h5", argv[2]);
 	printf("Writing data to %s\n", h5_filename);

        current = fopen(argv[1], "rb");
        output  = fopen(argv[2], "w");

	// Convert sample rate for determining time.
	sample_rate = atof(argv[3]);
	sample_rate = sample_rate * 1000;
        sample_rate = 1.0/sample_rate;

        // Create the hdf5 file:
        file_id = H5Fcreate(h5_filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        if (!current) {
   		printf("Cannot open current file.\n");
                return 1;
        }

        while (1) {
		if (i % 2 == 0) {
        		fread(&i_val[0], sizeof(float), 1, current);
		} else {
        		fread(&v_val[0], sizeof(float), 1, current);
			pos++;
		}

		// Check for end of file:
		if (feof(current)){
			break;
		} else {
                     //   t_val = i*sample_rate;
 			//if (i % 2 == 1) {
			//	//printf("t: %f \t i: %f \t V: %f \n", t_val, i_val, v_val);
                          //      fprintf(output, "%f\t%f\t%f\n", t_val, i_val, v_val);
			//}
			i++;
		}
	}
	
	printf("Position: %d\n", pos);

	// Create the dataset. pyPore expects this to be called 'data':
	dims[0] = 1;
	dims[1] = pos;
	dataspace_id = H5Screate_simple(2, dims, NULL);
	
	// Write the necessary attributes:
	
       
        // Close files:
        status = H5Fclose(file_id);
	fclose(output);
        fclose(current);
	return 0;
}
