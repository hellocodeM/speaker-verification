#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cfloat>
#include <string>
#define random(x) (rand()%x)
#define INF 1e+32
using namespace std;

//const double perturb = 0.001;
struct GSM {
	vector<double> mean;
	vector<double> var;
	vector<int> sample;
	double weight;
	int tol_num;
};

class UBM {
private:
	int dim;
	int num_GSM;
	//	static int dim;				//dimension of features
//	static int num_GSM;    		//the number of GSM
	static const int num_data = 2000000;
	static const int max_iter = 1000;       //the maximun number of iterations of K-means
	static const double threshold;   		//the threshold of K-means
	static const double threshold_EM;		//the threshold of EM

	string model_id;
	vector<GSM> gsm;
	vector<vector<double>> data;
	vector<vector<double>> pr;

	vector<vector<double>> personal_data;
	vector<vector<double>> personal_pr;
	vector<double> n;
	vector<vector<double>> Ex;

	void recluster();
//	int check_badcell();
//	void adjust_badmean();
	double update_mean();
	void update_var();
	void update_weight();
	double distance(int, int);
	double distance_p(int, int);
	int find_closest(int);
	double cal_gsm(int, vector<double>&);
	void E_step();
	double M_step();
	

public:
	UBM() {
		dim = 12;
		num_GSM = 100;
		model_id = "model_0";
	};
	int read_data(string);
	void init();
	void Init();
	void Kmeans();
	void EM();
	void set_dim(int);
    void set_num_gsm(int);
	int get_dim();
	int get_num_gsm();

	int read_personal_data(string);
	void self_adaption();

	double get_point(vector<vector<double>>&);
	vector<vector<double>> get_data() {
		return personal_data;	
	} 

	void set_model_id(string);
	void save_to_file();
	int recover_from_file(string);
	
	void show_weight();
	void show_mean();
	void show_var();
	void show_tol_num();
};

//int UBM::num_GSM = 100;
//int UBM::dim = 12;
const double UBM::threshold = 0.0001;
const double UBM::threshold_EM = 0.0005;

//============================ general function ============================//
int UBM::recover_from_file(string filename) {
	ifstream in(filename);
	if(!in) {
		cerr << "File not exists!" << endl;
		return 0;
	}
	int temp;
	in >> temp;
	dim = temp;

	in >> temp;
	num_GSM = temp;

	gsm.resize(num_GSM);
	for(int i=0; i<num_GSM; i++) {
		in >> gsm[i].weight;
		gsm[i].mean.resize(dim);
		gsm[i].var.resize(dim);
	}

	for(int i=0; i<num_GSM; i++) {
		for(int j=0; j<dim; j++) {
			in >> gsm[i].mean[j];
		}
	}

	for(int i=0; i<num_GSM; i++) {
		for(int j=0; j<dim; j++) {
			in >> gsm[i].var[j];
		}
	}
	in.close();
	cout << "Recovery is success!" << endl;
	return 1;
}

void UBM::save_to_file() {
	ofstream out(model_id);
	out << dim << " " << num_GSM << '\n';	//save dim and num_GSM in the 1st line
	
	for(int i=0; i<num_GSM; i++)			//save weight
		out << gsm[i].weight << '\n';

	for(int i=0; i<num_GSM; i++) {			//save mean
		for(int j=0; j<dim; j++) {
			out << gsm[i].mean[j] << " ";
		}
		out << '\n';
	}
	
	for(int i=0; i<num_GSM; i++) {			//save var
		for(int j=0; j<dim; j++) {
			out << gsm[i].var[j] << " ";
		}
		out << '\n';
	}
	out << endl;
	out.close();
}

void UBM::set_model_id(string x) {
	model_id = x;
}

void UBM::set_dim(int x) {
	dim = x;
}

void UBM::set_num_gsm(int x) {
	num_GSM = x;
}

int UBM::get_dim() {
	return dim;
}

int UBM::get_num_gsm() {
	return num_GSM;
}

void UBM::show_weight() {
	cout << "The weights are below:" << endl;	
	for(int i=0; i<num_GSM; i++) {
		cout << "GSM id=" << i << ": " << gsm[i].weight << endl;
	}
	return;
}

