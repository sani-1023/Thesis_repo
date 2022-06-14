//#define p (4)
__kernel void MatrixMultiplicationRWCL( __global const int * restrict row,__global const int * restrict col,
__global int * restrict k_dout,int n)
{

    int x = 0;
      for(int i = 1;i<=n;i++)
    {
        int j = (i-1)*n+1,sum = 0;
        for(int k = 1;k<=n*n;k++)
        {
            sum+=row[j]*col[k];
            if(j == i*n)
            {
                k_dout[x++] = sum;
                sum = 0;
                j = (i-1)*n+1;
            }
            else j++;
        }
    }


}





