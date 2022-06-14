
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <CL/opencl.h>

using namespace std;

#define N (1024 * 1024)
#define n (4 * 1)
#define inf (INT_MAX)

int main()
{

	struct timeval start, end;

	
	// double start = steady_clock::now();

	cl_platform_id fpga_paltform = NULL;
	clGetPlatformIDs(1, &fpga_paltform, NULL);

	// Search for an openCL device
	cl_device_id fpga_device = NULL;
	clGetDeviceIDs(fpga_paltform, CL_DEVICE_TYPE_ALL, 1, &fpga_device, NULL);

	// Create a context.
	cl_context context = clCreateContext(NULL, 1, &fpga_device, NULL, NULL, NULL);

	// Create a command queue.
	cl_command_queue queue = clCreateCommandQueue(context, fpga_device, 0, NULL);

	// Read FPGA binary
	size_t length = 0x10000000;
	unsigned char *binary = (unsigned char *)malloc(length);

	FILE *fp = fopen("floydWarshall.aocx", "rb");
	fread(binary, length, 1, fp);
	fclose(fp);

	// Create program
	cl_program program = clCreateProgramWithBinary(context, 1, &fpga_device, &length, (const unsigned char **)&binary, NULL, NULL);

	// Create kernel.
	cl_kernel kernel = clCreateKernel(program, "floydWarshall", NULL);

	// Host side data
	int *host_din, *host_dout;
	int node, edge, nn;

	cout << "Enter the number of node and vertices: ";

	cin >> node >> edge;

	posix_memalign((void **)(&host_din), 64, sizeof(int) * N);
	posix_memalign((void **)(&host_dout), 64, sizeof(int) * N);

	// int sq = sqrt(n);

	int dis[node][node];

	// node=4;

	for (int i = 0; i < node; i++)
	{
		for (int j = 0; j < node; j++)
		{
			if (i == j)
				dis[i][j] = 0;
			else
				dis[i][j] = inf;
		}
	}
	for (int i = 1; i <= edge; i++)
	{
		int x, y, d;
		cin >> x >> y >> d;
		dis[x][y] = d;
	}

	int x = 0;
	for (int i = 0; i < node; i++)
	{
		for (int j = 0; j < node; j++)
		{
			host_din[x++] = dis[i][j];
		}
	}

	// host_din[0] = 0;
	// host_din[1] = 5;
	// host_din[2] = inf;
	// host_din[3] = 10;
	// host_din[4] = inf;
	// host_din[5] = 0;
	// host_din[6] = 3;
	// host_din[7] = inf;
	// host_din[8] = inf;
	// host_din[9] = inf;
	// host_din[10] = 0;
	// host_din[11] = 1;
	// host_din[12] = inf;
	// host_din[13] = inf;
	// host_din[14] = inf;
	// host_din[15] = 0;

	for (int i = 0; i < node * node; i++)
	{
		host_dout[i] = inf;
	}
	

	// Create memory Object
	cl_mem dev_din = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * N, NULL, NULL);

	// cl_mem dev_n = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int),NULL, NULL);

	cl_mem dev_dout = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * N, NULL, NULL);

	// FPGA side data
	clEnqueueWriteBuffer(queue, dev_din, CL_TRUE, 0, sizeof(int) * N, host_din, 0, NULL, NULL);
	// clEnqueueWriteBuffer(queue, dev_n, CL_TRUE, 0, sizeof(int), host_n, 0,NULL, NULL);

       gettimeofday(&start, NULL);
	// Execute kernel
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &dev_din);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &dev_dout);
	clSetKernelArg(kernel, 2, sizeof(cl_int), &node);

	// clSetKernelArg(kernel, 2, 64, &host_n);
	cl_event kernel_event;



	clEnqueueTask(queue, kernel, 0, NULL, &kernel_event);
	clWaitForEvents(1, &kernel_event);
	clReleaseEvent(kernel_event);
        gettimeofday(&end, NULL);
	// Read data from FPGA
	clEnqueueReadBuffer(queue, dev_dout, CL_TRUE, 0, sizeof(int) * N, host_dout, 0, NULL, NULL);

	

	// Print output
	printf("The distant between all pairs:\n");
	for (int i = 0; i < node * node; i++)
	{

		// cout<<host_dout[i]<<" ";
		if ((i + 1) % node == 0)
		{
			if (host_dout[i] != inf)
			{
				printf(" %d ", host_dout[i]);
			}
			else
			{
				printf(" inf ");
			}

			printf("\n");
		}
		else
		{
			if (host_dout[i] != inf)
			{
				printf(" %d ", host_dout[i]);
			}
			else
			{
				printf(" inf ");
			}
		}
	}

	cout << endl;

	clFlush(queue);

	clFinish(queue);
	// Free memory
	clReleaseMemObject(dev_din);
	clReleaseMemObject(dev_dout);
	// clReleaseMemObject(dev_n);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	free(host_din);
	free(host_dout);
	// free(host_n);

	//double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    double time_taken;
  
    time_taken = (end.tv_sec - start.tv_sec) * 1e6;
    time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6;
  
    cout << "Time taken by program is : " << fixed<< time_taken << setprecision(6);
    cout << " sec" << endl;

	//cout << "Time taken by program is : " << fixed << time_taken << setprecision(20);
	//printf("time taken : %.50lf\n", time_taken);
	// cout << " sec " << endl;

	//   for(int k=0; k<node; k++)
	// {
	//     for(int i=0; i<node; i++)
	//     {
	//         // #pragma unroll
	//         for(int j=0; j<node; j++)
	//         {
	//             if(matrix[i][k]!=inf && matrix[k][j]!=inf)
	//             {
	//                 //ans[i][j] = matrix[i][k]+matrix[k][j]);
	//             }
	//             //ans[i][j]+=(matrix[i][k]*matrix[k][j]);
	//         }
	//     }
	// }

	// 0 3 10
	// 0 1 5
	// 1 2 3
	// 2 3 1



// 0 1 5
// 1 0 3
// 0 2 3
// 2 0 5
// 1 5 4
// 1 3 5
// 2 3 2
// 4 3 2
// 4 5 6
// 5 4 7













	return 0;
}


