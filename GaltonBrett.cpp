
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]){
	fstream f("save_.s2c", fstream::out|fstream::trunc);
	for(int x = 0; x<10; x++){
		for(int y = 0; y<10; y++){
			f<<"0 "; //decimal, fixed
			f<<0.015<<" "; //size
			f<<0.01<<" "; //mass
			f<<(0.05+0.1*x)<<" "; //pos
			f<<(2+0.1*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	for(int x = 0; x<10; x++){
		for(int y = 0; y<10; y++){
			f<<"1 "; //decimal, fixed
			f<<0.015<<" "; //size
			f<<0.01<<" "; //mass
			f<<(0.05+0.1*x)<<" "; //pos
			f<<(0.5+0.1*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
	for(int x = 0; x<10; x++){
		for(int y = 0; y<10; y++){
			f<<"1 "; //decimal, fixed
			f<<0.015<<" "; //size
			f<<0.01<<" "; //mass
			f<<(0.05+0.1*x)<<" "; //pos
			f<<(0.03*y)<<" ";
			f<<"0 "; //speed
			f<<"0\n";
		}
	}
}