void UBM::show_mean() {
	cout << "The mean vector are below:" << endl;
	for(int i=0; i<num_GSM; i++) {
		cout << "GSM id=" << i << ":";
		for(int j=0; j<dim; j++) {
			cout << " " << gsm[i].mean[j];
		}
		cout << endl;
	}
	return;
}
	
void UBM::show_var() {
	cout << "The varience vector are below:" << endl;
	for(int i=0; i<num_GSM; i++) {
		cout << "GSM id=" << i << ":";
		for(int j=0; j<dim; j++) {
			cout << " " << gsm[i].var[j];
		}
		cout << endl;
	}
	return;
}

void UBM::show_tol_num() {
	cout << "The number of each clusters' samples are below:" << endl;
	for(int i=0; i<num_GSM; i++) {
		cout << "GSM id=" << i << " total_number=" << gsm[i].tol_num << endl;
	}
	return;
}
//========================= end general function ============================//

//================================== read data =========================================//
int UBM::read_data(string filename) {
	model_id = "model_"+filename;
	ifstream in(filename);
	data.reserve(num_data);	
	if(!in) {
		cerr << "Data file not exist!" << endl;
		return 0;
	}

	vector<double> temp;
	double t;
	while(in >> t) {
		temp.push_back(t);
		for(int i=1; i<dim; i++) {
			in >> t;
			temp.push_back(t);
		}
		if(temp[0] != 0)
			data.push_back(temp);
		temp.clear();
	}
	cout << "Read Data Completed!" << endl;
	in.close();
	return 1;
}
//=============================== end read data =======================================//

//=============================== init ==================================//
double UBM::distance_p(int data_id, int data_id2) {
// calulate distance between data[data_id] and data[data_id2]
	double dist = 0;
	for(int i=0; i<dim; i++) {
		dist += (data[data_id2][i]-data[data_id][i])*(data[data_id2][i]-data[data_id][i]);
	}
	return sqrt(dist);
}

// use k-means++ to initialize
void UBM::Init() {
	gsm.resize(num_GSM);
	for(int i=0; i<num_GSM; i++) {
		gsm[i].mean.resize(dim);
		gsm[i].var.resize(dim);
	}
	cout << "Initialize K-means......" << endl;
	
	vector<double> dist(data.size());
	vector<double> dist2(data.size());
	vector<double> dist2_c(data.size());
	double sum0 = 0.0;
	
	int index = random(data.size());	// choose the 1st center
	for(int i=0; i<dim; i++) {
		gsm[0].mean[i] = data[index][i];
	}

	for(unsigned int i=0; i<data.size(); i++) {	// cal distance and save it
		dist[i] = distance_p(i,index);
		sum0 += dist[i];
	}
	
	for(int k=1; k<num_GSM; k++) {
		double maxSum = DBL_MAX;
		int new_center_id = -1;
		for(int j=0; j<3; j++) {			// 3 is decided by us
			double thres = (double)random((int)sum0);
			double s = 0.0;
			unsigned int i;
			for(i=0; i<data.size(); i++) {
				thres -= dist[i];
				if(thres <= 0) break;
			}
			int new_id = i;

			for(i=0; i<data.size(); i++) {
				dist2_c[i] = min(distance_p(i,new_id), dist[i]);
				s += dist2_c[i];
			}

			if(s < maxSum) {
				maxSum = s;	
				new_center_id = new_id;
				swap(dist2_c, dist2);
			}
		}
		for(int j=0; j<dim; j++) {
			gsm[k].mean[j] = data[new_center_id][j];
		}
		sum0 = maxSum;
		swap(dist, dist2);
	}
}

//============================== end init ===============================//


//============================== do cluster =============================//
double UBM::distance(int data_id, int gsm_id) {
// calulate distance between data[data_id] and gsm[gsm_id]
	double dist = 0;
	for(int i=0; i<dim; i++) {
		dist += (gsm[gsm_id].mean[i]-data[data_id][i])*(gsm[gsm_id].mean[i]-data[data_id][i]);
	}
	return dist;
}

int UBM::find_closest(int data_id) {
// find which gsm is closest to data[data_id]
	double min =  INF;
	int min_id = -1;
	for(int i=0; i<num_GSM; i++) {
		double d = distance(data_id, i);
		if(d < min) {
			min = d;
			min_id = i;
		}
	}
	return min_id;
}

