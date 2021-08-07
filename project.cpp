#include <iostream>
#include <vector>
#include<string>
#include<cmath>
#include<algorithm>
#include <fstream>
#include<Windows.h>


using namespace std;

int n;
vector < string> table; // for the hash table
vector <int > nodes; // to store the hashed values of the links for example the hashsed value for a link named "a" is 97
vector <vector <string>>keys; // to store the key words for each page (nodes [i]) in a vector of strings
vector < pair <double, double >> pr; // to store the old and new values of Page rank
vector <pair <double, int > > search_results;   // to store the score and the hashed link in one vector and then sort the vector accodring to the scores
vector <double > score;		// to store the socre of each page 
vector <pair <double, double>> impressionsandclicks; // to store the impressions and the number of clicks of each page
vector <double> ctr;
vector <vector <int>>outs; // to store the links that go from the page to the other pages
vector <vector <int > >ins; // to store the pages that have links to the respective page


int Hash(string s) { // to hash the links 
	int sum = 0;	// the fucntion returns the  position of the string in the hash table
	for (int i = 0; i < s.size(); i++) {
		sum += int(s[i]);
	}

	sum %= n;
	int i = sum;

	if (table[i] == s) return i; 
	if (table[i] == "")
	{
		nodes.push_back(i); 
		table[i] = s; return i;
	}

	
	while (table[i] != "" &&table[i]!=s) {
		i++;

		if (table[i] == "") { table[i] = s; nodes.push_back(i);  return i; }

		else if (table[i] == s) return i;

	}
	
}




void ranking() {
	ifstream input;
	input.open("links.csv");
	if (input.fail()) {
		cout << "fail"; return;
	}
	vector <string> inp;
	while (!input.eof()) {
		string st;
		input >> st;
		
		inp.push_back(st);

	}
	
	inp.pop_back();


	for (int j = 0; j < inp.size(); j++) {
		
		string x = inp[j];
		x = x.substr(1, x.size() - 2);
		int i = 0; int first;
		for (i; i < x.size(); i++) {
			if (x[i] == ',') break;
		}
		string a = x.substr(0, i); // hashing the first page

		string b = x.substr(i + 1, x.size() - (i + 1)); // hashing the second link

		int aa = Hash(a);

		int bb = Hash(b);


		vector <int >::iterator it1,it2;
		it1 = find(ins[bb].begin(), ins[bb].end(), aa);// ignore multible edges
		if (it1!=ins[bb].end())ins[bb].push_back(aa);	
		
		it2 = find(outs[aa].begin(), outs[aa].end(), bb);// ignore multible edges
		if (it2!=outs[aa].end())outs[aa].push_back(bb);		 								
		
	}

	//cout << "num of nodes : "<<nodes.size() << endl;
	for (int i = 0; i < nodes.size(); i++)

	{
		
		pr[nodes[i]].first = 1.0 / nodes.size(); //  initilizing the page rank of each page to 1/m, where m is the number of pages
		pr[nodes[i]].second = 1.0 / nodes.size();
		
	}


	
	
	double def = 1; // it is the indicator that the page ranks have become stable or not
	
	while (def>0.0001){
		
		for (int i = 0; i < nodes.size(); i++) {
			int vertex = nodes[i];

			double newval = 0; 
			
			for (int j = 0; j < ins[vertex].size(); j++) { 
				int innode = ins[vertex][j];
				newval+= pr[innode].first / outs[innode].size();
				
			}
			
			pr[vertex].second = 0.15+ 0.85*newval; // update teh value of pagerank of the page table[vertex]
			
			
		}
		double mm = -1; // used to get the max difference between the rankings of a page before and after an update
		for (int i = 0; i < nodes.size(); i++) {
			double temp =pr[nodes[i]].first - pr[nodes[i]].second;
			temp = abs(temp);
			
			mm = max(mm,temp ); 
			swap(pr[nodes[i]].first, pr[nodes[i]].second);
		}
		def = mm; // 

		
	}

}
double calc_score(int hash) {  // do the calculations given by the equation in the slides  
								// // score is calculated using impressions, number of clicks and PR 
	double con = (0.1*impressionsandclicks[hash].first) / (1+0.1*impressionsandclicks[hash].first);
	double temp1 = 0.4 * pr[hash].second;
	double temp2 = pr[hash].second*(1 - con);
	double temp3 = con * ctr[hash];
	temp2 += temp3;
	temp2 *= 0.6;
	double r = temp1 + temp2;
	return r;
}

