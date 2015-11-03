#include "ubm.h"

int main() {
	UBM ubm;
	ubm.recover_from_file("yinku/yinku_ubm100.txt");
	
	vector<string> name{"03a01Fa.txt", "03b01Fa.txt", "08a01Wa.txt", "08b01Fe.txt", "09a01Wb.txt",
						"09b03Wb.txt", "10b01Aa.txt", "11a05Fc.txt", "12a01Nb.txt", "13a05Nb.txt"};
//	vector<string> name{"03a01Fa.txt", "03a01Nc.txt", "03a01Wa.txt"};
	vector<UBM> models(10);
	for(int i=0; i<10; i++) {
		string filename = "yinku/models/"+name[i];
		string datadir = "yinku/yinku_fea/"+name[i];
		models[i].recover_from_file(filename);
		models[i].read_personal_data(datadir);
	}

	vector<vector<double>> ans(10, vector<double>(10));
	vector<vector<double>> features;

	for(int i=0; i<10; i++) {
		features = models[i].get_data();
		double t = ubm.get_point(features);
		for(int j=0; j<10; j++) {
			double s = models[j].get_point(features);
			ans[i][j] = s - t;
		}
	}

	for(auto &&i : ans) {
		for(auto j : i) {
			cout << j << " ";
		}
		cout << endl;
	}
}
