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
    floating_type size;
    //float x[3];  //padding
} Circle;

uint GetUint(__global uint* m_z, __global uint* m_w)
{
	uint z = *m_z;
    z = 36969 * (z & 65535) + (z >> 16);
    *m_z = z;
    uint w = *m_w;
    w = 18000 * (w & 65535) + (w >> 16);
    *m_w = w;
    return (z << 16) + w;
}

floating_type uGetUniform(__global uint* m_z, __global uint* m_w)
{
    // 0 <= u < 2^32
    uint u = GetUint(m_z, m_w);
    // The magic number below is 1/(2^32 + 2).
    // The result is strictly between 0 and 1.
    return (u + 1.0) * 2328306435454494 * pow(10.0,-25.0);
}

floating_type GetUniform(__global uint* m_z, __global uint* m_w)
{
    // 0 <= u < 2^32
    uint u = GetUint(m_z, m_w);
    // The magic number below is 1/(2^32 + 2).
    // The result is strictly between -1 and 1.
    return ((u + 1.0) * 2328306435454494 * pow(10.0,-25.0) * 2)-1;
}

__kernel void randomFill(__global struct Circle* circle, __global uint* m_z, __global uint* m_w,
						__global floating_type_vector* size, __global floating_type* max_speed){
    int gid = get_global_id(0);
    floating_type_vector s = *size;
	circle[gid].size = 10;
    
	circle[gid].pos.s0 = circle[gid].size/2+uGetUniform(m_z, m_w)*(s.s0-circle[gid].size);
	circle[gid].pos.s1 = circle[gid].size/2+uGetUniform(m_z, m_w)*(s.s1-circle[gid].size);
	#if _3D_
		circle[gid].pos.s2 = circle[gid].size/2+uGetUniform(m_z, m_w)*(s.s2-circle[gid].size);
	#endif
	
	circle[gid].speed.s0 = GetUniform(m_z, m_w);
	circle[gid].speed.s1 = GetUniform(m_z, m_w);
	#if _3D_
		circle[gid].speed.s2 = GetUniform(m_z, m_w);
	#endif
	circle[gid].speed = normalize(circle[gid].speed)*uGetUniform(m_z, m_w)*(*max_speed);
	
	circle[gid].force.s0 = 0;
	circle[gid].force.s1 = 0;
	#if _3D_
		circle[gid].force.s2 = 0;
	#endif
}

__kernel void moveStep(__global struct Circle* circle, __global int* num,
						__global floating_type_vector* size, __global floating_type* E, 
						__global floating_type* elastic, __global floating_type* gravity)
{
	floating_type_vector s = *size, force;
	floating_type e = *E, reduced = *elastic, g = *gravity;
	int gid = get_global_id(0);
	struct Circle c2;
	struct Circle c = circle[gid];
    
    
    
    //*
    floating_type_vector d_pos, d_d;
	floating_type both_r, d;
	for(int i = (*num)-1; i>gid; i--){
		c2 = circle[i];
		//#define c2 circle[i]
		both_r = c.size + c2.size;
		d_pos = c2.pos-c.pos;
		//d_pos.s2 = 0;
		d = length(d_pos);
		if (d < both_r) {
			//c->thisStepHit = true;
			//if(!(c_.free && c2->free))continue;
			d_d = (both_r/d-1)*e*d_pos;
			//*
			if(dot(d_pos, (c.speed-c2.speed)/d)<0){ //Skalarprodukt
				d_d *= reduced;
				//circle[gid].speed = 0;
				//circle[i].speed = 0;
			}// */
			circle[gid].force -= c.size*d_d;
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
    
    floating_type htw_d_d, fact = 1.0;
    if ((htw_d_d = (c.size - c.pos.s0))>0) {
		//c.speed.s0 = fabs(c.speed.s0);
		if(c.speed.s0 > 0)fact = reduced;
		circle[gid].force.s0 += c.size*htw_d_d*e*fact;
	}else if ((htw_d_d = (c.size + c.pos.s0 - s.s0))>0) {
		//c.speed.s0 = -fabs(c.speed.s0);
		if(c.speed.s0 < 0)fact = reduced;
		circle[gid].force.s0 -= c.size*htw_d_d*e*fact;
	}
	fact = 1.0;
    if ((htw_d_d = (c.size - c.pos.s1))>0) {
		//c.speed.s1 = fabs(c.speed.s1);
		if(c.speed.s1 > 0)fact = reduced;
		circle[gid].force.s1 += c.size*htw_d_d*e*fact;
	}else if ((htw_d_d = (c.size + c.pos.s1 - s.s1))>0) {
		//c.speed.s1 = -fabs(c.speed.s1);
		if(c.speed.s1 < 0)fact = reduced;
		circle[gid].force.s1 -= c.size*htw_d_d*e*fact;
	}
#if _3D_
	fact = 1.0;
    if ((htw_d_d = (c.size - c.pos.s2))>0) {
		//if(c.speed.s2 > 0)fact = reduced;
		circle[gid].force.s2 += c.size*htw_d_d*e*fact;
	}else if ((htw_d_d = (c.size + c.pos.s2 - s.s2))>0) {
		//if(c.speed.s2 < 0)fact = reduced;
		circle[gid].force.s2 -= c.size*htw_d_d*e*fact;
	}
#endif
	circle[gid].force.s1 -= g;
    //circle[gid].force -= 0.001*circle[gid].speed*fabs(length(circle[gid].speed));
    //0.00001*normalize(circle[gid].speed)*pow(length(circle[gid].speed),2);
    circle[gid].pos += c.speed;
    force = circle[gid].force;
    circle[gid].speed += force;
    circle[gid].force -= force;
}