bool searchforkey(string key, string data) { // used to search for a key in a given string data 
											// return 1 if the key exsits in the string data
											// return 0 if the key does not exsis in the string data

	if (key.size() > data.size() || key.size() < data.size()) return 0;
	int size = data.size() - key.size();
	for (int i = 0; i <= data.size()-key.size(); i++) {// need to reclaculate // done
		string temp = data.substr(i, key.size());
		if (temp == key) return 1;
	}
	
	return 0;
}
void getscores() {

	ifstream input;
	input.open("impressions.csv");
	if (input.fail()) {
		cout << "fail"; return;
	}
	vector <string> inp;
	
	while (!input.eof()) { // getting impressions 
		string st;
		input >> st;

		inp.push_back(st);

	}

	inp.pop_back();

	
	ifstream input2;
	input2.open("clicknumbers.csv");
	if (input2.fail()) {
		cout << "fail"; return;
	}
	vector <string> inp2;
	
	while (!input2.eof()) { // getting clickthroughs 
		string st;
		input2 >> st;

		inp2.push_back(st);

	}

	inp2.pop_back();

	// getting impressions from the file
	for (int j=0;j<inp.size();j++){
		string x = inp[j];
		
		if (x[0]=='"')x = x.substr(1, x.size() - 2);
		int i = 0;
		for ( i = 0; i < x.size(); i++) {
			if (x[i] == ',') break;
		}
		string temp = x.substr(0, i);
		int link = Hash(temp); // error expected when the link is not listed in the first file
		string temp2 = x.substr(i + 1, x.size());
		double temp3  = stod(temp2);
		impressionsandclicks[link].first = temp3;
		//ctr[link].second = 0.0;



		
	}
	for (int j = 0; j < inp2.size(); j++) { // getting number of clicks
		string x = inp2[j]; 
		if (x[0] == '"')x = x.substr(1, x.size() - 2);
		int i = 0;
		for (i = 0; i < x.size(); i++) {
			if (x[i] == ',') break;
		}

		string temp = x.substr(0, i);
		int link = Hash(temp); 
		string temp2 = x.substr(i + 1, x.size());
		double temp3 = stod(temp2);
		impressionsandclicks[link].second = temp3;
		
	}
	// getting the value of the ctr from the number of impressions / number of clicks
	for (int i = 0; i < nodes.size(); i++) ctr[nodes[i]] = impressionsandclicks[nodes[i]].second/  impressionsandclicks[nodes[i]].first;
	/*for (int i = 0; i < nodes.size(); i++) {// getting scores out of pr and ctr 
		score[nodes[i]] = calc_score(nodes[i]);  // score is calculated using impressions, number of clicks and PR 
	}*/

	//for (int i = 0; i < nodes.size(); i++) cout << ctr[nodes[i]].second << endl;
	//for (int i = 0; i < nodes.size(); i++) cout <<table[nodes[i]] << "  " << score[nodes[i]] << endl;
	
}

void getkeys() {
	ifstream input;
	input.open("keys.csv");
	if (input.fail()) {
		cout << "fail"; return;
	}
	
	vector <string> inp;
	
	while (!input.eof()) {
		string st;
		input >> st;
		
		inp.push_back(st);

	}

	
	inp.pop_back();

	//for (int i = 0; i < inp.size(); i++) cout << inp[i] << endl;
	
	for (int j = 0; j < inp.size();j++) {
		string x = inp[j];
		x = x.substr(1, x.size() - 2);
		int i = 0;
		for (; i < x.size(); i++)  if (x[i] == ',') { break; }

		string a = x.substr(0, i);
		int aa = Hash(a);


		string b = "";
		i++; 
		for (i; i < x.size(); i++)
		{

			if (x[i] == ',')
			{
				keys[aa].push_back(b);
				b = "";
				
			}

			else b += x[i];
			
			
		}
		keys[aa].push_back(b); 

		
	}
	
	

}
void updateimpressions(vector < pair <double, int>> vec) { // update the number of impressions of each page
	for (int i = 0; i < vec.size(); i++) {
		impressionsandclicks[nodes[vec[i].second]].first++;
	}
	

}


