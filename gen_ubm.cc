#include "ubm.h"

int main(int argc, char **argv) {
	if(argc < 3) {
		cerr << "You should input three parameters:" << endl;
		cerr << "#1 the file contains features of voice;" << endl;
		cerr << "#2 the dimension of features;" << endl;
		cerr << "#3 the number of Gaussian Single Model in GMM." << endl;
		return 0;
	}
	UBM ubm;
	string filename(argv[1]);
	ubm.set_dim(atoi(argv[2]));
	ubm.set_num_gsm(atoi(argv[3]));
	
	if(ubm.read_data(filename)==0) {
		cerr << "File not exists!" << endl;
		return 0;
	}
	srand((int)time(0));
	ubm.Init();
	ubm.Kmeans();
	ubm.EM();
	
	ubm.save_to_file();
	
	return 0;
}
