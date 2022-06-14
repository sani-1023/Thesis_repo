#define N (1024*1024)

 __kernel void FirstProgram( __global const int * restrict k_din,__global int * restrict k_dout )
 {

for(unsigned i=0; i<N; i++)

k_dout[i] = k_din[i] + 40;
 }


