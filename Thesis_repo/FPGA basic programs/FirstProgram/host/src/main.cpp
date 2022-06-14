#include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <CL/opencl.h>

 #define N (1024*1024)


int main()
 {

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

FILE *fp = fopen("FirstProgram.aocx", "rb");

fread(binary, length, 1, fp);

fclose(fp);

// Create program

cl_program program = clCreateProgramWithBinary(context, 1, &fpga_device, &
length,(const unsigned char **)&binary, NULL, NULL);

// Create kernel.

cl_kernel kernel = clCreateKernel(program, "FirstProgram", NULL);

// Host side data

int *host_din, *host_dout;

posix_memalign((void **)(&host_din), 64, sizeof(int)*N);

posix_memalign((void **)(&host_dout), 64, sizeof(int)*N);

for(int i=0; i<N; i++)
{
host_din[i] = i;
host_dout[i] = 0;
}

// Create memory Object

cl_mem dev_din = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int)*N,
NULL, NULL);

cl_mem dev_dout = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int)*N,NULL, NULL);

// FPGA side data

clEnqueueWriteBuffer(queue, dev_din, CL_TRUE, 0, sizeof(int)*N, host_din, 0,
NULL, NULL);

// Execute kernel

clSetKernelArg(kernel, 0, sizeof(cl_mem), &dev_din);

clSetKernelArg(kernel, 1, sizeof(cl_mem), &dev_dout);

cl_event kernel_event;

clEnqueueTask(queue, kernel, 0, NULL, &kernel_event);

clWaitForEvents(1, &kernel_event);

clReleaseEvent(kernel_event);
// Read data from FPGA

clEnqueueReadBuffer(queue, dev_dout, CL_TRUE, 0,sizeof(int)*N, host_dout, 0,
NULL, NULL);

// Print output

for(int i=N-10; i<N; i++)

printf("din[%d] = %d; dout[%d] = %d\n", i, host_din[i], i, host_dout[i]);
clFlush(queue);
clFinish(queue);

// Free memory

clReleaseMemObject(dev_din);

clReleaseMemObject(dev_dout);

clReleaseKernel(kernel);

clReleaseProgram(program);

clReleaseCommandQueue(queue);

clReleaseContext(context);

free(host_din);

free(host_dout);

return 0;}

