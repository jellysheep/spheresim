#ifdef ENGINE_CPP

#include "EigenCalculator_Engine.h"

#include <iostream>
using namespace std;


#include <QFileDialog>

#include "NanosecondTimer.h"
#include "GLWidget.h"

#define parallelFor 
//_Pragma("omp parallel for if(spheresCount>500)")

#define fixSun 0

uint32_t calcZOrder(uint16_t xPos, uint16_t yPos);


template <int dims, bool _3D_>
EigenCalculator_Engine<dims,_3D_>::EigenCalculator_Engine():EigenCalculator_QObject(){
	omp_set_num_threads(1);
	srand(2);
	//srand(NanosecondTimer::getNS());
	
	_E = E*1000000.0;
		
	bool eVectorizing = false;
	#ifdef EIGEN_VECTORIZE
		eVectorizing = true;
	#endif
	cout<<"Vectorizing: "<<eVectorizing<<endl;
	
	timeInterval = speed*speedCorrection/fps;
	spheres = new Sphere[spheresCount];
	spheresPos = new eVector[spheresCount];
	spheresOldPos = new eVector[spheresCount];
	spheresSpeed = new eVector[spheresCount];
	spheresForce = new eVector[spheresCount];
	both_r = new scalar*[spheresCount];
	
	numCells = min(pow_int(rowsPerStep, curveSteps), maxCellsPerAxis);
	updateGridSize();
	gridIndex = new int*[spheresCount];
	
	posX = new Pos[spheresCount];
	posY = new Pos[spheresCount];
	if(_3D_){
		posZ = new Pos[spheresCount];
	}
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		posX[i].posOfSphere = i;
		posX[i].sphereAtPos = i;
		posY[i].posOfSphere = i;
		posY[i].sphereAtPos = i;
		if(_3D_){
			posZ[i].posOfSphere = i;
			posZ[i].sphereAtPos = i;
		}
	}
	
	///cellSorting
	
	firstSphereInCell = new int[numCells*numCells*(_3D_?numCells:1)];
	memset(firstSphereInCell, 0, numCells*numCells*(_3D_?numCells:1));
	posCell = new Pos[spheresCount];
	cellOfSphere = new int[spheresCount];
	
	curveIndices = new int[numCells*numCells*(_3D_?numCells:1)];
	buildCurveIndices_RowColumn();
	//buildCurveIndices_zOrder();
	//buildCurveIndices_Peano();
	//buildCurveIndices_Hilbert();
	for(int y = numCells-1; y>=0; y--){
		for(int x = 0; x<numCells; x++){
			printf("%3d ", curveIndices[x+numCells*y]);
		}
		printf("\n");
	}
	
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		
		both_r[i] = new scalar[spheresCount];
		
		initSphere(i);
		
	}
	
	///cellSorting
	for(int i = 0; i<spheresCount; i++){
		posCell[i].sphereAtPos = i;
		posCell[i].posOfSphere = i;
	}
	sortSpheresByCells();
	
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		for(int j = 0; j<spheresCount; j++){
			both_r[i][j] = spheres[i].size + spheres[j].size;
		}
	}
	if(renderBool){
		readNum_render = min(maxShowSpheresCount,spheresCount);
		printf("Spheres to render: %d\n", readNum_render);
		renderBuffer = new eVector*[renderBufferCount];
		for(int i = 0; i<renderBufferCount; i++){
			renderBuffer[i] = new eVector[readNum_render];
		}
		bufferReadIndex = 0;
		bufferWriteIndex = 0;
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
	}
	
	numSpheresInCell = new char[numCells*numCells*(_3D_?numCells:1)];
	spheresInCell = new int*[numCells*numCells*(_3D_?numCells:1)];
	for(int i = (numCells*numCells*(_3D_?numCells:1))-1; i>=0; i--){
		spheresInCell[i] = new int[maxNumSpheresInCell];
	}
	
	printf("EigenCalculator_Engine initialized!\n");
}

