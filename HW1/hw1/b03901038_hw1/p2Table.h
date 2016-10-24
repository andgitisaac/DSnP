#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
 
using namespace std;

class Row
{
public:
	Row(int length){
		size = length;
		_data = new int[size];
	}
	int getSize(){ return size;	}
	const int operator[] (size_t i) const { return _data[i]; } // TODO
    int& operator[] (size_t i) { return _data[i]; } // TODO

private:
    int  *_data;
    int size;
};

class Table
{
public:
	// Table();
	~Table(){ _rows.clear(); }
	void initialize(int, int);
	void print();
	int sum(int);
	double ave(int);
	int max(int);
	int min(int);
	int count(int);
	void add(vector<int>&);

    const Row& operator[] (size_t i) const;
    Row& operator[] (size_t i);

    bool read(const string&);

private:
    vector<Row>  _rows;
    int length;
};

#endif // P2_TABLE_H
