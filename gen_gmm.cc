#include "ubm.h"

int main() {
	vector<string> name{"3244.txt", "3546.txt", "4073.txt", "4122.txt", "4987.txt",
						"4791.txt", "4799.txt", "5060.txt", "5536.txt", "6031.txt"};
	UBM ubm;
	ubm.recover_from_file("model_ubm.txt");
	UBM gmm;
	for(unsigned int i=0; i<name.size(); i++) {
		gmm = ubm;
		string filename = "features/"+name[i];
		gmm.read_personal_data(filename);
		gmm.self_adaption();
		gmm.save_to_file();
	}
	return 0;
}
