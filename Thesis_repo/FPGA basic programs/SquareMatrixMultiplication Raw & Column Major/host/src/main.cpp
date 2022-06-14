
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#include <CL/opencl.h>

using namespace std;

#define N (2048 * 2048)
#define n (1500 * 1500)
#define sz (1500)


long getTime(void){
struct timeval tp;
gettimeofday(&tp,NULL);
long double ms = tp.tv_sec*1000 + tp.tv_usec/1000;

return ms;

}

int main()
{

struct timeval start, end;

	//start = clock();
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

	FILE *fp = fopen("MatrixMultiplicationRWCL.aocx", "rb");
	fread(binary, length, 1, fp);
	fclose(fp);

	// Create program
	cl_program program = clCreateProgramWithBinary(context, 1, &fpga_device, &length, (const unsigned char **)&binary, NULL, NULL);

	// Create kernel.
	cl_kernel kernel = clCreateKernel(program, "MatrixMultiplicationRWCL", NULL);

	// Host side data
	int *host_din, *host_dout, *host_din2;
	int nn = 15;
        int upper = 3,lower = 1;
        srand(time(0));
	int matrix[sz+1][sz+1];
	int x = 0;
	for (int i = 1; i <=sz; i++)
	{
		for (int j = 1; j <=sz; j++)
		{
			matrix[i][j] = (rand() %(upper - lower + 1)) + lower;
		}
	}
	x = 1;     
	posix_memalign((void **)(&host_din), 64, sizeof(int) * N);
	posix_memalign((void **)(&host_din2), 64, sizeof(int) * N);
	posix_memalign((void **)(&host_dout), 64, sizeof(int) * N);

	for (int i = 1; i <=sz; i++)
	{
		for (int j = 1; j <=sz; j++)
		{
			host_din[x++] = matrix[i][j];
		}
	}

	

    x=1;
	for (int i = 1; i <= sz; i++)
	{
		for (int j = 1; j <=sz; j++)
		{
			host_din2[x++] = matrix[j][i];
		}
	}




        int q =sz;

	// host_n[0] = sq;

	// Create memory Object
	cl_mem dev_din = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * N, NULL, NULL);
	cl_mem dev_din2 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * N, NULL, NULL);

	// cl_mem dev_n = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int),NULL, NULL);

	cl_mem dev_dout = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * N, NULL, NULL);


     
	// FPGA side data
	clEnqueueWriteBuffer(queue, dev_din, CL_TRUE, 0, sizeof(int) * N, host_din, 0, NULL, NULL);

	clEnqueueWriteBuffer(queue, dev_din2, CL_TRUE, 0, sizeof(int) * N, host_din2, 0, NULL, NULL);
	// clEnqueueWriteBuffer(queue, dev_n, CL_TRUE, 0, sizeof(int), host_n, 0,NULL, NULL);
          long start_naive = getTime();
	// Execute kernel

        gettimeofday(&start, NULL);


	clSetKernelArg(kernel, 0, sizeof(cl_mem), &dev_din);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &dev_din2);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &dev_dout);
	clSetKernelArg(kernel, 3, sizeof(cl_int), &q);

	// clSetKernelArg(kernel, 2, 64, &host_n);
	cl_event kernel_event;
	clEnqueueTask(queue, kernel, 0, NULL, &kernel_event);
	clWaitForEvents(1, &kernel_event);
	clReleaseEvent(kernel_event);
         long end_naive = getTime();


        cout<<"Time taken: "<<(end_naive - start_naive)<<" miliseconds"<<endl;
	// Read data from FPGA
	clEnqueueReadBuffer(queue, dev_dout, CL_TRUE, 0, sizeof(int) * N, host_dout, 0, NULL, NULL);

	gettimeofday(&end, NULL);
	// printf("The input matrix : \n");
	// int sq = sqrt(n);
	// for(int i=0; i<n; i++){
	//     if((i+1)%sq==0){
	//       printf(" %lf ", host_din[i]);
	//       printf("\n");
	//      }else{
	//        printf(" %lf ", host_din[i]);
	//      }

	// }

	// Print output
	/*printf("The resultant matrix after multiplied with itself:\n");
	for (int i = 0; i < n; i++)
	{
		if ((i + 1) % sz == 0)
		{
			printf(" %d ", host_dout[i]);
			printf("\n");
		}
		else
		{
			printf(" %d ", host_dout[i]);
		}
	}

	cout << endl;*/
	// printf("din[%d] = %d; dout[%d] = %d\n", i, host_din[i], i, host_dout[i]);
	// printf(" dout[%d] = %d\n", i, host_dout[i]);

	clFlush(queue);

	clFinish(queue);
	// Free memory
	clReleaseMemObject(dev_din);
	clReleaseMemObject(dev_din2);
	clReleaseMemObject(dev_dout);
	// clReleaseMemObject(dev_n);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	free(host_din);
	free(host_din2);
	free(host_dout);
	// free(host_n);



   /* double time_taken;
  
    time_taken = (end.tv_sec - start.tv_sec) * 1e6;
    time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6;*/
  
   // cout << "Time taken by program is : " << fixed<< time_taken << setprecision(6);
   // cout << " sec" << endl;



	// double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

	// // cout << "Time taken by program is : " << fixed << time_taken << setprecision(20);
	// printf("time taken : %.50lf\n", time_taken);
	// // cout << " sec " << endl;

	return 0;
}

