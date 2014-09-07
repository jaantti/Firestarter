#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <stdlib.h>


using namespace std;

typedef map <const string, string> Properties;
Properties properties;

void read_properties();
int main(){
    read_properties();
    system("pause");
    return 0;
}

void read_properties(){
    string line;
    ifstream file;
    file.open("conf.properties");
    if (!file.is_open()) {
        cout << "Error opening conf.properties" << endl;
        return;
    }

    while(!file.eof()){
        getline(file, line);
        if (line.find_first_of("#") != -1 || line.find_first_of("=") == -1) continue;

        properties[line.substr(0,line.find('='))]=line.substr(line.find('=')+1);

    }
    for (Properties::iterator i=properties.begin();i!=properties.end();i++){

        if (i->first == "max_spd") cout << i->second<<endl;
        if (i->first == "slower_by") cout << i->second<<endl;


        cout << i->first << ":" << i->second <<endl;
    }

    //cout << atoi(properties["maxspeed"].c_str()) << endl;

}
