#include "ubm.h"

int main() {
	vector<string> name{"03a01Fa.txt", "03b01Fa.txt", "08a01Wa.txt", "08b01Fe.txt", "09a01Wb.txt",
						"09b03Wb.txt", "10b01Aa.txt", "11a05Fc.txt", "12a01Nb.txt", "13a05Nb.txt"};
//	vector<string> name{"03a01Fa.txt", "03a01Nc.txt", "03a01Wa.txt"};
	UBM ubm;
	ubm.recover_from_file("yinku/yinku_ubm100.txt");
	UBM gmm;
	for(unsigned int i=0; i<name.size(); i++) {
		gmm = ubm;
		string filename = "yinku/yinku_fea/"+name[i];
		gmm.read_personal_data(filename);
		gmm.Self_adaption();
		gmm.ScoreNormalization("yinku/yinku_fea/");
		gmm.save_to_file("yinku/models/"+name[i]);
	}
	return 0;
}