void UBM::recluster() {
// recluster according new mean vector	
	for(int i=0; i<num_GSM; i++) {
		gsm[i].sample.clear();
		gsm[i].tol_num = 0;
	}
	for(unsigned int i=0; i<data.size(); i++) {
		int t = find_closest(i);
		gsm[t].sample.push_back(i);
		gsm[t].tol_num++;
	}
//	show_tol_num();
	return;
}

double UBM::update_mean() {
// after once cluster, update the mean vector
// this function also return the change of mean vector
	double change = 0;
	for(int i=0; i<num_GSM; i++) {
		vector<double> new_mean(dim);
		for(int j=0; j<gsm[i].tol_num; j++) {
			int id = gsm[i].sample[j];
			for(int k=0; k<dim; k++) {
				new_mean[k] += data[id][k]/gsm[i].tol_num;
			}
		}
		double t = 0;
		for(int k=0; k<dim; k++) {
			t += (new_mean[k]-gsm[i].mean[k])*(new_mean[k]-gsm[i].mean[k]);
			gsm[i].mean[k] = new_mean[k];
		}
		change += t;
	}
//	show_mean();	

	return change;
}

void UBM::update_var() {
// after once cluster, update the var vector
	for(int i=0; i<num_GSM; i++) {
		for(int k=0; k<dim; k++) gsm[i].var[k] = 0;
		for(int j=0; j<gsm[i].tol_num; j++) {
			int id = gsm[i].sample[j];
			for(int k=0; k<dim; k++)
				gsm[i].var[k] += 1.0f/gsm[i].tol_num*(data[id][k]-gsm[i].mean[k])*(data[id][k]-gsm[i].mean[k]);
		}
	}
//	show_var();	
	return;
}

void UBM::update_weight() {
// after once cluster, update the weight of each cluster
	for(int i=0; i<num_GSM; i++) {
		gsm[i].weight = 1.0*gsm[i].tol_num/data.size();
	}
//	show_weight();
	return;
}

void UBM::Kmeans() {
	int i = 1;
	double change = INF;
	while(i<=max_iter && change>=threshold) {
		cout << "K-means Iteration " << i << ":" << endl;
		recluster();
		change = update_mean();
		i++;
	}
	update_var();
	update_weight();
	cout << "mean, var, weight have been updated!" << endl;
	show_weight();
	show_var();
	show_mean();
	show_tol_num();
	return;
}
//============================ end do cluster ==========================//

//============================== build UBM =============================//
double UBM::cal_gsm(int gsm_id, vector<double> &data) {
// calculate the gaussian for one single gsm	
	double sum = 0, output;
	double product = 1.0;
	double two_pi = 8.0*atan(1.0);
	for(int k=0; k<dim; k++) {
		sum += (data[k]-gsm[gsm_id].mean[k])*(data[k]-gsm[gsm_id].mean[k])/gsm[gsm_id].var[k];
		product *= sqrt(gsm[gsm_id].var[k]);
	}
	sum = exp(-0.5*sum);
	product = 1.0/product;
	output = 1.0/pow(two_pi,(double)dim/2.0)*product*sum;
	return output;
}

void UBM::E_step() {
// calculate pr[gsm_id | data_id]	
	double denominator;
	for(unsigned int i=0; i<data.size(); i++) {
		denominator = 0.0;
		for(int j=0; j<num_GSM; j++) {
			pr[j][i] = cal_gsm(j,data[i])*gsm[j].weight;	
			denominator += pr[j][i];
		}
		for(int j=0; j<num_GSM; j++) {
			pr[j][i] /= denominator;
		}
	}
	return;
}