//Hilbert-Kurve:
//rotate/flip a quadrant appropriately
void rot(int n, int *x, int *y, int rx, int ry) {
    if (ry == 0) {
        if (rx == 1) {
            *x = n-1 - *x;
            *y = n-1 - *y;
        }
 
        //Swap x and y
        int t  = *x;
        *x = *y;
        *y = t;
    }
}
//convert (x,y) to d
int xy2d_Hilbert (int n, int x, int y) {
    int rx, ry, s, d=0;
    for (s=n/2; s>0; s/=2) {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(s, &x, &y, rx, ry);
    }
    return d;
}
template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::buildCurveIndices_Hilbert(){
	for(int y = 0; y<numCells; y++){
		for(int x = 0; x<numCells; x++){
			if(_3D_){
				for(int z = 0; z<numCells; z++){
					curveIndices[x+numCells*y] = xy2d_Hilbert(numCells, x, y) + numCells*numCells*z;
				}
			}else{
				curveIndices[x+numCells*y] = xy2d_Hilbert(numCells, x, y);
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::buildCurveIndices_RowColumn(){
	for(int y = 0; y<numCells; y++){
		for(int x = 0; x<numCells; x++){
			if(_3D_){
				for(int z = 0; z<numCells; z++){
					curveIndices[x+numCells*y] = x+numCells*y + numCells*numCells*z;
				}
			}else{
				curveIndices[x+numCells*y] = x+numCells*y;
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::buildCurveIndices_zOrder(){
	for(int y = 0; y<numCells; y++){
		for(int x = 0; x<numCells; x++){
			if(_3D_){
				for(int z = 0; z<numCells; z++){
					curveIndices[x+numCells*y] = calcZOrder(x,y) + numCells*numCells*z;
				}
			}else{
				curveIndices[x+numCells*y] = calcZOrder(x,y);
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::buildCurveIndices_Peano(){
	indexCounter = 0;
	buildPeanoCurve(0, 0, 0, 0, 0);
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::buildPeanoCurve(int x, int y, int z, int step, int direction){
	// direction: 
	// 0 = /^
	// 1 = ^\ 
	
	if(step == curveSteps){
		//highest recursion depth
		curveIndices[x+numCells*y+numCells*numCells*z] = indexCounter++;
	}else{
		x *= 3;
		y *= 3;
		z *= 3;
		step++;
		if(_3D_){
			///TODO
		}else{
			if(direction == 0){
				buildPeanoCurve(x+0, y+0, z, step, 0+direction);
				buildPeanoCurve(x+0, y+1, z, step, 1-direction);
				buildPeanoCurve(x+0, y+2, z, step, 0+direction);
				buildPeanoCurve(x+1, y+2, z, step, 1-direction);
				buildPeanoCurve(x+1, y+1, z, step, 0+direction);
				buildPeanoCurve(x+1, y+0, z, step, 1-direction);
				buildPeanoCurve(x+2, y+0, z, step, 0+direction);
				buildPeanoCurve(x+2, y+1, z, step, 1-direction);
				buildPeanoCurve(x+2, y+2, z, step, 0+direction);
			}else if(direction == 1){
				buildPeanoCurve(x+2, y+0, z, step, 0+direction);
				buildPeanoCurve(x+2, y+1, z, step, 1-direction);
				buildPeanoCurve(x+2, y+2, z, step, 0+direction);
				buildPeanoCurve(x+1, y+2, z, step, 1-direction);
				buildPeanoCurve(x+1, y+1, z, step, 0+direction);
				buildPeanoCurve(x+1, y+0, z, step, 1-direction);
				buildPeanoCurve(x+0, y+0, z, step, 0+direction);
				buildPeanoCurve(x+0, y+1, z, step, 1-direction);
				buildPeanoCurve(x+0, y+2, z, step, 0+direction);
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::initSphere(int i){
	spheres[i].size = rans(sphereSize.s0, sphereSize.s1);
	spheres[i].E = E;
	spheres[i].poisson = poisson;
	spheres[i].fixed = 0;
	
	spheresPos[i].Zero();
	spheresPos[i](0) = spheres[i].size+rans(boxSize.s0-2*spheres[i].size);
	spheresPos[i](1) = spheres[i].size+rans(boxSize.s1-2*spheres[i].size);
	if(_3D_){
		spheresPos[i](2) = spheres[i].size+rans(boxSize.s2-2*spheres[i].size);
	}
	spheresOldPos[i] = spheresPos[i];
	
	gridIndex[i] = new int[dims];
	gridIndex[i][0] = spheresPos[i](0)/gridWidth;
	gridIndex[i][1] = spheresPos[i](1)/gridWidth;
	if(_3D_){
		gridIndex[i][2] = spheresPos[i](2)/gridWidth;
	}
	
	spheresSpeed[i] = eVector::Random();
	#if useSSE
		if(_3D_){
			spheresSpeed[i](3) = 0;
		}
		#if _double_==0
			else{
				spheresSpeed[i](2) = 0;
			}
		#endif
	#endif
	spheresSpeed[i].normalize();
	spheresSpeed[i] *= rans(max_speed);
	//cout<<spheresSpeed[i]<<endl<<endl;
	
	spheresForce[i].Zero();
	
	//cout<<"3D enabled: "<<(_3D_?1:0)<<endl<<endl;
	
	#if fixSun
		if(i == 0) spheres[i].size = 3*sphereSize.s0;
		//if(i == 0) spheres[i].mass*=5;
		if(i == 0) ceBuffer[i].hsvColor = QColor::fromHsv(0,0,0);
		if(i == 0) spheresPos[i](0) = boxSize.s0/2;
		if(i == 0) spheresPos[i](1) = boxSize.s1/2;
		if(_3D_){
			if(i == 0) spheresPos[i](2) = boxSize.s2/2;
		}
		if(i == 0) spheresSpeed[i] = eVector::Zero();
	#endif
	spheresOldPos[i] = spheresPos[i];
	spheres[i].mass = 4.0/3.0*pow(spheres[i].size,3)*M_PI  *950; //Kautschuk
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateGridSize(){
	gridWidth = max(2*sphereSize.s1, max(boxSize.s0, max(boxSize.s1, boxSize.s2))/numCells);
	printf("gridWidth: %5f \n", gridWidth);
	if(_3D_){
		gridSteps = (int)(max(boxSize.s0, max(boxSize.s1, boxSize.s2))/gridWidth);
	}else{
		gridSteps = (int)(max(boxSize.s0, boxSize.s1)/gridWidth);
	}
	printf("Grid steps: %5d\n", gridSteps);
}


template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::save(){
	//not yet implemented
}

#define _G_ 1
#define newFor 0
#define joinFors 1

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::calcWallResistance(){
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		if(spheres[i].fixed == 1) continue;
		scalar d_, R, _E_;
		scalar force_, htw_d_d, fact = 1.0f;
		if ((htw_d_d = (spheres[i].size - spheresPos[i](0)))>0) {
			//printf("Sphere %d collides with left wall\n", i);
			if(spheresSpeed[i](0) > 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](0) += force_*fact;
			curWallForces[0] += force_*fact;
		}else if ((htw_d_d = (spheres[i].size + spheresPos[i](0) - boxSize.s0))>0) {
			//printf("Sphere %d collides with right wall\n", i);
			if(spheresSpeed[i](0) < 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](0) -= force_*fact;
			curWallForces[1] += force_*fact;
		}
		fact = 1.0f;
		if ((htw_d_d = (spheres[i].size - spheresPos[i](1)))>0) {
			//printf("Sphere %d collides with bottom wall\n", i);
			if(spheresSpeed[i](1) > 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](1) += force_*fact;
			curWallForces[2] += force_*fact;
		}else if ((htw_d_d = (spheres[i].size + spheresPos[i](1) - boxSize.s1))>0) {
			//printf("Sphere %d collides with top wall\n", i);
			if(spheresSpeed[i](1) < 0)fact = elastic;
			d_ = htw_d_d;
			R = spheres[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			spheresForce[i](1) -= force_*fact;
			curWallForces[3] += force_*fact;
		}
		if(_3D_){
			fact = 1.0f;
			if ((htw_d_d = (spheres[i].size - spheresPos[i](2)))>0) {
				if(spheresSpeed[i](2) > 0)fact = elastic;
				d_ = htw_d_d;
				R = spheres[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				spheresForce[i](2) += force_*fact;
				curWallForces[4] += force_*fact;
			}else if ((htw_d_d = (spheres[i].size + spheresPos[i](2) - boxSize.s2))>0) {
				if(spheresSpeed[i](2) < 0)fact = elastic;
				d_ = htw_d_d;
				R = spheres[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				spheresForce[i](2) -= force_*fact;
				curWallForces[5] += force_*fact;
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::calcBallResistance(){
	parallelFor
	for(int i = 0; i<spheresCount; i++){
		eVector force;
		
		eVector d_pos, d_n;
		scalar *both_r_ = &this->both_r[i][0], d, d_, R, _E_;
	#if newFor
		int j;
		for(int a = 0; a<x; a++){
			j = (i+a+1)%spheresCount;
	#else
		for(int j = i+1; j<spheresCount; j++){
	#endif
			
			//#define c2 sphere[i]
			//both_r = spheres[i].size + spheres[j].size;
			//both_r = this->both_r[i][j];
			
			#if _G_
			if(G==0)
			#endif
			{
				if(abs(gridIndex[i][0]-gridIndex[j][0]) >1) continue;
				if(abs(gridIndex[i][1]-gridIndex[j][1]) >1) continue;
				if(_3D_){
					if(abs(gridIndex[i][2]-gridIndex[j][2]) >1) continue;
				}
			}
			
			collideBalls(i,j);
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::collideBalls(int i, int j){
	if(i == j) return;
	scalar both_r_ = both_r[i][j];
	eVector d_pos = spheresPos[j]-spheresPos[i];
	
	//d_pos.s2 = 0;
	scalar d = d_pos.norm();
	d = max(d,(scalar)0.00005f);
	
	eVector d_n, force;
	
	#if _G_
	if(G_fact!=0)
	{
		d_n = d_pos/d;
		// bzw. normalize(d_pos);
		// Gravitation:
		force = G*G_fact*spheres[i].mass*spheres[j].mass/pow(max(d,(scalar)spheres[i].size/10),2) *d_n;
		spheresForce[i] += force;
		
		spheresForce[j] -= force;
	}
	#endif
	
	// Abstossung:
	if (d < both_r_) {
		//printf("Spheres %d and %d are colliding!\n", i, j);
		scalar d_, R, _E_;
		//*
		//#if _G_==0
		if(G_fact == 0)
		{
			d_n = d_pos/d; // bzw. normalize(d_pos);
		}
		//#endif
		// */
		// nach Kontaktmechanik mit Poisson-Zahlen:
		d_ = both_r_ - d;
		//*
		R = 1/((1/spheres[i].size)+(1/spheres[j].size));
		//_E_ = 1/(((1-spheres[i].poisson*spheres[i].poisson)/(_E))+((1-spheres[j].poisson*spheres[j].poisson)/(_E)));
		_E_ = 1/(((1-(poisson*poisson))/(_E))+((1-(poisson*poisson))/(_E)));
		force = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3)) *d_n;
		//printf("Stoss! r: %5f m: %5f d: %5f R: %5f E*: %5f F: %5f\n", spheres[i].size, spheres[i].mass, d_, R, _E_, force.norm());
		if(elastic!=1 && d_pos.dot((spheresSpeed[i]-spheresSpeed[j])/d)<0){ //Skalarprodukt
			force *= elastic;
		}
		#pragma atomic
			spheresForce[i] -= force;
		#pragma atomic
			spheresForce[j] += force;
	}
}

#define sqr(x) ((x)*(x))
template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::sumUpForces(){
	//parallelFor
	for(int i = 0; i<spheresCount; i++){
		if(spheres[i].fixed == 0){
			eVector force;
			#if fixSun
				if(i==0)continue;
			#endif
			EIGEN_ASM_COMMENT("begin");
			spheresForce[i] -= airResistance*(0.47*M_PI*sqr(spheres[i].size))*0.5*spheresSpeed[i]*std::abs(spheresSpeed[i].norm());
			force = spheresForce[i];
			/*printf("sphere %d force: %5f %5f\n", i, spheresForce[i](0), spheresForce[i](1));
			printf("sphere %d speed: %5f %5f\n", i, spheresSpeed[i](0), spheresSpeed[i](1));
			printf("sphere %d pos: %5f %5f\n", i, spheresPos[i](0), spheresPos[i](1));
			printf("sphere %d old pos: %5f %5f\n", i, spheresOldPos[i](0), spheresOldPos[i](1));
			printf("sphere %d mass: %5f\n", i, spheres[i].mass);
			QThread::msleep(10);// */
			eVector acceleration = force / spheres[i].mass;
			acceleration(0) += gravity.s0;
			acceleration(1) += gravity.s1;
			//printf("sphere %d gravity: %5f %5f\n", i, gravity.s0, gravity.s1);
			if(_3D_){
				acceleration(2) += gravity.s2;
			}
			spheresOldPos[i] += spheresSpeed[i]*timeInterval;
			spheresOldPos[i] += 0.5*acceleration*timeInterval*timeInterval;
			
			spheresSpeed[i] += acceleration*timeInterval;
			spheresForce[i] -= force;
			
			#if 0
				if(wallResistance){
					spheresOldPos[i](0) = min(spheresOldPos[i](0), boxSize.s0);
					spheresOldPos[i](1) = min(spheresOldPos[i](1), boxSize.s1);
					if(_3D_) spheresOldPos[i](2) = min(spheresOldPos[i](2), boxSize.s2);
					spheresOldPos[i](0) = max(spheresOldPos[i](0), 0.0);
					spheresOldPos[i](1) = max(spheresOldPos[i](1), 0.0);
					if(_3D_) spheresOldPos[i](2) = max(spheresOldPos[i](2), 0.0);
				}
			#endif
		}
		spheresPos[i] = spheresOldPos[i];
		spheresPos[i] += spheresSpeed[i]*(timeInterval/2);
		//spheresPos[i] += 0.5*acceleration*(timeInterval/2)*(timeInterval/2);
		
		gridIndex[i][0] = (int)(spheresPos[i](0)/gridWidth);
		gridIndex[i][1] = (int)(spheresPos[i](1)/gridWidth);
		if(_3D_){
			gridIndex[i][2] = spheresPos[i](2)/gridWidth;
		}
		/*printf("sphere %d speed: %5f %5f\n", i, spheresSpeed[i](0), spheresSpeed[i](1));
		printf("sphere %d pos: %5f %5f\n", i, spheresPos[i](0), spheresPos[i](1));
		printf("sphere %d old pos: %5f %5f\n", i, spheresOldPos[i](0), spheresOldPos[i](1));// */
		
		EIGEN_ASM_COMMENT("end");
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::sort(Pos* p, int dim){
	//scalar temp;
	int temp;
	int j, cid; //sphere ID

	for (int i=1; i < spheresCount; i++){
		cid = p[i].sphereAtPos;
		//temp = spheresPos[cid](dim);
		temp = gridIndex[cid][dim];
		j = i-1;

		//while (j >= 0 && spheresPos[p[j].sphereAtPos](dim) > temp){
		while (j >= 0 && gridIndex[p[j].sphereAtPos][dim] > temp){
			p[j+1].sphereAtPos = p[j].sphereAtPos;
			j--;
		}

		p[j+1].sphereAtPos = cid;
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::calcSortedBallResistance(){
	sort(posX, 0);
	/*
	for(int pid = 0; pid<spheresCount; pid++){ // pos. ID
		printf("%3d ", posX[pid].sphereAtPos);
	}
	printf("\n"); // */

	/// Kugeln kollidieren nur mit anderen, die rechts davon liegen.
	/// Dadurch werden doppelt berechnete Kollisionen vermieden.
	
	parallelFor
	for(int pid = 0; pid<spheresCount; pid++){ // pos. ID
		int i = posX[pid].sphereAtPos, j; //sphere IDs
		posX[i].posOfSphere = pid;
		for(int pid2 = pid+1; pid2<spheresCount; pid2++){
			j = posX[pid2].sphereAtPos;
			if((gridIndex[j][0] <= gridIndex[i][0]+1)){
			//if(abs(gridIndex[i][0]-gridIndex[j][0]) <=1){
			//if(spheresPos[j][0]-spheresPos[i][0] < both_r[i][j]){
				if(abs(gridIndex[i][1]-gridIndex[j][1]) <=1){
					if((!_3D_) || (abs(gridIndex[i][2]-gridIndex[j][2]) <=1)){
						collideBalls(i, j);
					}
				}
			}else{
				break;
			}
		}
	}
}

uint32_t calcZOrder(uint16_t xPos, uint16_t yPos)
{
    static const uint32_t MASKS[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const uint32_t SHIFTS[] = {1, 2, 4, 8};

    uint32_t x = xPos;  // Interleave lower 16 bits of x and y, so the bits of x
    uint32_t y = yPos;  // are in the even positions and bits from y in the odd;

    x = (x | (x << SHIFTS[3])) & MASKS[3];
    x = (x | (x << SHIFTS[2])) & MASKS[2];
    x = (x | (x << SHIFTS[1])) & MASKS[1];
    x = (x | (x << SHIFTS[0])) & MASKS[0];

    y = (y | (y << SHIFTS[3])) & MASKS[3];
    y = (y | (y << SHIFTS[2])) & MASKS[2];
    y = (y | (y << SHIFTS[1])) & MASKS[1];
    y = (y | (y << SHIFTS[0])) & MASKS[0];

    const uint32_t result = x | (y << 1);
    return result;
}

template <int dims, bool _3D_>
int EigenCalculator_Engine<dims,_3D_>::calcCellID(int x, int y, int z){
	x = min(numCells-1, x);
	y = min(numCells-1, y);
	z = min(numCells-1, z);
	x = max(0, x);
	y = max(0, y);
	z = max(0, z);
	return curveIndices[x+numCells*y+(_3D_?numCells*numCells*z:0)];
	//return x + numCells*y + numCells*numCells*z;
	//return calcZOrder(x,y) + numCells*numCells*z;
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::sortSpheresByCells(){
	if(_3D_){
		for(int i = 0; i<spheresCount; i++){
			//calculate cell ID
			cellOfSphere[i] = calcCellID(gridIndex[i][0], gridIndex[i][1], gridIndex[i][1]);
		}
	}else{
		for(int i = 0; i<spheresCount; i++){
			//calculate cell ID
			cellOfSphere[i] = calcCellID(gridIndex[i][0], gridIndex[i][1]);
			//printf
		}
	}
	
	int temp;
	int j, cid; //sphere ID
	for (int i=1; i < spheresCount; i++){
		cid = posCell[i].sphereAtPos;
		temp = cellOfSphere[cid];
		j = i-1;

		//while (j >= 0 && spheresPos[p[j].sphereAtPos](dim) > temp){
		while (j >= 0 && cellOfSphere[posCell[j].sphereAtPos] > temp){
			posCell[j+1].sphereAtPos = posCell[j].sphereAtPos;
			j--;
		}

		posCell[j+1].sphereAtPos = cid;
	}
	
	//save array index of each sphere
	for(int i = 0; i<spheresCount; i++){
		posCell[posCell[i].sphereAtPos].posOfSphere = i;
	}
	
	/*
	//set first sphere of each cell to -1
	for(int i = boxSize.s0/gridWidth + 1; i>=0; i--){
		for(j = boxSize.s1/gridWidth + 1; j>=0; j--){
			if(_3D_){
				for(int k = boxSize.s2/gridWidth; k>=0; k--){
					firstSphereInCell[i + numCells*j + numCells*numCells*k] = -1;
				}
			}else{
				firstSphereInCell[i + numCells*j] = -1;
			}
		}
	}// */
	
	//find posCell array position of first sphere of each cell
	for(int i = spheresCount-1; i>=0; i--){
		firstSphereInCell[cellOfSphere[posCell[i].sphereAtPos]] = i;
	}
	/*for(int i = 0; i<spheresCount; i++){
		printf("Sphere: %2d cellID: %2d first sphere in cell: %2d\n", posCell[i].sphereAtPos, cellOfSphere[posCell[i].sphereAtPos], posCell[firstSphereInCell[cellOfSphere[posCell[i].sphereAtPos]]].sphereAtPos);
	}// */
}

#define twiceCalcCollisions 1

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::checkCollision(int i, int x, int y, int z, bool sameCell){ 
	//sphere ID, cell position, bool if same cell (i.e. dx = dy = dz = 0)
	if(x<0 || y<0 || z<0 || x>=numCells || y>=numCells || z>=numCells){
		//if(x>=numCells || y>=numCells) printf("return because out of bounds\n");
		return;
	}
	int cellID = calcCellID(x,y,z);
	int j = firstSphereInCell[cellID]; //sphere 2 pos in array
	if(j < 0){
		//printf("empty cell: %2d (%2d)\n", cellID, j);
		return;
	}
	int c2 = posCell[j].sphereAtPos; //sphere 2 ID
	//printf("sphere 2 ID: %3d\n", c2);
	//if(c2>spheresCount) printf("cell: %2d sphere 2 ID: %2d cell of sphere before: %2d cell of next sphere: %2d\n", cellID, c2, cellOfSphere[posCell[j-1].sphereAtPos], cellOfSphere[posCell[j+1].sphereAtPos]);
	//printf("cell: %2d | %2d cell of sphere before: %2d cell of next sphere: %2d\n", cellID, cellOfSphere[posCell[j].sphereAtPos], cellOfSphere[posCell[j-1].sphereAtPos], cellOfSphere[posCell[j+1].sphereAtPos]);
	while(cellOfSphere[c2] == cellID){
		#if twiceCalcCollisions==0
			if(!(sameCell && spheresOldPos[i][0]>spheresOldPos[c2][0]))
		#endif
		{
			//if(spheresOldPos[i][0]>spheresOldPos[c2][0]) 
			if(i>c2) 
			collideBalls(i, c2);
		}
		j++;
		//if(j>=spheresCount) return;
		c2 = posCell[j].sphereAtPos;
		//if(c2>=spheresCount || c2<0) return;
	}; //check collisions of all spheres in that cell
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::calcCellSortedBallResistance(){
	//QThread::msleep(50);
	sortSpheresByCells();
	
	/*
	for(int pid = 0; pid<spheresCount; pid++){ // pos. ID
		printf("%3d ", posX[pid].sphereAtPos);
	}
	printf("\n"); // */

	/// Kugeln kollidieren nur mit anderen, die in Zellen rechts, oben, vorne davon liegen.
	/// Dadurch werden doppelt berechnete Kollisionen vermieden.
	
	if(_3D_){
		parallelFor
		for(int i = 0; i<spheresCount; i++){ //sphere ID 1
			int x = gridIndex[i][0], y = gridIndex[i][1], z = gridIndex[i][2];
			// dz = -1
			// ___
			// ___
			// ___
			
			#if twiceCalcCollisions
				checkCollision(i, x+0, y+0, z-1);
				checkCollision(i, x+1, y+0, z-1);
				checkCollision(i, x-1, y+0, z-1);
				checkCollision(i, x+0, y+1, z-1);
				checkCollision(i, x+1, y+1, z-1);
				checkCollision(i, x-1, y+1, z-1);
				checkCollision(i, x+0, y-1, z-1);
				checkCollision(i, x+1, y-1, z-1);
				checkCollision(i, x-1, y-1, z-1);
			#endif
			
			// dz = 0
			// _XX
			// _0X
			// __X
			
			checkCollision(i, x+0, y+0, z+0, true);
			checkCollision(i, x+1, y+0, z+0);
			checkCollision(i, x+1, y+1, z+0);
			checkCollision(i, x+1, y+1, z+0);
			checkCollision(i, x+0, y+1, z+0);
			
			#if twiceCalcCollisions
				checkCollision(i, x+0, y-1, z+0);
				checkCollision(i, x-1, y+0, z+0);
				checkCollision(i, x-1, y+1, z+0);
				checkCollision(i, x-1, y-1, z+0);
			#endif
			
			// dz = 1
			// XXX
			// XXX
			// XXX
			
			checkCollision(i, x+0, y+0, z+1);
			checkCollision(i, x+1, y+0, z+1);
			checkCollision(i, x-1, y+0, z+1);
			checkCollision(i, x+0, y+1, z+1);
			checkCollision(i, x+1, y+1, z+1);
			checkCollision(i, x-1, y+1, z+1);
			checkCollision(i, x+0, y-1, z+1);
			checkCollision(i, x+1, y-1, z+1);
			checkCollision(i, x-1, y-1, z+1);
		}
	}else{
		parallelFor
		for(int i = 0; i<spheresCount; i++){ //sphere ID 1
			int x = gridIndex[i][0], y = gridIndex[i][1], z = 0;
			// _XX
			// _0X
			// __X
			checkCollision(i, x+0, y+0, z);
			checkCollision(i, x+1, y+0, z);
			checkCollision(i, x+1, y+1, z);
			checkCollision(i, x+1, y-1, z);
			checkCollision(i, x+0, y+1, z);
			
			#if twiceCalcCollisions
				checkCollision(i, x+0, y-1, z);
				checkCollision(i, x-1, y+0, z);
				checkCollision(i, x-1, y+1, z);
				checkCollision(i, x-1, y-1, z);
			#endif
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::countSpheresPerCell(){
	for(int i = (numCells*numCells*(_3D_?numCells:1))-1; i>=0; i--){
		numSpheresInCell[i] = 0;
	}
	static const scalar fact = 1.1;
	parallelFor
	for(int i = 0; i<spheresCount; i++){
		int minX, maxX, minY, maxY, minZ, maxZ, cellID;
		minX = (int)((spheresPos[i](0)-spheres[i].size*fact)/gridWidth);
		maxX = (int)((spheresPos[i](0)+spheres[i].size*fact)/gridWidth);
		minY = (int)((spheresPos[i](1)-spheres[i].size*fact)/gridWidth);
		maxY = (int)((spheresPos[i](1)+spheres[i].size*fact)/gridWidth);
		if(_3D_){
			minZ = (int)((spheresPos[i](2)-spheres[i].size*fact)/gridWidth);
			maxZ = (int)((spheresPos[i](2)+spheres[i].size*fact)/gridWidth);
		}
		for(int x = minX-1; x<=maxX+1; x++){
			for(int y = minY-1; y<=maxY+1; y++){
				if(_3D_){
					for(int z = minZ-1; z<=maxZ+1; z++){
						cellID = calcCellID(x,y,z);
						spheresInCell[cellID][numSpheresInCell[cellID]++] = i;
					}
				}else{
					cellID = calcCellID(x,y);
					spheresInCell[cellID][numSpheresInCell[cellID]++] = i;
				}
			}
		}
	}
	return;
	for(int i = (numCells*numCells*(_3D_?numCells:1))-1; i>=0; i--){
		if(numSpheresInCell[i]>=maxNumSpheresInCell){
			printf("Many (%3d) spheres in cell!\n", numSpheresInCell[i]);
		}
		numSpheresInCell[i] = std::min(maxNumSpheresInCell-1, (int)numSpheresInCell[i]);
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::collideSpheresPerCell(){
	parallelFor
	for(int i = (numCells*numCells*(_3D_?numCells:1))-1; i>=0; i--){
		//cycle through cell IDs
		int num = numSpheresInCell[i];
		for(int c = 0, c2; c<num-1; c++){
			for(c2 = c+1; c2<num; c2++){
				collideBalls(spheresInCell[i][c], spheresInCell[i][c2]);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::doStep(){
	if(forceCounter == 0){
		for(int i = 0; i<numWalls; i++){
			curWallForces[i] = 0;
		}
	}
	forceCounter++;
	int x = (int)ceil((spheresCount-1)/2.0);
	
	#if 1
		if(G_fact == 0){
			//calcSortedBallResistance();
			//calcCellSortedBallResistance();
			
			countSpheresPerCell();
			collideSpheresPerCell();
		}else
	#endif
	{
		calcBallResistance();
	}
	
	if(wallResistance){
		calcWallResistance();
	}
	
	timeInterval = std::min((double)timeInterval, 1.0/minFps);
	sumUpForces();
	#if useSSE
		if(_3D_){
			for(int i = 0; i<spheresCount; i++){
				spheresPos[i](3) = 0;
			}
		}
		#if _double_==0
			else{
				for(int i = 0; i<spheresCount; i++){
					spheresPos[i](2) = 0;
					spheresPos[i](3) = 0;
				}
			}
		#endif
	#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateG(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateAirResistance(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateWallResistance(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateEModul(){
	_E = E*1000000.0;
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updatePoisson(){
	//nothing to do
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::updateElasticity(){
	//nothing to do
}

template <int dims, bool _3D_>
bool EigenCalculator_Engine<dims,_3D_>::saveFrame(){
	for(int i = 0; i<readNum_render; i++){
		renderBuffer[bufferWriteIndex][i] = spheresOldPos[i];
	}
	return true;
}

template <int dims, bool _3D_>
bool EigenCalculator_Engine<dims,_3D_>::isFixed(int i){
	return spheres[i].fixed == 1;
}

#define k_b 1.3806488E-23
template <int dims, bool _3D_>
scalar EigenCalculator_Engine<dims,_3D_>::getTemperature(){
	//1m : 65pm = 1.0E12 : 1
	//950kg/m^3 : 1.25kg/m^3 = 760 : 1
	const static double mass = 14*1.66E-27;
	double E_kin = 0;
	for(int i = 0; i<spheresCount; i++){
		E_kin += 0.5*mass*pow(spheresSpeed[i].norm(),2);
		//E_kin += 0.5*spheres[i].mass*pow(spheresSpeed[i].norm(),2);
	}
	//printf("E_kin: %5f\n", E_kin);
	double temp = 2.0/3.0 * E_kin/k_b;///(1.0E24*0.015*0.015);
	return temp;
}

template <int dims, bool _3D_>
Sphere* EigenCalculator_Engine<dims,_3D_>::getDirectSphere(int i){
	if(renderBuffer[bufferWriteIndex][i]==eVector::Zero()) return NULL;
	spheres[i].pos.s0 = renderBuffer[bufferWriteIndex][i](0);
	spheres[i].pos.s1 = renderBuffer[bufferWriteIndex][i](1);
	if(_3D_){
		spheres[i].pos.s2 = renderBuffer[bufferWriteIndex][i](2);
	}
	return &spheres[i];
}

template <int dims, bool _3D_>
Sphere* EigenCalculator_Engine<dims,_3D_>::getSphere(int i){
	if(renderBuffer[bufferReadIndex][i]==eVector::Zero()) return NULL;
	#if 0
		//"live" view
		spheres[i].pos.s0 = spheresPos[i](0);
		spheres[i].pos.s1 = spheresPos[i](1);
		if(_3D_){
			spheres[i].pos.s2 = spheresPos[i](2);
		}
	#else
		//normal view
		spheres[i].pos.s0 = renderBuffer[bufferReadIndex][i](0);
		spheres[i].pos.s1 = renderBuffer[bufferReadIndex][i](1);
		if(_3D_){
			spheres[i].pos.s2 = renderBuffer[bufferReadIndex][i](2);
		}
	#endif
	return &spheres[i];
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::fpsChanged(scalar timeInt){
	timeInterval = timeInt;
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::boxSizeChanged(){
	updateGridSize();
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::gravityChanged(){
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::sphereCountChanged_subclass(int i){
	///spheres
	spheres = newCopy<Sphere>(spheres, spheresCount, i);
	///spheresOldPos
	spheresOldPos = newCopy<eVector>(spheresOldPos, spheresCount, i);
	///spheresPos
	spheresPos = newCopy<eVector>(spheresPos, spheresCount, i);
	///spheresSpeed
	spheresSpeed = newCopy<eVector>(spheresSpeed, spheresCount, i);
	///spheresForce
	spheresForce = newCopy<eVector>(spheresForce, spheresCount, i);
	///gridIndex
	gridIndex = newCopy<int*>(gridIndex, spheresCount, i);
	///position indexes
	posX = newCopy<Pos>(posX, spheresCount, i);
	posY = newCopy<Pos>(posY, spheresCount, i);
	if(_3D_){
		posZ = newCopy<Pos>(posZ, spheresCount, i);
	}
	/*
	if(i<(spheresCount/2)){
		for(int j = i; j<spheresCount; j++){
			posX[posX[j].posOfSphere].sphereAtPos = posX[j].sphereAtPos;
			posX[posY[j].posOfSphere].sphereAtPos = posY[j].sphereAtPos;
			if(_3D_){
				posX[posZ[j].posOfSphere].sphereAtPos = posZ[j].sphereAtPos;
			}
		}
	}else // */
	{
		for(int j = 0; j<i; j++){
			posX[j].sphereAtPos = j;
			posX[j].posOfSphere = j;
			posY[j].sphereAtPos = j;
			posY[j].posOfSphere = j;
			if(_3D_){
				posZ[j].sphereAtPos = j;
				posZ[j].posOfSphere = j;
			}
		}
	}
	//parallelFor
	for(int j = spheresCount; j<i; j++){
		posX[j].sphereAtPos = j;
		posX[j].posOfSphere = j;
		posY[j].sphereAtPos = j;
		posY[j].posOfSphere = j;
		if(_3D_){
			posZ[j].sphereAtPos = j;
			posZ[j].posOfSphere = j;
		}
	}
	///renderBuffer
	int readNum_render_new = min(maxShowSpheresCount,i);
	if(readNum_render_new>readNum_render)
	{
		//parallelFor
		for(int j = 0; j<renderBufferCount; j++){
			renderBuffer[j] = newCopy<eVector>(renderBuffer[j], readNum_render, readNum_render_new);
		}
	}
	readNum_render = readNum_render_new;
	
	//parallelFor
	for(int j = spheresCount; j<i; j++){
		if(j<readNum_render){
			for(int k = bufferReadIndex; k!=bufferWriteIndex; k=(k+1)%renderBufferCount){
				renderBuffer[k][j] = eVector::Zero();
			}
		}
		
		initSphere(j);
	}
	//bufferReadIndex = (readNum_render+bufferWriteIndex-1)%readNum_render;
	///both_r
	both_r = newCopy<scalar*>(both_r, spheresCount, i);
	parallelFor
	for(int j = 0; j<i; j++){
		if(j<spheresCount){
			both_r[j] = newCopy<scalar>(both_r[j], spheresCount, i);
			for(int k = spheresCount; k<i; k++){
				both_r[j][k] = spheres[j].size+spheres[k].size;
			}
		}else{
			both_r[j] = new scalar[i];
			for(int k = 0; k<i; k++){
				both_r[j][k] = spheres[j].size+spheres[k].size;
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::maxSphereCountChanged_subclass(int i){
	//renderBuffer
	int readNum_render_new = min(i,spheresCount);
	if(readNum_render_new>readNum_render)
	{
		parallelFor
		for(int j = 0; j<renderBufferCount; j++){
			renderBuffer[j] = newCopy<eVector>(renderBuffer[j], readNum_render, readNum_render_new);
		}
	}
	
	parallelFor
	for(int j = readNum_render; j<readNum_render_new; j++){
		for(int k = bufferReadIndex; k!=bufferWriteIndex; k=(k+1)%renderBufferCount){
			renderBuffer[k][j] = eVector::Zero();
		}
	}
	readNum_render = readNum_render_new;
}


template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::loadConfig(const char* file){
	f2.open(file, std::fstream::in);
	bool b = true;
	if (f2.is_open())
	{
		//readLine();
		int _3d;
		saveInVar(_3d);
		
		//*
		if((_3d!=0) != _3D_){
			if(_3d == 0){
				std::cerr<<"You have to open this file with 2D viewer."<<std::endl;
			}else{
				std::cerr<<"You have to open this file with 3D viewer."<<std::endl;
			}
			return;
		}// */
		use3D = (_3d!=0);
		
		int newSpheresCount;
		saveInVar(newSpheresCount);
		int newMaxSpheresCount;
		saveInVar(newMaxSpheresCount);
		
		saveInVar(boxSize.s0);
		saveInVar(boxSize.s1);
		if(_3D_) saveInVar(boxSize.s2);
		saveInVar(sphereSize.s0);
		saveInVar(sphereSize.s1);
		saveInVar(renderFpsMax);
		timeInterval = 1000/renderFpsMax;
		saveInVar(renderFps);
		saveInVar(speed);
		saveInVar(fps);
		saveInVar(minFps);
		saveInVar(max_speed);
		saveInVar(E);
		saveInVar(poisson);
		saveInVar(elastic);
		
		saveInVar(gravity_abs);
		printf("gravity: %5f\n", gravity_abs);
		glWidget->updateGravity();
		
		saveInVar(G_fact);
		saveInVar(airResistance);
		int _wallResistance;
		saveInVar(_wallResistance);
		wallResistance = (_wallResistance != 0);
		
		
		
		sphereCountChanged(newSpheresCount);
		//spheresCount = newSpheresCount;

		maxSphereCountChanged(newMaxSpheresCount);
		//maxShowSpheresCount = newMaxSpheresCount;
		
		int fixed;
		for(int i = 0; i<newSpheresCount; i++){
			saveInVar(fixed);
			spheres[i].fixed = fixed;
			saveInVar(spheres[i].size);
			//printf("sphere size: %5f\n", spheres[i].size);
			saveInVar(spheres[i].mass);
			saveInVar(spheresPos[i](0));
			saveInVar(spheresPos[i](1));
			//if(fixed==0) 
			spheresPos[i](0) += 0.001*((rand()%1024)/1024.0);
			//if(fixed==0) 
			spheresPos[i](1) += 0.001*((rand()%1024)/1024.0);
			if(_3D_){
				saveInVar(spheresPos[i](2));
				//if(fixed==0) 
				spheresPos[i](2) += 0.001*((rand()%1024)/1024.0);
			}
			#if useSSE
				if(_3D_){
					spheresPos[i](3) = 0;
				}
				#if _double_==0
					else{
						spheresPos[i](2) = 0;
						spheresPos[i](3) = 0;
					}
				#endif
			#endif
			spheresOldPos[i] = spheresPos[i];
			//printf("sphere pos: %5f %5f\n", spheresPos[i](0), spheresPos[i](1));
			//printf("sphere oldPos: %5f %5f\n", spheresOldPos[i](0), spheresOldPos[i](1));
			saveInVar(spheresSpeed[i](0));
			saveInVar(spheresSpeed[i](1));
			if(_3D_){
				saveInVar(spheresSpeed[i](2));
			}
			#if useSSE
				if(_3D_){
					spheresSpeed[i](3) = 0;
				}
				#if _double_==0
					else{
						spheresSpeed[i](2) = 0;
						spheresSpeed[i](3) = 0;
					}
				#endif
			#endif
			//printf("sphere speed: %5f %5f\n", spheresSpeed[i](0), spheresSpeed[i](1));
			spheresForce[i].Zero();
		}
		
		for(int j = 0; j<spheresCount; j++){
			for(int k = 0; k<spheresCount; k++){
				both_r[j][k] = spheres[j].size+spheres[k].size;
				both_r[k][j] = spheres[j].size+spheres[k].size;
			}
		}
		
		initialized = true;
		
		bufferReadIndex = 0;
		bufferWriteIndex = 0;
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		
		printf("Spheres to render: %d\n", readNum_render);
		
	}
	else
	{
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		exit(0);
	}
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::loadConfig(){
	const char* file = (std::string(filename)+getConfigFileExtension()).c_str();
	
	const char* filter = (std::string("SphereSim View File (*.")+getConfigFileExtension()+")").c_str();
	QString str = QFileDialog::getOpenFileName(0, ("Open file"), (std::string("./save.")+getConfigFileExtension()).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		exit(0);
	}
	file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);
	
	loadConfig(file);
}

template <int dims, bool _3D_>
void EigenCalculator_Engine<dims,_3D_>::paintGL(bool b){
	Calculator::paintGL(b);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(0.5);
	
	for(double x = 0; x < boxSize.s0; x+=gridWidth){
		for(double y = 0; y < boxSize.s1; y+=gridWidth){
			glBegin(GL_LINE_STRIP);
			glVertex2d(x+gridWidth, y);
			glVertex2d(x+gridWidth, y+gridWidth);
			glVertex2d(x, y+gridWidth);
			glEnd();
		}
	}
	
	return;
	eVector pos;
	for(int i = 0; i<spheresCount; i++){
		//pos = renderBuffer[bufferReadIndex][i];
		pos = spheresPos[i];
		glWidget->renderText(pos[0], pos[1], pos[2], QString::number(	//posCell[i].posOfSphere));
																		cellOfSphere[i]));
	}
}

template class EigenCalculator_Engine<2,false>;
template class EigenCalculator_Engine<3,true>;

#endif
