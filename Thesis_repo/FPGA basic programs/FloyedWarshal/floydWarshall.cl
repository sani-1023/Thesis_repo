#define n (4)
#define inf (INT_MAX)

__kernel void floydWarshall( __global const int * restrict k_din,
__global int * restrict k_dout,int node)
{



//n = node;

int matrix[100][100];


    int row=0,col=0;
    for(int i=0; i<node*node; i++)
    {
        col = i%node;
        row = i/node;
        matrix[row][col] = k_din[i];
        
 
    }

	  for(int k=0; k<node; k++)
    {           
        for(int i=0; i<node; i++)
        {
            for(int j=0; j<node; j++)
            {   
                if(matrix[i][k]!=inf && matrix[k][j]!=inf)
                {
                    matrix[i][j] = min(matrix[i][j],matrix[i][k]+matrix[k][j]);
                }
               
            }
        }
    }

 


   int x=0;
    for(int i=0; i<node; i++)
    {
        for(int j=0; j<node; j++)
        { 
            k_dout[x++] = matrix[i][j];
            
        }
    }



}





















