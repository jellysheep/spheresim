#define _3D_ x
#define _double_ x
#define _G_ 1
#define _v_nicht_const_ 1
#define heun 1
#define step (5.0f/10.0f)
// Heun-Verfahren / Halbschritt-Verfahren

#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#pragma OPENCL EXTENSION cl_amd_vec3 : enable

//#define reduced 0.9999999999999
//0.9

#if _3D_
	#define double_vector double3
	#define float_vector float3
#else
	#define double_vector double2
	#define float_vector float2
#endif

#if _double_
	#define scalar double
	#define vector double_vector
	#define vector3 double3
	#define vector2 double2
#else
	#define scalar float
	#define vector float_vector
	#define vector3 float3
	#define vector2 float2
#endif

#define sqrt_(x) (sqrt(x))
#define pow_(x,y) (pow((scalar)(x),(scalar)(y)))
//sqrt half_sqrt native_sqrt

typedef struct Circle
{
    vector pos, oldPos;
    vector speed;
    vector force;
    scalar size,mass,poisson,E;
    //float x[3];  //padding
} Circle;

uint GetUint(uint* m_z, uint* m_w, uint gid)
{
	uint z = *m_z;
    z = 36969 * (z & 65535) + (z >> 16);
    *m_z = z;
    uint w = *m_w;
    w = 18000 * (w & 65535) + (w >> 16);
    *m_w = w;
    return (z << 16) + w;
}
uint gGetUint(__global uint* m_z, __global uint* m_w, uint gid)
{
	uint z = *m_z;
    z = 36969 * (z & 65535) + (z >> 16);
    *m_z = z;
    uint w = *m_w;
    w = 18000 * (w & 65535) + (w >> 16);
    *m_w = w;
    return (z << 16) + w;
}

scalar uGetUniform(uint* m_z, uint* m_w, uint gid)
{
    // 0 <= u < 2^32
    uint u = GetUint(m_z, m_w, gid);
    // The magic number below is 1/(2^32 + 2).
    // The result is strictly between 0 and 1.
    return (u + 1.0) //* 2.328306435454494e-10;
		* 2328306435454494 * pow_(10.0,-25.0);
}

scalar GetUniform(uint* m_z, uint* m_w, uint gid)
{
    // 0 <= u < 2^32
    uint u = GetUint(m_z, m_w, gid);
    // The magic number below is 1/(2^32 + 2).
    // The result is strictly between -1 and 1.
    return ((u + 1.0) //* 2.328306435454494e-10
		* 2328306435454494 * pow_(10.0,-25.0) 
		* 2)-1;
}


