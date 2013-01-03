#define _3D_ 0

#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_amd_fp64 : enable

//#define reduced 0.9999999999999
//0.9

#define floating_type double
#if _3D_
	#define floating_type_vector double3
#else
	#define floating_type_vector double2
#endif

typedef struct Circle
{
    floating_type_vector pos;
    floating_type_vector speed;
    floating_type_vector force;
    floating_type size,mass,poisson,E;
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

floating_type uGetUniform(uint* m_z, uint* m_w, uint gid)
{
    // 0 <= u < 2^32
    uint u = GetUint(m_z, m_w, gid);
    // The magic number below is 1/(2^32 + 2).
    // The result is strictly between 0 and 1.
    return (u + 1.0) * 2.328306435454494e-10;
		//* 2328306435454494 * pow(10.0,-25.0);
}

floating_type GetUniform(uint* m_z, uint* m_w, uint gid)
{
    // 0 <= u < 2^32
    uint u = GetUint(m_z, m_w, gid);
    // The magic number below is 1/(2^32 + 2).
    // The result is strictly between -1 and 1.
    return ((u + 1.0) * 2.328306435454494e-10
		//* 2328306435454494 * pow(10.0,-25.0) 
		* 2)-1;
}

__kernel void randomFill(__global struct Circle* circle, __global uint* z, __global uint* w,
						__global floating_type_vector* boxSize, __global floating_type* max_speed,
						__global floating_type_vector* size, __global floating_type* poisson,
						__global floating_type* E){
    int gid = get_global_id(0);
    uint m_z_ = (*z) + gid;
    uint m_w_ = (*w) + gid;
    uint* m_z = &m_z_;
    uint* m_w = &m_w_;
    floating_type_vector s = *boxSize, s2 = *size;
	circle[gid].size = s2.s0+((s2.s1-s2.s0)*uGetUniform(m_z, m_w, gid));
	circle[gid].mass = 4.0/3.0*pow(circle[gid].size,3)*M_PI  *950; //Kautschuk
	circle[gid].poisson = *poisson;
	circle[gid].E = *E;
	    
	circle[gid].pos.s0 = circle[gid].size/2+uGetUniform(m_z, m_w, gid)*(s.s0-circle[gid].size);
	circle[gid].pos.s1 = circle[gid].size/2+uGetUniform(m_z, m_w, gid)*(s.s1-circle[gid].size);
	#if _3D_
		circle[gid].pos.s2 = circle[gid].size/2+uGetUniform(m_z, m_w, gid)*(s.s2-circle[gid].size);
	#endif
	
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
						__global floating_type_vector* size,
						__global floating_type* elastic, __global floating_type* g,
						__global floating_type* delta_t_, __global floating_type* G_)
{
	floating_type_vector s = *size, force, acceleration;
	floating_type reduced = *elastic, gravity = *g, delta_t = *delta_t_, G = *G_;
	int id = get_global_id(0);
	struct Circle c2;
	struct Circle c = circle[id];
    
    
    
    floating_type_vector d_pos, d_d, d_n;
	floating_type both_r, d, d_, R, E_, force_;
	//*
	for(int i = (*num)-1; i>id; i--){
		c2 = circle[i];
		//#define c2 circle[i]
		both_r = c.size + c2.size;
		d_pos = c2.pos-c.pos;
		//d_pos.s2 = 0;
		d = length(d_pos);
		d_n = d_pos/d; // bzw. normalize(d_pos);
		
		// Gravitation:
		force = G*c.mass*c2.mass/pow(d,2) *d_n;
		circle[id].force += force;
		circle[i].force -= force;
		
		// Abstossung:
		if (d < both_r) {
			// nach Kontaktmechanik mit Poisson-Zahlen:
			d_ = both_r - d;
			R = 1/((1/c.size)+(1/c2.size));
			E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c2.poisson*c2.poisson)/c2.E));
			force = 4.0/3.0*E_*pow(R, 1.0/2.0)*pow(d_, 3.0/2.0) *d_n;
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
			//*
			if(dot(d_pos, (c.speed-c2.speed)/d)<0){ //Skalarprodukt
				d_d *= reduced;
				//circle[id].speed = 0;
				//circle[i].speed = 0;
			}// * /
			circle[id].force -= c.size*d_d;
			circle[i].force += c2.size*d_d;
			
			/*
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
    floating_type htw_d_d, fact = 1.0;
    if ((htw_d_d = (c.size - c.pos.s0))>0) {
		//c.speed.s0 = fabs(c.speed.s0);
		if(c.speed.s0 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0/3.0*E_*pow(R, 1.0/2.0)*pow(d_, 3.0/2.0);
		circle[id].force.s0 += force_*fact;
		//circle[id].force.s0 += c.size*htw_d_d*c.E*fact;
	}else if ((htw_d_d = (c.size + c.pos.s0 - s.s0))>0) {
		//c.speed.s0 = -fabs(c.speed.s0);
		if(c.speed.s0 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0/3.0*E_*pow(R, 1.0/2.0)*pow(d_, 3.0/2.0);
		circle[id].force.s0 -= force_*fact;
		//circle[id].force.s0 -= c.size*htw_d_d*c.E*fact;
	}
	fact = 1.0;
    if ((htw_d_d = (c.size - c.pos.s1))>0) {
		//c.speed.s1 = fabs(c.speed.s1);
		if(c.speed.s1 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0/3.0*E_*pow(R, 1.0/2.0)*pow(d_, 3.0/2.0);
		circle[id].force.s1 += force_*fact;
		//circle[id].force.s1 += c.size*htw_d_d*c.E*fact;
	}else if ((htw_d_d = (c.size + c.pos.s1 - s.s1))>0) {
		//c.speed.s1 = -fabs(c.speed.s1);
		if(c.speed.s1 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0/3.0*E_*pow(R, 1.0/2.0)*pow(d_, 3.0/2.0);
		circle[id].force.s1 -= force_*fact;
		//circle[id].force.s1 -= c.size*htw_d_d*c.E*fact;
	}
#if _3D_
	fact = 1.0;
    if ((htw_d_d = (c.size - c.pos.s2))>0) {
		if(c.speed.s2 > 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0/3.0*E_*pow(R, 1.0/2.0)*pow(d_, 3.0/2.0);
		circle[id].force.s2 += force_*fact;
		//circle[id].force.s2 += c.size*htw_d_d*c.E*fact;
	}else if ((htw_d_d = (c.size + c.pos.s2 - s.s2))>0) {
		if(c.speed.s2 < 0)fact = reduced;
		d_ = htw_d_d;
		R = c.size;
		E_ = 1/(((1-c.poisson*c.poisson)/c.E)+((1-c.poisson*c.poisson)/c.E));
		force_ = 4.0/3.0*E_*pow(R, 1.0/2.0)*pow(d_, 3.0/2.0);
		circle[id].force.s2 -= force_*fact;
		//circle[id].force.s2 -= c.size*htw_d_d*c.E*fact;
	}
#endif
	// */
	
	// Gravitation nach unten:
	circle[id].force.s1 -= gravity*circle[id].mass;
	
	// Luftwiderstand:
    //circle[id].force -= 0.001*circle[id].speed*fabs(length(circle[id].speed));
    //0.00001*normalize(circle[id].speed)*pow(length(circle[id].speed),2);
    
    /*circle[id].pos += c.speed;
    force = circle[id].force;
    circle[id].speed += force;
    circle[id].force -= force;*/
    
    force = circle[id].force;
    acceleration = force / circle[id].mass;
    circle[id].pos += circle[id].speed*delta_t + 0.5*acceleration*(delta_t*delta_t);
    circle[id].speed += acceleration*delta_t;
    circle[id].force -= force;
}


