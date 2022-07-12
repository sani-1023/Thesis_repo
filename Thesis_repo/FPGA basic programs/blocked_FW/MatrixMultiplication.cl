#define n (800)
__kernel void MatrixMultiplication( __global const int * restrict k_din,
__global int * restrict k_dout)
{

int arr[n+5][n+5],mat_multi[n][n];
    int row=0,col=0;
    int k=0;
            for(int i=1;i<=n;i++){ 
                for(int j=1;j<=n;j++){
                    arr[i][j]=k_din[k++];
                }
                
            }
    for(int rnd = 1; rnd<=n/4; rnd++)
    {
 
        //for self_dependant block
 
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=(rnd-1)*4+1; i<=rnd*4; i++)
            {
                for(int j=(rnd-1)*4+1; j<=rnd*4; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        //phase-2 block right of block (r,r)
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=(rnd-1)*4+1; i<=rnd*4; i++)
            {
                for(int j=rnd*4+1; j<=n; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        // phase-2 blocks above (r,r);
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=1; i<=(rnd-1)*4; i++)
            {
                for(int j=(rnd-1)*4+1; j<=rnd*4; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        // phase-3 blocks above and right of block (r,r)
 
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=1; i<=(rnd-1)*4; i++)
            {
                for(int j=rnd*4+1; j<=n; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        //phase-2 blocks left of block (r,r)
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=(rnd-1)*4+1; i<=rnd*4; i++)
            {
                for(int j=1; j<=rnd*4; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        //phase-3 above and left of block (r,r)
        for( int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=1; i<=(rnd-1)*4; i++)
            {
                for(int j=1; j<=rnd*4; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        //phase-2 blocks below (r,r)
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=(rnd-1)*4+1; i<=n; i++)
            {
                for(int j=(rnd-1)*4+1; j<=rnd*4; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        //phase-3 blocks below and left of (r,r)
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=(rnd-1)*4+1; i<=n; i++)
            {
                for(int j=1; j<=rnd*4; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
        //phase-3 blocks right and below (r,r)
 
        for(int k = (rnd-1)*4+1 ; k<=rnd*4; k++)
        {
            for(int i=(rnd-1)*4+1; i<=n; i++)
            {
                for(int j=rnd*4+1; j<=n; j++)
                {
                    arr[i][j] = min(arr[i][j],arr[i][k]+arr[k][j]);
                }
            }
        }
 
    }


    int x=0;
    for(int i=1; i<=n; i++)
    {
        for(int j=1; j<=n; j++)
        { 
            k_dout[x++] = arr[i][j];
        }
    }



}




