#define n (100)
__kernel void MatrixMultiplication( __global const int * restrict k_din,
__global int * restrict k_dout )
{

int matrix[n][n],mat_multi[n][n];
    int row=0,col=0;
    for(int i=0; i<n*n; i++)
    {
        col = i%n;
        row = i/n;
        matrix[row][col] = k_din[i];
        mat_multi[row][col] = 0;
 
    }

   for(int i=0; i<n; i++)
    {   
        #pragma unroll
            
        for(int j=0; j<n; j++)
        {
            #pragma unroll 
            for(int k=0; k<n; k++)
            {   
                mat_multi[i][j]+=matrix[i][k]*matrix[k][j];
            }
        }
    }


   int x=0;
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        { 
            k_dout[x++] = mat_multi[i][j];
            
        }
    }



}




