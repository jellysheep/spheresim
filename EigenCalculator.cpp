#ifdef ENGINE_CPP

#include "EigenCalculator.h"

#include <iostream>
using namespace std;


#include <QFileDialog>

#include "NanosecondTimer.h"
#include "GLWidget.h"

#define parallelFor 
//_Pragma("omp parallel for if(circlesCount>500)")

#define fixSun 0

uint32_t calcZOrder(uint16_t xPos, uint16_t yPos);

EigenCalculator::EigenCalculator():Calculator(){
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
	circles = new Circle[circlesCount];
	circlesPos = new eVector[circlesCount];
	circlesOldPos = new eVector[circlesCount];
	circlesSpeed = new eVector[circlesCount];
	circlesForce = new eVector[circlesCount];
	both_r = new scalar*[circlesCount];
	
	numCells = min(pow_int(rowsPerStep, curveSteps), maxCellsPerAxis);
	updateGridSize();
	gridIndex = new int*[circlesCount];
	
	posX = new Pos[circlesCount];
	posY = new Pos[circlesCount];
	if(use3D){
		posZ = new Pos[circlesCount];
	}
	//parallelFor
	for(int i = 0; i<circlesCount; i++){
		posX[i].posOfCircle = i;
		posX[i].circleAtPos = i;
		posY[i].posOfCircle = i;
		posY[i].circleAtPos = i;
		if(use3D){
			posZ[i].posOfCircle = i;
			posZ[i].circleAtPos = i;
		}
	}
	
	///cellSorting
	
	firstSphereInCell = new int[numCells*numCells*(use3D?numCells:1)];
	memset(firstSphereInCell, 0, numCells*numCells*(use3D?numCells:1));
	posCell = new Pos[circlesCount];
	cellOfSphere = new int[circlesCount];
	
	curveIndices = new int[numCells*numCells*(use3D?numCells:1)];
	//buildCurveIndices_RowColumn();
	//buildCurveIndices_zOrder();
	//buildCurveIndices_Peano();
	buildCurveIndices_Hilbert();
	for(int y = numCells-1; y>=0; y--){
		for(int x = 0; x<numCells; x++){
			printf("%3d ", curveIndices[x+numCells*y]);
		}
		printf("\n");
	}
	
	//parallelFor
	for(int i = 0; i<circlesCount; i++){
		
		both_r[i] = new scalar[circlesCount];
		
		initCircle(i);
		
	}
	
	///cellSorting
	for(int i = 0; i<circlesCount; i++){
		posCell[i].circleAtPos = i;
		posCell[i].posOfCircle = i;
	}
	sortSpheresByCells();
	
	//parallelFor
	for(int i = 0; i<circlesCount; i++){
		for(int j = 0; j<circlesCount; j++){
			both_r[i][j] = circles[i].size + circles[j].size;
		}
	}
	if(renderBool){
		readNum_render = min(maxShowCirclesCount,circlesCount);
		printf("Circles to render: %d\n", readNum_render);
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
	printf("EigenCalculator initialized!\n");
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
void EigenCalculator::buildCurveIndices_Hilbert(){
	for(int y = 0; y<numCells; y++){
		for(int x = 0; x<numCells; x++){
			if(use3D){
				for(int z = 0; z<numCells; z++){
					curveIndices[x+numCells*y] = xy2d_Hilbert(numCells, x, y) + numCells*numCells*z;
				}
			}else{
				curveIndices[x+numCells*y] = xy2d_Hilbert(numCells, x, y);
			}
		}
	}
}

void EigenCalculator::buildCurveIndices_RowColumn(){
	for(int y = 0; y<numCells; y++){
		for(int x = 0; x<numCells; x++){
			if(use3D){
				for(int z = 0; z<numCells; z++){
					curveIndices[x+numCells*y] = x+numCells*y + numCells*numCells*z;
				}
			}else{
				curveIndices[x+numCells*y] = x+numCells*y;
			}
		}
	}
}

void EigenCalculator::buildCurveIndices_zOrder(){
	for(int y = 0; y<numCells; y++){
		for(int x = 0; x<numCells; x++){
			if(use3D){
				for(int z = 0; z<numCells; z++){
					curveIndices[x+numCells*y] = calcZOrder(x,y) + numCells*numCells*z;
				}
			}else{
				curveIndices[x+numCells*y] = calcZOrder(x,y);
			}
		}
	}
}

void EigenCalculator::buildCurveIndices_Peano(){
	indexCounter = 0;
	buildPeanoCurve(0, 0, 0, 0, 0);
}

void EigenCalculator::buildPeanoCurve(int x, int y, int z, int step, int direction){
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
		if(use3D){
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

void EigenCalculator::initCircle(int i){
	circles[i].size = rans(sphereSize.s0, sphereSize.s1);
	circles[i].E = E;
	circles[i].poisson = poisson;
	circles[i].fixed = 0;
	
	circlesPos[i].Zero();
	circlesPos[i](0) = circles[i].size+rans(boxSize.s0-2*circles[i].size);
	circlesPos[i](1) = circles[i].size+rans(boxSize.s1-2*circles[i].size);
	if(use3D){
		circlesPos[i](2) = circles[i].size+rans(boxSize.s2-2*circles[i].size);
	}else{
		circlesPos[i](2) = 0;
	}
	circlesOldPos[i] = circlesPos[i];
	
	gridIndex[i] = new int[3];
	gridIndex[i][0] = circlesPos[i](0)/gridWidth;
	gridIndex[i][1] = circlesPos[i](1)/gridWidth;
	if(use3D){
		gridIndex[i][2] = circlesPos[i](2)/gridWidth;
	}else{
		gridIndex[i][2] = 0;
	}
	
	circlesSpeed[i] = eVector::Random();
	#if useSSE
		circlesSpeed[i](3) = 0;
		if(!use3D){
			circlesSpeed[i](2) = 0;
		}
	#endif
	if(use3D){
		circlesSpeed[i](2) = 0;
	}
	circlesSpeed[i].normalize();
	circlesSpeed[i] *= rans(max_speed);
	//cout<<circlesSpeed[i]<<endl<<endl;
	
	circlesForce[i].Zero();
	
	//cout<<"3D enabled: "<<(use3D?1:0)<<endl<<endl;
	
	#if fixSun
		if(i == 0) circles[i].size = 3*sphereSize.s0;
		//if(i == 0) circles[i].mass*=5;
		if(i == 0) ceBuffer[i].hsvColor = QColor::fromHsv(0,0,0);
		if(i == 0) circlesPos[i](0) = boxSize.s0/2;
		if(i == 0) circlesPos[i](1) = boxSize.s1/2;
		if(use3D){
			if(i == 0) circlesPos[i](2) = boxSize.s2/2;
		}
		if(i == 0) circlesSpeed[i] = eVector::Zero();
	#endif
	circlesOldPos[i] = circlesPos[i];
	circles[i].mass = 4.0/3.0*pow(circles[i].size,3)*M_PI  *950; //Kautschuk
}

void EigenCalculator::updateGridSize(){
	gridWidth = max(2*sphereSize.s1, max(boxSize.s0, max(boxSize.s1, boxSize.s2))/numCells);
	printf("gridWidth: %5f \n", gridWidth);
	if(use3D){
		gridSteps = (int)(max(boxSize.s0, max(boxSize.s1, boxSize.s2))/gridWidth);
	}else{
		gridSteps = (int)(max(boxSize.s0, boxSize.s1)/gridWidth);
	}
	printf("Grid steps: %5d\n", gridSteps);
}


void EigenCalculator::save(){
	//not yet implemented
}

#define _G_ 1
#define newFor 0
#define joinFors 1

void EigenCalculator::calcWallResistance(){
	//parallelFor
	for(int i = 0; i<circlesCount; i++){
		if(circles[i].fixed == 1) continue;
		scalar d_, R, _E_;
		scalar force_, htw_d_d, fact = 1.0f;
		if ((htw_d_d = (circles[i].size - circlesPos[i](0)))>0) {
			//printf("Sphere %d collides with left wall\n", i);
			if(circlesSpeed[i](0) > 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			circlesForce[i](0) += force_*fact;
			curWallForces[0] += force_*fact;
		}else if ((htw_d_d = (circles[i].size + circlesPos[i](0) - boxSize.s0))>0) {
			//printf("Sphere %d collides with right wall\n", i);
			if(circlesSpeed[i](0) < 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			circlesForce[i](0) -= force_*fact;
			curWallForces[1] += force_*fact;
		}
		fact = 1.0f;
		if ((htw_d_d = (circles[i].size - circlesPos[i](1)))>0) {
			//printf("Sphere %d collides with bottom wall\n", i);
			if(circlesSpeed[i](1) > 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			circlesForce[i](1) += force_*fact;
			curWallForces[2] += force_*fact;
		}else if ((htw_d_d = (circles[i].size + circlesPos[i](1) - boxSize.s1))>0) {
			//printf("Sphere %d collides with top wall\n", i);
			if(circlesSpeed[i](1) < 0)fact = elastic;
			d_ = htw_d_d;
			R = circles[i].size;
			_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
			force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
			circlesForce[i](1) -= force_*fact;
			curWallForces[3] += force_*fact;
		}
		if(use3D){
			fact = 1.0f;
			if ((htw_d_d = (circles[i].size - circlesPos[i](2)))>0) {
				if(circlesSpeed[i](2) > 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				circlesForce[i](2) += force_*fact;
				curWallForces[4] += force_*fact;
			}else if ((htw_d_d = (circles[i].size + circlesPos[i](2) - boxSize.s2))>0) {
				if(circlesSpeed[i](2) < 0)fact = elastic;
				d_ = htw_d_d;
				R = circles[i].size;
				_E_ = 1/(((1-poisson*poisson)/_E)+((1-poisson*poisson)/_E));
				force_ = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3));
				circlesForce[i](2) -= force_*fact;
				curWallForces[5] += force_*fact;
			}
		}
	}
}

void EigenCalculator::calcBallResistance(){
	parallelFor
	for(int i = 0; i<circlesCount; i++){
		eVector force;
		
		eVector d_pos, d_n;
		scalar *both_r_ = &this->both_r[i][0], d, d_, R, _E_;
	#if newFor
		int j;
		for(int a = 0; a<x; a++){
			j = (i+a+1)%circlesCount;
	#else
		for(int j = i+1; j<circlesCount; j++){
	#endif
			
			//#define c2 circle[i]
			//both_r = circles[i].size + circles[j].size;
			//both_r = this->both_r[i][j];
			
			#if _G_
			if(G==0)
			#endif
			{
				if(abs(gridIndex[i][0]-gridIndex[j][0]) >1) continue;
				if(abs(gridIndex[i][1]-gridIndex[j][1]) >1) continue;
				if(use3D){
					if(abs(gridIndex[i][2]-gridIndex[j][2]) >1) continue;
				}
			}
			
			collideBalls(i,j);
		}
	}
}

void EigenCalculator::collideBalls(int i, int j){
	if(i == j) return;
	scalar both_r_ = both_r[i][j];
	eVector d_pos = circlesPos[j]-circlesPos[i];
	
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
		force = G*G_fact*circles[i].mass*circles[j].mass/pow(max(d,(scalar)circles[i].size/10),2) *d_n;
		circlesForce[i] += force;
		
		circlesForce[j] -= force;
	}
	#endif
	
	// Abstossung:
	if (d < both_r_) {
		//printf("Circles %d and %d are colliding!\n", i, j);
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
		R = 1/((1/circles[i].size)+(1/circles[j].size));
		//_E_ = 1/(((1-circles[i].poisson*circles[i].poisson)/(_E))+((1-circles[j].poisson*circles[j].poisson)/(_E)));
		_E_ = 1/(((1-(poisson*poisson))/(_E))+((1-(poisson*poisson))/(_E)));
		force = 4.0f/3.0f*_E_*sqrt(R*pow(d_,3)) *d_n;
		//printf("Stoss! r: %5f m: %5f d: %5f R: %5f E*: %5f F: %5f\n", circles[i].size, circles[i].mass, d_, R, _E_, force.norm());
		if(elastic!=1 && d_pos.dot((circlesSpeed[i]-circlesSpeed[j])/d)<0){ //Skalarprodukt
			force *= elastic;
		}
		#pragma atomic
			circlesForce[i] -= force;
		#pragma atomic
			circlesForce[j] += force;
	}
}

#define sqr(x) ((x)*(x))
void EigenCalculator::sumUpForces(){
	//parallelFor
	for(int i = 0; i<circlesCount; i++){
		if(circles[i].fixed == 0){
			eVector force;
			#if fixSun
				if(i==0)continue;
			#endif
			EIGEN_ASM_COMMENT("begin");
			circlesForce[i] -= airResistance*(0.47*M_PI*sqr(circles[i].size))*0.5*circlesSpeed[i]*std::abs(circlesSpeed[i].norm());
			force = circlesForce[i];
			/*printf("circle %d force: %5f %5f\n", i, circlesForce[i](0), circlesForce[i](1));
			printf("circle %d speed: %5f %5f\n", i, circlesSpeed[i](0), circlesSpeed[i](1));
			printf("circle %d pos: %5f %5f\n", i, circlesPos[i](0), circlesPos[i](1));
			printf("circle %d old pos: %5f %5f\n", i, circlesOldPos[i](0), circlesOldPos[i](1));
			printf("circle %d mass: %5f\n", i, circles[i].mass);
			QThread::msleep(10);// */
			eVector acceleration = force / circles[i].mass;
			acceleration(0) += gravity.s0;
			acceleration(1) += gravity.s1;
			//printf("circle %d gravity: %5f %5f\n", i, gravity.s0, gravity.s1);
			if(use3D){
				acceleration(2) += gravity.s2;
			}
			circlesOldPos[i] += circlesSpeed[i]*timeInterval;
			circlesOldPos[i] += 0.5*acceleration*timeInterval*timeInterval;
			
			circlesSpeed[i] += acceleration*timeInterval;
			circlesForce[i] -= force;
			
			#if 0
				if(wallResistance){
					circlesOldPos[i](0) = min(circlesOldPos[i](0), boxSize.s0);
					circlesOldPos[i](1) = min(circlesOldPos[i](1), boxSize.s1);
					if(use3D) circlesOldPos[i](2) = min(circlesOldPos[i](2), boxSize.s2);
					circlesOldPos[i](0) = max(circlesOldPos[i](0), 0.0);
					circlesOldPos[i](1) = max(circlesOldPos[i](1), 0.0);
					if(use3D) circlesOldPos[i](2) = max(circlesOldPos[i](2), 0.0);
				}
			#endif
		}
		circlesPos[i] = circlesOldPos[i];
		circlesPos[i] += circlesSpeed[i]*(timeInterval/2);
		//circlesPos[i] += 0.5*acceleration*(timeInterval/2)*(timeInterval/2);
		
		gridIndex[i][0] = (int)(circlesPos[i](0)/gridWidth);
		gridIndex[i][1] = (int)(circlesPos[i](1)/gridWidth);
		if(use3D){
			gridIndex[i][2] = circlesPos[i](2)/gridWidth;
		}
		/*printf("circle %d speed: %5f %5f\n", i, circlesSpeed[i](0), circlesSpeed[i](1));
		printf("circle %d pos: %5f %5f\n", i, circlesPos[i](0), circlesPos[i](1));
		printf("circle %d old pos: %5f %5f\n", i, circlesOldPos[i](0), circlesOldPos[i](1));// */
		
		EIGEN_ASM_COMMENT("end");
	}
}

void EigenCalculator::sort(Pos* p, int dim){
	//scalar temp;
	int temp;
	int j, cid; //circle ID

	for (int i=1; i < circlesCount; i++){
		cid = p[i].circleAtPos;
		//temp = circlesPos[cid](dim);
		temp = gridIndex[cid][dim];
		j = i-1;

		//while (j >= 0 && circlesPos[p[j].circleAtPos](dim) > temp){
		while (j >= 0 && gridIndex[p[j].circleAtPos][dim] > temp){
			p[j+1].circleAtPos = p[j].circleAtPos;
			j--;
		}

		p[j+1].circleAtPos = cid;
	}
}

void EigenCalculator::calcSortedBallResistance(){
	sort(posX, 0);
	/*
	for(int pid = 0; pid<circlesCount; pid++){ // pos. ID
		printf("%3d ", posX[pid].circleAtPos);
	}
	printf("\n"); // */

	/// Kugeln kollidieren nur mit anderen, die rechts davon liegen.
	/// Dadurch werden doppelt berechnete Kollisionen vermieden.
	
	parallelFor
	for(int pid = 0; pid<circlesCount; pid++){ // pos. ID
		int i = posX[pid].circleAtPos, j; //circle IDs
		posX[i].posOfCircle = pid;
		for(int pid2 = pid+1; pid2<circlesCount; pid2++){
			j = posX[pid2].circleAtPos;
			if((gridIndex[j][0] <= gridIndex[i][0]+1)){
			//if(abs(gridIndex[i][0]-gridIndex[j][0]) <=1){
			//if(circlesPos[j][0]-circlesPos[i][0] < both_r[i][j]){
				if(abs(gridIndex[i][1]-gridIndex[j][1]) <=1){
					if((!use3D) || (abs(gridIndex[i][2]-gridIndex[j][2]) <=1)){
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

int EigenCalculator::calcCellID(int x, int y, int z){
	x = min(numCells-1, x);
	y = min(numCells-1, y);
	z = min(numCells-1, z);
	x = max(0, x);
	y = max(0, y);
	z = max(0, z);
	return curveIndices[x+numCells*y+(use3D?numCells*numCells*z:0)];
	//return x + numCells*y + numCells*numCells*z;
	//return calcZOrder(x,y) + numCells*numCells*z;
}

void EigenCalculator::sortSpheresByCells(){
	if(use3D){
		for(int i = 0; i<circlesCount; i++){
			//calculate cell ID
			cellOfSphere[i] = calcCellID(gridIndex[i][0], gridIndex[i][1], gridIndex[i][1]);
		}
	}else{
		for(int i = 0; i<circlesCount; i++){
			//calculate cell ID
			cellOfSphere[i] = calcCellID(gridIndex[i][0], gridIndex[i][1]);
			//printf
		}
	}
	
	int temp;
	int j, cid; //circle ID
	for (int i=1; i < circlesCount; i++){
		cid = posCell[i].circleAtPos;
		temp = cellOfSphere[cid];
		j = i-1;

		//while (j >= 0 && circlesPos[p[j].circleAtPos](dim) > temp){
		while (j >= 0 && cellOfSphere[posCell[j].circleAtPos] > temp){
			posCell[j+1].circleAtPos = posCell[j].circleAtPos;
			j--;
		}

		posCell[j+1].circleAtPos = cid;
	}
	
	//save array index of each sphere
	for(int i = 0; i<circlesCount; i++){
		posCell[posCell[i].circleAtPos].posOfCircle = i;
	}
	
	/*
	//set first sphere of each cell to -1
	for(int i = boxSize.s0/gridWidth + 1; i>=0; i--){
		for(j = boxSize.s1/gridWidth + 1; j>=0; j--){
			if(use3D){
				for(int k = boxSize.s2/gridWidth; k>=0; k--){
					firstSphereInCell[i + numCells*j + numCells*numCells*k] = -1;
				}
			}else{
				firstSphereInCell[i + numCells*j] = -1;
			}
		}
	}// */
	
	//find posCell array position of first sphere of each cell
	for(int i = circlesCount-1; i>=0; i--){
		firstSphereInCell[cellOfSphere[posCell[i].circleAtPos]] = i;
	}
	/*for(int i = 0; i<circlesCount; i++){
		printf("Circle: %2d cellID: %2d first circle in cell: %2d\n", posCell[i].circleAtPos, cellOfSphere[posCell[i].circleAtPos], posCell[firstSphereInCell[cellOfSphere[posCell[i].circleAtPos]]].circleAtPos);
	}// */
}

#define twiceCalcCollisions 1

void EigenCalculator::checkCollision(int i, int x, int y, int z, bool sameCell){ 
	//circle ID, cell position, bool if same cell (i.e. dx = dy = dz = 0)
	if(x<0 || y<0 || z<0 || x>=numCells || y>=numCells || z>=numCells){
		//if(x>=numCells || y>=numCells) printf("return because out of bounds\n");
		return;
	}
	int cellID = calcCellID(x,y,z);
	int j = firstSphereInCell[cellID]; //circle 2 pos in array
	if(j < 0){
		//printf("empty cell: %2d (%2d)\n", cellID, j);
		return;
	}
	int c2 = posCell[j].circleAtPos; //circle 2 ID
	//printf("circle 2 ID: %3d\n", c2);
	//if(c2>circlesCount) printf("cell: %2d circle 2 ID: %2d cell of circle before: %2d cell of next circle: %2d\n", cellID, c2, cellOfSphere[posCell[j-1].circleAtPos], cellOfSphere[posCell[j+1].circleAtPos]);
	//printf("cell: %2d | %2d cell of circle before: %2d cell of next circle: %2d\n", cellID, cellOfSphere[posCell[j].circleAtPos], cellOfSphere[posCell[j-1].circleAtPos], cellOfSphere[posCell[j+1].circleAtPos]);
	while(cellOfSphere[c2] == cellID){
		#if twiceCalcCollisions==0
			if(!(sameCell && circlesOldPos[i][0]>circlesOldPos[c2][0]))
		#endif
		{
			//if(circlesOldPos[i][0]>circlesOldPos[c2][0]) 
			if(i>c2) 
			collideBalls(i, c2);
		}
		j++;
		//if(j>=circlesCount) return;
		c2 = posCell[j].circleAtPos;
		//if(c2>=circlesCount || c2<0) return;
	}; //check collisions of all spheres in that cell
}

void EigenCalculator::calcCellSortedBallResistance(){
	//QThread::msleep(50);
	sortSpheresByCells();
	
	/*
	for(int pid = 0; pid<circlesCount; pid++){ // pos. ID
		printf("%3d ", posX[pid].circleAtPos);
	}
	printf("\n"); // */

	/// Kugeln kollidieren nur mit anderen, die in Zellen rechts, oben, vorne davon liegen.
	/// Dadurch werden doppelt berechnete Kollisionen vermieden.
	
	if(use3D){
		parallelFor
		for(int i = 0; i<circlesCount; i++){ //circle ID 1
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
		for(int i = 0; i<circlesCount; i++){ //circle ID 1
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

void EigenCalculator::doStep(){
	if(forceCounter == 0){
		for(int i = 0; i<numWalls; i++){
			curWallForces[i] = 0;
		}
	}
	forceCounter++;
	int x = (int)ceil((circlesCount-1)/2.0);
	
	#if 1
		if(G_fact == 0){
			//calcSortedBallResistance();
			calcCellSortedBallResistance();
		}else
	#endif
	{
		calcBallResistance();
	}
	
	if(wallResistance){
		calcWallResistance();
	}
	
	timeInterval = std::min(timeInterval, 1.0/minFps);
	sumUpForces();
	if(!use3D){
		for(int i = 0; i<circlesCount; i++){
			circlesPos[i](2) = 0;
			circlesPos[i](3) = 0;
		}
	}else{
		for(int i = 0; i<circlesCount; i++){
			circlesPos[i](3) = 0;
		}
	}
}

void EigenCalculator::updateG(){
	//nothing to do
}

void EigenCalculator::updateAirResistance(){
	//nothing to do
}

void EigenCalculator::updateWallResistance(){
	//nothing to do
}

void EigenCalculator::updateEModul(){
	_E = E*1000000.0;
}

void EigenCalculator::updatePoisson(){
	//nothing to do
}

void EigenCalculator::updateElasticity(){
	//nothing to do
}

bool EigenCalculator::saveFrame(){
	for(int i = 0; i<readNum_render; i++){
		renderBuffer[bufferWriteIndex][i] = circlesOldPos[i];
	}
	return true;
}

bool EigenCalculator::isFixed(int i){
	return circles[i].fixed == 1;
}

#define k_b 1.3806488E-23
scalar EigenCalculator::getTemperature(){
	//1m : 65pm = 1.0E12 : 1
	//950kg/m^3 : 1.25kg/m^3 = 760 : 1
	const static double mass = 14*1.66E-27;
	double E_kin = 0;
	for(int i = 0; i<circlesCount; i++){
		E_kin += 0.5*mass*pow(circlesSpeed[i].norm(),2);
		//E_kin += 0.5*circles[i].mass*pow(circlesSpeed[i].norm(),2);
	}
	//printf("E_kin: %5f\n", E_kin);
	double temp = 2.0/3.0 * E_kin/k_b;///(1.0E24*0.015*0.015);
	return temp;
}

Circle* EigenCalculator::getDirectCircle(int i){
	if(renderBuffer[bufferWriteIndex][i]==eVector::Zero()) return NULL;
	circles[i].pos.s0 = renderBuffer[bufferWriteIndex][i](0);
	circles[i].pos.s1 = renderBuffer[bufferWriteIndex][i](1);
	if(use3D){
		circles[i].pos.s2 = renderBuffer[bufferWriteIndex][i](2);
	}
	return &circles[i];
}

Circle* EigenCalculator::getCircle(int i){
	if(renderBuffer[bufferReadIndex][i]==eVector::Zero()) return NULL;
	#if 0
		//"live" view
		circles[i].pos.s0 = circlesPos[i](0);
		circles[i].pos.s1 = circlesPos[i](1);
		if(use3D){
			circles[i].pos.s2 = circlesPos[i](2);
		}
	#else
		//normal view
		circles[i].pos.s0 = renderBuffer[bufferReadIndex][i](0);
		circles[i].pos.s1 = renderBuffer[bufferReadIndex][i](1);
		if(use3D){
			circles[i].pos.s2 = renderBuffer[bufferReadIndex][i](2);
		}
	#endif
	return &circles[i];
}

void EigenCalculator::fpsChanged(scalar timeInt){
	timeInterval = timeInt;
}

void EigenCalculator::boxSizeChanged(){
	updateGridSize();
}

void EigenCalculator::gravityChanged(){
}

void EigenCalculator::circleCountChanged_subclass(int i){
	///circles
	circles = newCopy<Circle>(circles, circlesCount, i);
	///circlesOldPos
	circlesOldPos = newCopy<eVector>(circlesOldPos, circlesCount, i);
	///circlesPos
	circlesPos = newCopy<eVector>(circlesPos, circlesCount, i);
	///circlesSpeed
	circlesSpeed = newCopy<eVector>(circlesSpeed, circlesCount, i);
	///circlesForce
	circlesForce = newCopy<eVector>(circlesForce, circlesCount, i);
	///gridIndex
	gridIndex = newCopy<int*>(gridIndex, circlesCount, i);
	///position indexes
	posX = newCopy<Pos>(posX, circlesCount, i);
	posY = newCopy<Pos>(posY, circlesCount, i);
	if(use3D){
		posZ = newCopy<Pos>(posZ, circlesCount, i);
	}
	/*
	if(i<(circlesCount/2)){
		for(int j = i; j<circlesCount; j++){
			posX[posX[j].posOfCircle].circleAtPos = posX[j].circleAtPos;
			posX[posY[j].posOfCircle].circleAtPos = posY[j].circleAtPos;
			if(use3D){
				posX[posZ[j].posOfCircle].circleAtPos = posZ[j].circleAtPos;
			}
		}
	}else // */
	{
		for(int j = 0; j<i; j++){
			posX[j].circleAtPos = j;
			posX[j].posOfCircle = j;
			posY[j].circleAtPos = j;
			posY[j].posOfCircle = j;
			if(use3D){
				posZ[j].circleAtPos = j;
				posZ[j].posOfCircle = j;
			}
		}
	}
	//parallelFor
	for(int j = circlesCount; j<i; j++){
		posX[j].circleAtPos = j;
		posX[j].posOfCircle = j;
		posY[j].circleAtPos = j;
		posY[j].posOfCircle = j;
		if(use3D){
			posZ[j].circleAtPos = j;
			posZ[j].posOfCircle = j;
		}
	}
	///renderBuffer
	int readNum_render_new = min(maxShowCirclesCount,i);
	if(readNum_render_new>readNum_render)
	{
		//parallelFor
		for(int j = 0; j<renderBufferCount; j++){
			renderBuffer[j] = newCopy<eVector>(renderBuffer[j], readNum_render, readNum_render_new);
		}
	}
	readNum_render = readNum_render_new;
	
	//parallelFor
	for(int j = circlesCount; j<i; j++){
		if(j<readNum_render){
			for(int k = bufferReadIndex; k!=bufferWriteIndex; k=(k+1)%renderBufferCount){
				renderBuffer[k][j] = eVector::Zero();
			}
		}
		
		initCircle(j);
	}
	//bufferReadIndex = (readNum_render+bufferWriteIndex-1)%readNum_render;
	///both_r
	both_r = newCopy<scalar*>(both_r, circlesCount, i);
	parallelFor
	for(int j = 0; j<i; j++){
		if(j<circlesCount){
			both_r[j] = newCopy<scalar>(both_r[j], circlesCount, i);
			for(int k = circlesCount; k<i; k++){
				both_r[j][k] = circles[j].size+circles[k].size;
			}
		}else{
			both_r[j] = new scalar[i];
			for(int k = 0; k<i; k++){
				both_r[j][k] = circles[j].size+circles[k].size;
			}
		}
	}
}

void EigenCalculator::maxCircleCountChanged_subclass(int i){
	//renderBuffer
	int readNum_render_new = min(i,circlesCount);
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


void EigenCalculator::loadConfig(const char* file){
	f2.open(file, std::fstream::in);
	bool b = true;
	if (f2.is_open())
	{
		//readLine();
		int _3d;
		saveInVar(_3d);
		/*if((_3d!=0) != use3D){
			if(_3d == 0){
				std::cerr<<"You have to open this file with 2D viewer."<<std::endl;
			}else{
				std::cerr<<"You have to open this file with 3D viewer."<<std::endl;
			}
			return;
		}*/
		use3D = (_3d!=0);
		
		int newCirclesCount;
		saveInVar(newCirclesCount);
		int newMaxCirclesCount;
		saveInVar(newMaxCirclesCount);
		
		saveInVar(boxSize.s0);
		saveInVar(boxSize.s1);
		if(use3D) saveInVar(boxSize.s2);
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
		
		
		
		circleCountChanged(newCirclesCount);
		//circlesCount = newCirclesCount;

		maxCircleCountChanged(newMaxCirclesCount);
		//maxShowCirclesCount = newMaxCirclesCount;
		
		int fixed;
		for(int i = 0; i<newCirclesCount; i++){
			saveInVar(fixed);
			circles[i].fixed = fixed;
			saveInVar(circles[i].size);
			//printf("circle size: %5f\n", circles[i].size);
			saveInVar(circles[i].mass);
			saveInVar(circlesPos[i](0));
			saveInVar(circlesPos[i](1));
			//if(fixed==0) 
			circlesPos[i](0) += 0.001*((rand()%1024)/1024.0);
			//if(fixed==0) 
			circlesPos[i](1) += 0.001*((rand()%1024)/1024.0);
			if(use3D){
				saveInVar(circlesPos[i](2));
				//if(fixed==0) 
				circlesPos[i](2) += 0.001*((rand()%1024)/1024.0);
			}else{
				circlesPos[i](2) = 0;
			}
			circlesOldPos[i] = circlesPos[i];
			//printf("circle pos: %5f %5f\n", circlesPos[i](0), circlesPos[i](1));
			//printf("circle oldPos: %5f %5f\n", circlesOldPos[i](0), circlesOldPos[i](1));
			saveInVar(circlesSpeed[i](0));
			saveInVar(circlesSpeed[i](1));
			if(use3D){
				saveInVar(circlesSpeed[i](2));
			}else{
				circlesSpeed[i](2) = 0;
			}
			//printf("circle speed: %5f %5f\n", circlesSpeed[i](0), circlesSpeed[i](1));
			circlesForce[i].Zero();
		}
		
		for(int j = 0; j<circlesCount; j++){
			for(int k = 0; k<circlesCount; k++){
				both_r[j][k] = circles[j].size+circles[k].size;
				both_r[k][j] = circles[j].size+circles[k].size;
			}
		}
		
		initialized = true;
		
		bufferReadIndex = 0;
		bufferWriteIndex = 0;
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		
		printf("Circles to render: %d\n", readNum_render);
		
	}
	else
	{
		std::cerr<<"File could not be opened!"<<std::endl;
		circlesCount = 0;
		exit(0);
	}
}

void EigenCalculator::loadConfig(){
	const char* file = (std::string(filename)+getConfigFileExtension()).c_str();
	
	const char* filter = (std::string("SphereSim View File (*.")+getConfigFileExtension()+")").c_str();
	QString str = QFileDialog::getOpenFileName(0, ("Open file"), (std::string("./save.")+getConfigFileExtension()).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		circlesCount = 0;
		exit(0);
	}
	file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);
	
	loadConfig(file);
}

void EigenCalculator::paintGL(bool b){
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
	for(int i = 0; i<circlesCount; i++){
		//pos = renderBuffer[bufferReadIndex][i];
		pos = circlesPos[i];
		glWidget->renderText(pos[0], pos[1], pos[2], QString::number(	//posCell[i].posOfCircle));
																		cellOfSphere[i]));
	}
}

#endif
