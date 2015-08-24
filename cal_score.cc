#include "ubm.h"

int main() {
	UBM ubm;
	ubm.recover_from_file("model_ubm.txt");
	
	vector<string> name{"3244.txt", "3546.txt", "4073.txt", "4122.txt", "4987.txt",
						"4791.txt", "4799.txt", "5060.txt", "5536.txt", "6031.txt"};
	
	vector<UBM> models(10);
	for(int i=0; i<10; i++) {
		string filename = "models/"+name[i];
		string datadir = "features/"+name[i];
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
