#include "ubm.h"

int main(int argc, char **argv) {
	if(argc < 4) {
		cerr << "You should input four parameters:" << endl;
		cerr << "#1 the file contains features of voice;" << endl;
		cerr << "#2 the dimension of features;" << endl;
		cerr << "#3 the number of Gaussian Single Model in GMM;" << endl;
		cerr << "#4 the directory contains many others' features used for score normalization." << endl;	
		return 0;
	}
	UBM ubm;
	string filename(argv[1]);
	ubm.set_dim(atoi(argv[2]));
	ubm.set_num_gsm(atoi(argv[3]));
	
	if(ubm.read_data(filename)==0) {
		cerr << "Data file not exists!" << endl;
		return 0;
	}

	srand((int)time(0));
	ubm.Init();
	ubm.Kmeans();
	ubm.EM();
	ubm.ScoreNormalization(string(argv[4]));
	
	ubm.save_to_file("model_ubm.txt");
	
	return 0;
}