double UBM::M_step() {
// update mean, varience, weight
	double new_mean[num_GSM], new_var[dim];
	double new_weight;
	double change_weight=0.0, change_mean=0.0, change_var=0.0;
	double sum1, sum2, sum3;

	for(int i=0; i<num_GSM; i++) {
		sum1=0.0;
		for(unsigned int j=0; j<data.size(); j++) 
			sum1 += pr[i][j];
		
		for(int j=0; j<dim; j++) {
			sum2 = 0.0;
			sum3 = 0.0;
			for(unsigned int k=0; k<data.size(); k++) 
				sum2 += data[k][j]*pr[i][k];
			new_mean[j] = sum2/sum1;		
			change_mean += fabs(new_mean[j]-gsm[i].mean[j]); //record mean's change
			gsm[i].mean[j] = new_mean[j];					 //update mean

			for(unsigned int k=0; k<data.size(); k++) {
				sum3 += pr[i][k]*(data[k][j]-new_mean[j])*(data[k][j]-new_mean[j]);
			}
			new_var[j] = sum3/sum1;
			change_var += fabs(new_var[j]-gsm[i].var[j]);    //record var's change
			gsm[i].var[j] = new_var[j];						 //update var
		}

		new_weight = sum1/data.size();
		change_weight += fabs(new_weight-gsm[i].weight);  //record weight's change
		gsm[i].weight = new_weight;						 //update weight
	}
	return change_weight+change_mean+change_var;	
}

void UBM::EM() {
	vector<double> t(data.size(), 0);
	for(int i=0; i<num_GSM; i++) {
		pr.push_back(t);
	}
	int cnt = 6000;
	double change = INF;
	int iter=1;

	cout << "EM begin!" << endl;
	while(iter <= cnt && change >= threshold_EM) {
		cout << "EM Iteration " << iter++ << ":" << endl;
		cout << "Performing E_step" << endl;
		E_step();
		cout << "Performing M_step" << endl;
		change = M_step();
		show_weight();
		show_mean();
		show_var();
		cout << "total change in M_step: " << change << endl;
	}
//	show_weight();
//	show_mean();
//	show_var();
	return;
}
//============================ end build UBM ===========================//

//========================= self adaption starts here ========================//
int UBM::read_personal_data(string filename) {
	ifstream in(filename);

	if(!in) {
		cerr << "Personal data file not exist!" << endl;
		return 1;
	}
	
	vector<double> temp;
	double t;
	while(in >> t) {
		temp.push_back(t);
		for(int i=1; i<dim; i++) {
			in >> t;
			temp.push_back(t);
		}
		personal_data.push_back(temp);
		temp.clear();
	}
	in.close();	
	cout << "Personal data read completed! Start training now........." << endl;
	return 0;
}

void UBM::self_adaption() {
	vector<double> temp(personal_data.size(), 0);
	for(int i=0; i<num_GSM; i++) {
		personal_pr.push_back(temp);
		Ex.push_back(vector<double>(dim,0));	
	}
	n.resize(num_GSM);
	// calculate personal pr[i|xt]
	double denominator;
	for(unsigned int i=0; i<personal_data.size(); i++) {
		denominator = 0.0;
		for(int j=0; j<num_GSM; j++) {
			personal_pr[j][i] = cal_gsm(j,personal_data[i])*gsm[j].weight;	
			denominator += personal_pr[j][i];
		}
		for(int j=0; j<num_GSM; j++) {
			personal_pr[j][i] /= denominator;
		}
	}
	
	// calculate personal data mean vector Ex[][] and n[];
	for(int i=0; i<num_GSM; i++) {
		n[i] = 0.0;
		for(unsigned int j=0; j<personal_data.size(); j++) 
			n[i] += personal_pr[i][j];
	
		for(int j=0; j<dim; j++) {
			Ex[i][j] = 0.0;
			for(unsigned int k=0; k<personal_data.size(); k++) 
				Ex[i][j] += personal_pr[i][k]*personal_data[k][j];	
			Ex[i][j] /= n[i];
		}
	}

	// just adapt the mean vector
	for(int i=0; i<num_GSM; i++) {
		int Rro = 16;
		double coef = n[i]/(n[i]+Rro);
		for(int j=0; j<dim; j++) {
			double old_mean = gsm[i].mean[j];
			gsm[i].mean[j] = coef*Ex[i][j] + (1-coef)*old_mean;
		}
	}
	show_mean();
}
//============================== end self adaption ===========================//

//============================== get point ================================//
double UBM::get_point(vector<vector<double>> &test) {
	double ans = 0;
	for(unsigned int i=0; i<test.size(); i++) {
		double temp = 0;
		for(int j=0; j<num_GSM; j++) {
			temp += cal_gsm(j,test[i])*gsm[j].weight;
		}
		ans += temp;
	}
	return ans/test.size();
};
//============================ end get point ==============================//