void updateclicks(int user, vector < pair <double, int>> vec) {  // update the number of clicks of each page
	impressionsandclicks[nodes[vec[user].second]].second++;

}
void prtint_search_results(vector <pair <double, int>> sr) { // printf the pages resutled by the search,  sorted according to the socres
	if (sr.size() == 0) {
		printf("no matches found !\n");
		return;
	}
	printf("search results : \n");
	for (int i = 0; i < sr.size(); i++) {

		cout << i + 1 << " " << table[nodes[sr[i].second]] << endl;
	}

}
void saveupdates() {
	
	ofstream output1;

	output1.open("impressions.csv");
	
	if (output1.fail()) { cout << "fail"; return; }
	output1.clear();
	for (int i = 0; i < nodes.size(); i++) {
		string x = table[nodes[i]] + ','+ to_string(impressionsandclicks[nodes[i]].first);
		output1 << x << endl;
	}	

	
	ofstream output2;
	output2.open("clicknumbers.csv");
	output1.clear();
	if (output2.fail()) { cout << "fail"; return; }


	for (int i = 0; i < nodes.size(); i++) {
		string x = table[nodes[i]] + ',' + to_string(impressionsandclicks[nodes[i]].second);
		output2 << x <<endl;
	}
}
void search() {
	int f = 1; 

	printf("welcome\nwhat you wanna to do\n1)search \n2)exit\n");
	int user1; cin >> user1;
	
	if (user1 == 2) return;

	bool flag = 0;
	cin.ignore();
	while (1) {

		if (flag) cout << endl;



		if (!flag) {
			cout << endl;
			printf("enter your search\n");
			search_results.clear();

			string x; getline(cin, x);
			string a;
			bool quotation = 0,  one = 1;
			bool andd = 0, orr = 0;
			string part1, part2;
			if (x[0] == '"' && x[x.size() - 1] == '"') { quotation = 1; a = x.substr(1, x.size() - 2); }	//why -2
			else if (x.size() < 5) /*assuming that this is a songle word*/ {
				quotation = 1;
				a = x;
			}
			else {
				for (int u = 0; u < x.size(); u++) {
					if (x[u] == ' ') one = 0;

				}
				
				int i = 0;
				 
					for (i = 0; i < x.size() - 4; i++) {

						string temp = x.substr(i, 3);
						if (temp == "AND") {
							part1 = x.substr(0, i - 1); andd = 1; break;
						}

					}
				

				if (andd)
				{
					part2 = x.substr(i + 4, x.size() - i - 4);
				}
				else  if (!andd) {
					int j = 0;
					for (j; j < x.size() - 3; j++)
					{
						string temp = x.substr(j, 2);

						if (temp == "OR") {
							part1 = x.substr(0, j - 1); // need to change back to one after fixing the glitch

							orr = 1;
							break;
						}

					}
					part2 = x.substr(j + 3, x.size() - j - 3);
					if (!orr) {
						if (one) {
							a = x;
							quotation = 1;
						}
						else {
							orr = 1;
							int k = 0;
							for (k = 0; k < x.size(); k++)
							{
								if (x[k] == ' ') break;
							}
							part1 = x.substr(0, k);
							part2 = x.substr(k + 1, x.size() - k - 1);
						}
					}
				}
			}

			


			//if (quotation) cout << a << endl;
			//else  cout << part1 << "/" << part2 << endl;


			if (quotation) {

				for (int i = 0; i < nodes.size(); i++) {
					bool s = 0;
					for (int j = 0; j < keys[nodes[i]].size(); j++) {

						string words = keys[nodes[i]][j];

						if (!s)s = searchforkey(a, words);

					}
					if (s) search_results.push_back(make_pair(score[nodes[i]], i)); // results
				}

			}
			else {
				//bool s =0, s1=0;
				for (int i = 0; i < nodes.size(); i++) {
					bool s = 0, s1 = 0;
					for (int j = 0; j < keys[nodes[i]].size(); j++) {
						string words = keys[nodes[i]][j];

						if (!s)s = searchforkey(part1, words);
						if (!s1)s1 = searchforkey(part2, words);

						if (s && s1) break;
					}

					if (andd && s && s1)  search_results.push_back(make_pair(score[nodes[i]], i));

					else if (orr && (s || s1))  search_results.push_back(make_pair(score[nodes[i]], i));
				}




			}

			sort(search_results.rbegin(), search_results.rend());

			updateimpressions(search_results);
			//part1 = part1.substr(1, part1.size() - 1);
		}
		flag = 0;
		prtint_search_results(search_results);

		printf("would you like to \n1)choose a web page to open \n2)new search \n3)exit\n");
		cin >> user1;
		int choice;

		if (user1 == 3) break;
		else if (user1 == 2)
		{
			cin.ignore(); continue;
		}

		else if (user1 == 1) {
			cin >> choice;
			updateclicks(choice -1, search_results);
			cout << "\nyou are now viewing the page: " << table[nodes[search_results[choice-1].second]] << endl;
			printf("would you like to \n1)back to search results\n2)new search\n3)exit\n");

			int input; cin >> input;

			if (input == 3) break;

			else if (input == 2)
			{
				cin.ignore();
				continue;
			}
			else if (input == 1)
			{
				flag = 1;

				continue;
			}


		}

		

		cin.ignore();

	}
	saveupdates();

}


int main() {  
		
	n = 10000;
	table.resize(n);
	outs.resize(n);
	ins.resize(n);
	pr.resize(n);
	impressionsandclicks.resize(n);
	score.resize(n);
	keys.resize(n);
	ctr.resize(n);
	

	ranking();

	
	getkeys();
	getscores();
	
	search();
	




}

