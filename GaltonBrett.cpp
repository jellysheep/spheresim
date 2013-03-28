
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
	
	int spheres = 0;
	
	for(int x = 0; x<40; x++){
		for(int y = 0; y<40; y++){
			spheres++;
			f<<"0 "; //decimal, fixed
			f<<0.006<<" "; //size
			f<<0.01<<" "; //mass
			f<<(0.6+0.05+0.020*x)<<" "; //pos
			f<<(2.0+0.02*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	int c = 30;
	for(int x = -c; x<=c; x++){
		if(abs(x)<1) continue;
		spheres++;
		f<<"1 "; //decimal, fixed
		f<<0.02<<" "; //size
		f<<0.01<<" "; //mass
		f<<(1.015+0.025*x+(x>0?1:-1)*0.015)<<" "; //pos
		f<<(1.5+0.03*abs(x))<<" ";
		f<<"0 "; //speed
		f<<"0\n";
	}
	for(int x = 0; x<32; x++){
		for(int y = 0; y<50; y++){
			spheres++;
			f<<"1 "; //decimal, fixed
			f<<0.005<<" "; //size
			f<<0.01<<" "; //mass
			f<<((y%2==1?0.035:0)+0.025+0.06*x)<<" "; //pos
			f<<(0.5+0.020*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	for(int x = 0; x<32; x++){
		for(int y = 0; y<12; y++){
			spheres++;
			f<<"1 "; //decimal, fixed
			f<<0.015<<" "; //size
			f<<0.01<<" "; //mass
			f<<(0.025+0.06*x)<<" "; //pos
			f<<(0.02+0.04*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	f.close();
	
	cout<<spheres<<" spheres."<<endl;
}
