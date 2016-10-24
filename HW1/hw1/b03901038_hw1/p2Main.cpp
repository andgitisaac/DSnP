#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <climits>
#include <iomanip>
#include "p2Table.h"

using namespace std;

int main()
{
    Table table;

    // TODO: read in the csv file
    ifstream file;
    string csvFile, line, action, str;
    size_t Cols = 1, Rows = 0;    
    bool peekComma = true;
    vector<int> tmp, data;

    cout << "Please enter the file name: ";
    cin >> csvFile;
    if (table.read(csvFile))
       cout << "File \"" << csvFile << "\" was read in successfully." << endl;
    else exit(-1); // csvFile does not exist.

    file.open(csvFile.c_str(), ios::in); 
    while (getline(file, line)) {       
       for (size_t i=0; i < line.length()-1; i++){  
          if (int(line[i]) == 13) Rows++;
          if (Rows == 1 && int (line[i]) == 44) Cols++;
        }
    }

    file.clear();
    file.seekg(0, ios::beg);

    // while (getline(file, line, '\r')) {
    //   preLoc = 0;
    //   loc = line.find(',', preLoc);

    //   while(loc != string::npos){        
    //     if (loc == preLoc){
    //       tmp.push_back(INT_MAX);
    //     }
    //     else{
    //       tmp.push_back(atoi(line.substr(preLoc, loc-preLoc).c_str()));
    //     }
    //     preLoc = loc + 1;
    //     loc = line.find(',', preLoc);
    //   }      
    //   if (preLoc == line.size()){
    //     tmp.push_back(INT_MAX);
    //   }
    //   else{
    //     tmp.push_back(atoi(line.substr(preLoc).c_str()));
    //   }      
    // }
       
   
    table.initialize(Rows, Cols);

    for(size_t i=0; i < Rows;)
      for(size_t j=0; j < Cols;){
        if (peekComma){
          if (file.peek() == ','){
            file.ignore();          
            table[i][j] = INT_MAX;
            j++;
          }
          else if (file.peek() == '\r' || file.peek() == '\n'){
            file.ignore();
            table[i][j] = INT_MAX;
            i++;
            break;
          }
          else{
            peekComma = false;
            file >> table[i][j];
          }
          
        }
        else if (file.peek() == ','){          
          file.ignore();
          j++;
          peekComma = true;
        }
        else if (file.peek() == '\r' || file.peek() == '\n'){
          file.ignore();
          i++;
          peekComma = true;
          break;
        }
        else if (file.peek() == EOF){
          // cout << "End of file" << endl;
          peekComma = false;
          break;
        }
        else{
          // char c;
          peekComma = false;
          file >> table[i][j];
        }
      }









    // for(size_t i=0; i < Rows; i++)
   	// 	for(size_t j=0; j < Cols; j++){
   	// 		table[i][j] = tmp[cnt];
   	// 		cnt++;
   	// 	}

	line.clear();
	tmp.clear();	

    // TODO read and execute commands
  while (true) {
  	cin.clear();
   	line.clear();
   	action.clear();
   	data.clear();
  	bool flag = true;

  	getline(cin, line);
  	istringstream item(line);
   	while(item >> str){    		
   		if(flag){
   			action = str;
   			flag = false;
   		}
   		else{
   			if(str == "-")	data.push_back(INT_MAX);
   			else data.push_back(atoi(str.c_str()));
   		}    		
   	}

   	if(action == "PRINT") table.print();
   	else if(action == "SUM"){
   		cout << "The summation of data in column #"
   			<< data[0] << " is " << table.sum(data[0]) << endl;
   	}
  	else if(action == "AVE"){
  		cout << "The average of data in column #"
  			<< data[0] << " is " << fixed << setprecision(1)
  			<< table.ave(data[0]) << endl; 
  	}
   	else if(action == "MAX"){
  		cout << "The maximum of data in column #"
 				<< data[0] << " is " << table.max(data[0]) << endl;
  	}
   	else if(action == "MIN"){
   		cout << "The minimum of data in column #"
				<< data[0] << " is " << table.min(data[0]) << endl;
  	}
    else if(action == "COUNT"){
    	cout << "The distinct count of data in column #"
   			<< data[0] << " is " << table.count(data[0]) << endl;
    }
    else if(action == "ADD")	table.add(data);
    else if(action == "EXIT") break;    	
  }
  file.close();
  return 0;
}