__kernel void randomFill(__global struct Circle* circle, __global uint* z, __global uint* w,
						__global vector3* boxSize, __global scalar* max_speed,
						__global vector* size, __global scalar* poisson,
						__global scalar* E, __global uint* num){
    int gid = get_global_id(0);
    vector3 s = *boxSize;
    vector s2 = *size;
    uint m_z_ = 0;//(*z) + (gid*s.s0*1000000);
    uint m_w_ = 0;//(*w) + (gid*s2.s0*100000000);
    uint n = *num;
    for(int i = 0; i<n; i++){
		if(i == gid){
			m_z_ = gGetUint(z,w,gid);
			m_w_ = gGetUint(z,w,gid);
		}
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
    uint* m_z = &m_z_;
    uint* m_w = &m_w_;
	circle[gid].size = s2.s0+((s2.s1-s2.s0)*uGetUniform(m_z, m_w, gid));
	circle[gid].mass = 4.0/3.0*pow_(circle[gid].size,3)*M_PI  *950; //Kautschuk
	circle[gid].poisson = *poisson;
	circle[gid].E = *E;
	    
	circle[gid].pos.s0 = circle[gid].size/2+uGetUniform(m_z, m_w, gid)*(s.s0-circle[gid].size);
	circle[gid].pos.s1 = circle[gid].size/2+uGetUniform(m_z, m_w, gid)*(s.s1-circle[gid].size);
	#if _3D_
		circle[gid].pos.s2 = circle[gid].size/2+uGetUniform(m_z, m_w, gid)*(s.s2-circle[gid].size);
		//printf("pos: %f\n", circle[gid].pos.s2);
	#endif
	circle[gid].oldPos = circle[gid].pos;
	
	circle[gid].speed.s0 = GetUniform(m_z, m_w, gid);
	circle[gid].speed.s1 = GetUniform(m_z, m_w, gid);
	#if _3D_
		circle[gid].speed.s2 = GetUniform(m_z, m_w, gid);
	#endif
	circle[gid].speed = normalize(circle[gid].speed)*uGetUniform(m_z, m_w, gid)*(*max_speed);
	
	circle[gid].force.s0 = 0;
	circle[gid].force.s1 = 0;
	#if _3D_
		circle[gid].force.s2 = 0;
	#endif
}

__kernel void moveStep(__global struct Circle* circle, __global int* num,
						__global vector* size,
						__global scalar* elastic, __global scalar* g,
						__global scalar* delta_t_, __global scalar* G_)
{
	vector s = *size, force, acceleration;
	scalar reduced = *elastic, gravity = *g, delta_t = *delta_t_, G = *G_;
	int id = get_global_id(0);
	struct Circle c2;
	
	struct Circle c = circle[id];
	#if heun
		c.pos += c.speed*(delta_t*step) + c.force/c.mass*(delta_t*step*delta_t*step)/2.0f;
	#endif
    
    vector d_pos, d_n;
	scalar both_r, d, d_, R, E_, force_;
	//*
	int n = *num, n2 = 0;
	for(int i = n-1; i>id; i--){
		c2 = circle[i];
		#if heun
			c2.pos += c2.speed*(delta_t*step) + c2.force/c2.mass*(delta_t*step*delta_t*step)/2.0f;
		#endif
		
		//#define c2 circle[i]
		both_r = c.size + c2.size;
		d_pos = c2.pos-c.pos;
		
		/*
		if(gravity == 0){
			if(d_pos.s0>both_r||d_pos.s0<-both_r||d_pos.s1>both_r||d_pos.s1<-both_r
			#if _3D_
			||d_pos.s2>both_r||d_pos.s2<-both_r
			#endif
			){
				continue;
			}
			// *
			else{
				if(++n2>4) break;
			}// * /
		}// */

		//d_pos.s2 = 0;
		d = length(d_pos);
		
		if(gravity!=0){
			d_n = d_pos/d; // bzw. normalize(d_pos);
			// Gravitation:
			force = G*c.mass*c2.mass/pow_(d,2) *d_n;
			circle[id].force += force;
			circle[i].force -= force;
		}
		
		// Abstossung:
		if (d < both_r) {
			if(gravity == 0){
				d_n = d_pos/d; // bzw. normalize(d_pos);
			}
			// nach Kontaktmechanik mit Poisson-Zahlen:
			d_ = both_r - d;
			R = 1/((1/c.size)+(1/c2.size));
			E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c2.poisson*c2.poisson)/c2.E));
			force = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3)) *d_n;
			if(dot(d_pos, (c.speed-c2.speed)/d)<0){ //Skalarprodukt
				force *= reduced;
			}
			circle[id].force -= force;
			circle[i].force += force;
			//continue;
			
			/*
			//c->thisStepHit = true;
			//if(!(c_.free && c2->free))continue;
			d_d = (both_r/d-1)*e*d_pos;
			// *
			if(dot(d_pos, (c.speed-c2.speed)/d)<0){ //Skalarprodukt
				d_d *= reduced;
				//circle[id].speed = 0;
				//circle[i].speed = 0;
			}// * /
			circle[id].force -= c.size*d_d;
			circle[i].force += c2.size*d_d;
			
			/ *
			htc_d = sqrt(htc_dSqr);
			htc_d_d = htc_both_r-htc_d;
			htc_f_1 = E*c_.r*c_.r*htc_d_d/(2*c_.r)/1000000.0;
			htc_f_2 = E*c2->r*c2->r*htc_d_d/(2*c2->r)/1000000.0;
			c->fx = c_.fx-htc_dx/htc_d*htc_f_1;
			c->fy = c_.fy-htc_dy/htc_d*htc_f_1;
			c2->fx += htc_dx/htc_d*htc_f_2;
			c2->fy += htc_dy/htc_d*htc_f_2;// */
		}
	}// */
    
    //*
    scalar htw_d_d, fact = 1.0f;
    if ((htw_d_d = (c.size - c.pos.s0))>0) {
		//c.speed.s0 = fabs(c.speed.s0);
		if(c.speed.s0 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s0 += force_*fact;
		//circle[id].force.s0 += c.size*htw_d_d*c.E*fact;
	}else if ((htw_d_d = (c.size + c.pos.s0 - s.s0))>0) {
		//c.speed.s0 = -fabs(c.speed.s0);
		if(c.speed.s0 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s0 -= force_*fact;
		//circle[id].force.s0 -= c.size*htw_d_d*c.E*fact;
	}
	fact = 1.0f;
    if ((htw_d_d = (c.size - c.pos.s1))>0) {
		//c.speed.s1 = fabs(c.speed.s1);
		if(c.speed.s1 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s1 += force_*fact;
		//circle[id].force.s1 += c.size*htw_d_d*c.E*fact;
	}else if ((htw_d_d = (c.size + c.pos.s1 - s.s1))>0) {
		//c.speed.s1 = -fabs(c.speed.s1);
		if(c.speed.s1 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s1 -= force_*fact;
		//circle[id].force.s1 -= c.size*htw_d_d*c.E*fact;
	}
#if _3D_
	fact = 1.0f;
    if ((htw_d_d = (c.size - c.pos.s2))>0) {
		if(c.speed.s2 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s2 += force_*fact;
		//circle[id].force.s2 += c.size*htw_d_d*c.E*fact;
	}else if ((htw_d_d = (c.size + c.pos.s2 - s.s2))>0) {
		if(c.speed.s2 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s2 -= force_*fact;
		//circle[id].force.s2 -= c.size*htw_d_d*c.E*fact;
	}
#endif
	// */
	
	// Luftwiderstand:
    //circle[id].force -= 0.001*circle[id].speed*fabs(length(circle[id].speed));
    //0.00001*normalize(circle[id].speed)*pow_(length(circle[id].speed),2);
    
    /*circle[id].pos += c.speed;
    force = circle[id].force;
    circle[id].speed += force;
    circle[id].force -= force;*/
    
    force = circle[id].force;
	
	
	// Gravitation nach unten:
	//circle[id].force.s1 -= gravity*circle[id].mass;
    acceleration = force / circle[id].mass;
	acceleration.s1 -= gravity;
	
    circle[id].pos += c.speed*delta_t;
    #if _v_nicht_const_
		circle[id].pos += acceleration*(delta_t*delta_t)/2.0f;
	#endif
    circle[id].speed += acceleration*delta_t;
    circle[id].force -= force;
}

__kernel void moveStep2(__global struct Circle* circle, __global int* num,
						__global vector* size,
						__global scalar* elastic, __global scalar* g,
						__global scalar* delta_t_, __global scalar* G_)
{
	vector s = *size, force, acceleration, pos, pos2;
	scalar reduced = *elastic, gravity = *g, delta_t = *delta_t_, G = *G_;
	int id = get_global_id(0);
	__global struct Circle* c2;
	
	__global struct Circle* c = &circle[id];
	#if heun
		pos = c->pos + c->speed*(delta_t*step) + c->force/c->mass*(delta_t*step*delta_t*step)/2.0f;
	#endif
    
    vector d_pos, d_n;
	scalar both_r, d, d_, R, E_, force_;
	//*
	int n = *num, n2 = 0;
	for(int i = n-1; i>id; i--){
		c2 = &circle[i];
		#if heun
			pos2 = c2->pos + c2->speed*(delta_t*step) + c2->force/c2->mass*(delta_t*step*delta_t*step)/2.0f;
		#endif
		
		//#define c2 circle[i]
		both_r = c->size + c2->size;
		d_pos = pos2-pos;
		
		/*
		if(G == 0){
			if(d_pos.s0>both_r||d_pos.s0<-both_r||d_pos.s1>both_r||d_pos.s1<-both_r
			#if _3D_
			||d_pos.s2>both_r||d_pos.s2<-both_r
			#endif
			){
				continue;
			}
			//*
			else{
				if(++n2>6) break;
			}// * /
		}// */

		//d_pos.s2 = 0;
		d = length(d_pos);
		
		if(G!=0){
			d_n = d_pos/d; // bzw. normalize(d_pos);
			// Gravitation:
			force = G*c->mass*c2->mass/pow_(d,2) *d_n;
			circle[id].force += force;
			circle[i].force -= force;
		}
		
		// Abstossung:
		if (d < both_r) {
			if(G == 0){
				d_n = d_pos/d; // bzw. normalize(d_pos);
			}
			// nach Kontaktmechanik mit Poisson-Zahlen:
			d_ = both_r - d;
			R = 1/((1/c->size)+(1/c2->size));
			E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c2->poisson*c2->poisson)/c2->E));
			force = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3)) *d_n;
			if(dot(d_pos, (c->speed-c2->speed)/d)<0){ //Skalarprodukt
				force *= reduced;
			}
			circle[id].force -= force;
			circle[i].force += force;
			//continue;
			
			/*
			//c->thisStepHit = true;
			//if(!(c_.free && c2->free))continue;
			d_d = (both_r/d-1)*e*d_pos;
			// *
			if(dot(d_pos, (c->speed-c2->speed)/d)<0){ //Skalarprodukt
				d_d *= reduced;
				//circle[id].speed = 0;
				//circle[i].speed = 0;
			}// * /
			circle[id].force -= c->size*d_d;
			circle[i].force += c2->size*d_d;
			
			/ *
			htc_d = sqrt(htc_dSqr);
			htc_d_d = htc_both_r-htc_d;
			htc_f_1 = E*c_.r*c_.r*htc_d_d/(2*c_.r)/1000000.0;
			htc_f_2 = E*c2->r*c2->r*htc_d_d/(2*c2->r)/1000000.0;
			c->fx = c_.fx-htc_dx/htc_d*htc_f_1;
			c->fy = c_.fy-htc_dy/htc_d*htc_f_1;
			c2->fx += htc_dx/htc_d*htc_f_2;
			c2->fy += htc_dy/htc_d*htc_f_2;// */
		}
	}// */
    
    //*
    scalar htw_d_d, fact = 1.0f;
    if ((htw_d_d = (c->size - pos.s0))>0) {
		//c->speed.s0 = fabs(c->speed.s0);
		if(c->speed.s0 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s0 += force_*fact;
		//circle[id].force.s0 += c->size*htw_d_d*c->E*fact;
	}else if ((htw_d_d = (c->size + pos.s0 - s.s0))>0) {
		//c->speed.s0 = -fabs(c->speed.s0);
		if(c->speed.s0 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s0 -= force_*fact;
		//circle[id].force.s0 -= c->size*htw_d_d*c->E*fact;
	}
	fact = 1.0f;
    if ((htw_d_d = (c->size - pos.s1))>0) {
		//c->speed.s1 = fabs(c->speed.s1);
		if(c->speed.s1 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s1 += force_*fact;
		//circle[id].force.s1 += c->size*htw_d_d*c->E*fact;
	}else if ((htw_d_d = (c->size + pos.s1 - s.s1))>0) {
		//c->speed.s1 = -fabs(c->speed.s1);
		if(c->speed.s1 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s1 -= force_*fact;
		//circle[id].force.s1 -= c->size*htw_d_d*c->E*fact;
	}
#if _3D_
	fact = 1.0f;
    if ((htw_d_d = (c->size - pos.s2))>0) {
		if(c->speed.s2 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s2 += force_*fact;
		//circle[id].force.s2 += c->size*htw_d_d*c->E*fact;
	}else if ((htw_d_d = (c->size + pos.s2 - s.s2))>0) {
		if(c->speed.s2 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s2 -= force_*fact;
		//circle[id].force.s2 -= c->size*htw_d_d*c->E*fact;
	}
#endif
	// */
	
	// Luftwiderstand:
    //circle[id].force -= 0.001*circle[id].speed*fabs(length(circle[id].speed));
    //0.00001*normalize(circle[id].speed)*pow_(length(circle[id].speed),2);
    
    /*circle[id].pos += c->speed;
    force = circle[id].force;
    circle[id].speed += force;
    circle[id].force -= force;*/
    
    force = c->force;
	
	
	// Gravitation nach unten:
	//circle[id].force.s1 -= gravity*circle[id].mass;
    acceleration = force / c->mass;
	acceleration.s1 -= gravity;
	
    c->pos += c->speed*delta_t;
    #if _v_nicht_const_
		c->pos += acceleration*(delta_t*delta_t)/2.0f;
	#endif
	c->speed += acceleration*delta_t;
    c->force -= force;
}

__kernel void moveStep3_addInterForces(__global struct Circle* circle, 
						__global scalar* elastic, __global scalar* G_)
{
	int id = get_global_id(0);
	int id2 = get_global_id(1);
	//printf("group id: %5d local id: %5d global id: %5d\n",id,id2, get_global_id(0));
	if(id2<=id) return;
	
	vector force;
	scalar reduced = *elastic, G = *G_;
	
	__global struct Circle* c;
	c = &circle[id];
	__global struct Circle* c2;
	c2 = &circle[id2];
    
    vector d_pos, d_n;
	scalar both_r, d, d_, R, E_;
	
	//#define c2 circle[i]
	both_r = c->size + c2->size;
	d_pos = c2->pos-c->pos;
	
	//*
	#if _G_==0
	if(G == 0)
	{
		if(d_pos.s0>both_r||d_pos.s0<-both_r||d_pos.s1>both_r||d_pos.s1<-both_r
			#if _3D_
			||d_pos.s2>both_r||d_pos.s2<-both_r
			#endif
			){
			return;
		}
	}
	#endif
	// */

	//d_pos.s2 = 0;
	d = length(d_pos);
	d = max(d,0.00005f);
	
	/*
	d_n = d_pos/d;
	force = 10000000000000000000000000.0*pow_(d,12) *d_n;
	c->force -= force;
	c2->force += force;
	force = 1000000000000.0*pow_(d,6) *d_n;
	c->force += force;
	c2->force -= force;
	return;*/
	
	#if _G_
	if(G!=0)
	{
		d_n = d_pos/d;
		 // bzw. normalize(d_pos);
		// Gravitation:
		force = G*c->mass*c2->mass/pow_(d,2) *d_n;
		c->force += force;
		c2->force -= force;
	}
	#endif
	
	// Abstossung:
	if (d < both_r) {
		//*
		//#if _G_==0
		if(G == 0)
		{
			d_n = d_pos/d; // bzw. normalize(d_pos);
		}
		//#endif
		// */
		// nach Kontaktmechanik mit Poisson-Zahlen:
		d_ = both_r - d;
		//*
		R = 1/((1/c->size)+(1/c2->size));
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c2->poisson*c2->poisson)/c2->E));
		force = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3)) *d_n;
		if(reduced!=1 && dot(d_pos, (c->speed-c2->speed)/d)<0){ //Skalarprodukt
			force *= reduced;
		}
		c->force -= force;
		c2->force += force;
		// */
	}
}


__kernel void randomFill2(__global struct Circle* circle, 
						__global vector* size,
						__global scalar* elastic){
}

__kernel void moveStep3_addWallForces(__global struct Circle* circle, 
						__global vector* size,
						__global scalar* elastic)
{
	vector s = *size;
	scalar reduced = *elastic;
	int id = get_global_id(0);
	
	__global struct Circle* c = &circle[id];
    
	scalar d_, R, E_, force_;
    
    //*
    scalar htw_d_d, fact = 1.0f;
    if ((htw_d_d = (c->size - c->pos.s0))>0) {
		//c->speed.s0 = fabs(c->speed.s0);
		if(c->speed.s0 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s0 += force_*fact;
		//circle[id].force.s0 += c->size*htw_d_d*c->E*fact;
	}else if ((htw_d_d = (c->size + c->pos.s0 - s.s0))>0) {
		//c->speed.s0 = -fabs(c->speed.s0);
		if(c->speed.s0 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s0 -= force_*fact;
		//circle[id].force.s0 -= c->size*htw_d_d*c->E*fact;
	}
	fact = 1.0f;
    if ((htw_d_d = (c->size - c->pos.s1))>0) {
		//c->speed.s1 = fabs(c->speed.s1);
		if(c->speed.s1 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s1 += force_*fact;
		//circle[id].force.s1 += c->size*htw_d_d*c->E*fact;
	}else if ((htw_d_d = (c->size + c->pos.s1 - s.s1))>0) {
		//c->speed.s1 = -fabs(c->speed.s1);
		if(c->speed.s1 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s1 -= force_*fact;
		//circle[id].force.s1 -= c->size*htw_d_d*c->E*fact;
	}
#if _3D_
	fact = 1.0f;
    if ((htw_d_d = (c->size - c->pos.s2))>0) {
		if(c->speed.s2 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s2 += force_*fact;
		//circle[id].force.s2 += c->size*htw_d_d*c->E*fact;
	}else if ((htw_d_d = (c->size + c->pos.s2 - s.s2))>0) {
		if(c->speed.s2 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c->size;
		E_ = 1/(((1-c->poisson*c->poisson)/c->E)+((1-c->poisson*c->poisson)/c->E));
		force_ = 4.0f/3.0f*E_*sqrt(R*pow_(d_,3));
		circle[id].force.s2 -= force_*fact;
		//circle[id].force.s2 -= c->size*htw_d_d*c->E*fact;
	}
#endif
}


__kernel void moveStep3_updatePositions(__global struct Circle* circle,
						__global scalar* g,
						__global scalar* delta_t_)
{
	vector force, acceleration;
	scalar gravity = *g, delta_t = *delta_t_;
	int id = get_global_id(0);
	
	__global struct Circle* c = &circle[id];
    
	// */
	
	// Luftwiderstand:
    //circle[id].force -= 0.001*circle[id].speed*fabs(length(circle[id].speed));
    //0.00001*normalize(circle[id].speed)*pow_(length(circle[id].speed),2);
    
    /*circle[id].pos += c->speed;
    force = circle[id].force;
    circle[id].speed += force;
    circle[id].force -= force;*/
    
    force = c->force;
	
	
	// Gravitation nach unten:
	//circle[id].force.s1 -= gravity*circle[id].mass;
    acceleration = force / c->mass;
	acceleration.s1 -= gravity;
	
	#if heun
		c->pos = c->oldPos;
	#endif
    c->pos += c->speed*delta_t;
    
    #if _v_nicht_const_
		c->pos += acceleration*(delta_t*delta_t)/2.0f;
	#endif
	c->speed += acceleration*delta_t;
    c->force -= force;
    
	#if heun
		c->oldPos = c->pos;
		c->pos += c->speed*(delta_t*step) + c->force/c->mass*(delta_t*step*delta_t*step)/2.0f;
	#endif
}


