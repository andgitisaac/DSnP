#include "p2Table.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <climits>
#include <cmath>
#include <algorithm>

using namespace std;

// Implement member functions of class Row and Table here

void Table::initialize(int r, int c){
	length = r;
	for(int i=0; i < length; i++)
		_rows.push_back(Row(c));	
}

void Table::print(){
	for(int i=0; i < length; i++){
		for(int j=0; j < _rows[i].getSize(); j++){
			if(_rows[i][j] != INT_MAX)
				cout << right << setw(4) << _rows[i][j];
			else
				cout << right << setw(4) << " ";
		}
		cout << endl;
	}
}

int Table::sum(int index){
	int sum = 0;
	for(int i=0; i < length; i++)
		if (_rows[i][index] != INT_MAX)
			sum += _rows[i][index];
	return sum;
}

double Table::ave(int index){
	int sum = 0, cnt = 0;
	for(int i=0; i < length; i++)
		if (_rows[i][index] != INT_MAX){
			sum += _rows[i][index];
			cnt ++;
		}	
	return sum / double(cnt);
}

int Table::max(int index){
	int ans = INT_MIN;
	for(int i=0; i < length; i++)
		if(_rows[i][index] != INT_MAX)
			if(_rows[i][index] > ans)
				ans = _rows[i][index];
	return ans;
}

int Table::min(int index){
	int ans = INT_MAX;
	for(int i=0; i < length; i++)
		if(_rows[i][index] != INT_MAX)
			if(_rows[i][index] < ans)
				ans = _rows[i][index];
	return ans;
}

int Table::count(int index){
	std::vector<int> v;
	for(int i=0; i < length; i++)
		if(_rows[i][index] != INT_MAX)
			v.push_back(_rows[i][index]); 
	sort(v.begin(), v.end());
	std::vector<int>::iterator it = std::unique(v.begin(), v.end());
	v.erase(it, v.end());
	return v.size();	
}

void Table::add(vector<int> &data){
	int Cols = _rows[0].getSize();
	_rows.push_back(Row(Cols));
	for(int i=0; i < Cols; i++)
		_rows[length][i] = data[i];	
	length++;
}

const Row& Table::operator[] (size_t i) const{ return _rows[i]; }

Row& Table::operator[] (size_t i){ return _rows[i]; }

bool
Table::read(const string& csvFile)
{
	ifstream file(csvFile.c_str());
	if(file.good()) return true; // TODO
	else return false;
}
