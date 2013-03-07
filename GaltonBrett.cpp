
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

int main(int argc, char *argv[]){
	ifstream f2("GaltonBrett_header.s2c", fstream::in);
	ofstream f("GaltonBrett.s2c", fstream::out|fstream::trunc);
	
	string line;
	while(getline(f2,line)){
		f<<line<<'\n';
	}
	f2.close();
	
	int circles = 0;
	
	for(int x = 0; x<30; x++){
		for(int y = 0; y<30; y++){
			circles++;
			f<<"0 "; //decimal, fixed
			f<<0.01<<" "; //size
			f<<0.01<<" "; //mass
			f<<(0.5+0.05+0.03*x)<<" "; //pos
			f<<(2.0+0.02*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	for(int x = -22; x<22; x++){
		if(abs(x)<1) continue;
		circles++;
		f<<"1 "; //decimal, fixed
		f<<0.02<<" "; //size
		f<<0.01<<" "; //mass
		f<<(0.5+0.525+0.025*x+(x>0?1:-1)*0.008)<<" "; //pos
		f<<(1.3+0.03*abs(x))<<" ";
		f<<"0 "; //speed
		f<<"0\n";
	}
	for(int x = 0; x<30; x++){
		for(int y = 0; y<30; y++){
			circles++;
			f<<"1 "; //decimal, fixed
			f<<0.005<<" "; //size
			f<<0.01<<" "; //mass
			f<<((y%2==0?0.035:0)+0.025+0.075*x)<<" "; //pos
			f<<(0.5+0.025*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	for(int x = 0; x<30; x++){
		for(int y = 0; y<12; y++){
			circles++;
			f<<"1 "; //decimal, fixed
			f<<0.015<<" "; //size
			f<<0.01<<" "; //mass
			f<<(0.025+0.075*x)<<" "; //pos
			f<<(0.02+0.04*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	f.close();
	
	cout<<circles<<" circles."<<endl;
}
