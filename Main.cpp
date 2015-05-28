#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include <CL/cl.h>

void randomizeArray(cl_int* data, size_t vectorSize)
{
    for (size_t i = 0; i < vectorSize; ++i) 
    {
        data[i] = rand() % 10;
    }
}

void zeroArray(cl_int* data, size_t vectorSize)
{
	for (size_t i = 0; i < vectorSize; ++i)
	{
		data[i] = 0;
	}
}

void compareArray(cl_int* a, cl_int* b, cl_int* d, size_t vectorSize)
{
	for (size_t i = 0; i < vectorSize; ++i)
	{
		switch (i % 4){
		case 0: d[0] = a[i] == b[i] ? ++d[0] : d[0];
			break;
		case 1: d[1] = a[i] == b[i] ? ++d[1] : d[1];
			break;
		case 2: d[2] = a[i] == b[i] ? ++d[2] : d[2];
			break;
		case 3: d[3] = a[i] == b[i] ? ++d[3] : d[3];
			break;
		}
	}
}

int main()
{
	// zmienne do pomiaru czasu algorytmów
	std::chrono::time_point<std::chrono::system_clock>start, end;
	std::chrono::duration<double> alg1, alg2;

    cl_int error = CL_SUCCESS;

    // Numer Platformy.
    cl_uint platformNumber = 0;

    error = clGetPlatformIDs(0, NULL, &platformNumber);
	std::cout << error << std::endl;

    if (0 == platformNumber)
    {
        std::cout << "No OpenCL platforms found." << std::endl;

        return 0;
    }

    // Identyfikator platformy.
    cl_platform_id* platformIds = new cl_platform_id[platformNumber];

    error = clGetPlatformIDs(platformNumber, platformIds, NULL);
	std::cout << error << std::endl;

    // Informacje o platformie.
    for (cl_uint i = 0; i < platformNumber; ++i)
    {
        char name[1024] = { '\0' };

        std::cout << "Platform:\t" << i << std::endl;

        error = clGetPlatformInfo(platformIds[i], CL_PLATFORM_NAME, 1024, &name, NULL);
		std::cout << error << std::endl;

        std::cout << "Name:\t\t" << name << std::endl;

        error = clGetPlatformInfo(platformIds[i], CL_PLATFORM_VENDOR, 1024, &name, NULL);
		std::cout << error << std::endl;

        std::cout << "Vendor:\t\t" << name << std::endl;

        std::cout << std::endl;
    }

    // Numer urz¹dzenia.
    cl_uint deviceNumber;

    error = clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_DEFAULT, 0, NULL, &deviceNumber);
	std::cout << error << std::endl;

    if (0 == deviceNumber)
    {
        std::cout << "No OpenCL devices found on platform " << 1 << "." << std::endl;
    }

    // Identyfikator urz¹dzenia.
    cl_device_id* deviceIds = new cl_device_id[deviceNumber];

	error = clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_DEFAULT, deviceNumber, deviceIds, &deviceNumber);
	std::cout << error << std::endl;

    // informacje o urz¹dzeniu.
    for (cl_uint i = 0; i < deviceNumber; ++i)
    {
        char name[1024] = { '\0' };

        std::cout << "Device:\t\t" << i << std::endl;

        error = clGetDeviceInfo(deviceIds[i], CL_DEVICE_NAME, 1024, &name, NULL);
		std::cout << error << std::endl;

        std::cout << "Name:\t\t" << name << std::endl;

        error = clGetDeviceInfo(deviceIds[i], CL_DEVICE_VENDOR, 1024, &name, NULL);
		std::cout << error << std::endl;

        std::cout << "Vendor:\t\t" << name << std::endl;

        error = clGetDeviceInfo(deviceIds[i], CL_DEVICE_VERSION, 1024, &name, NULL);
		std::cout << error << std::endl;

        std::cout << "Version:\t" << name << std::endl;
    }

    std::cout << std::endl;

    // Alokacja pamiêci dla  tablic
    size_t vectorSize = 1000000;
    size_t localWorkSize = 8;

    cl_int* a = new cl_int[vectorSize];
    cl_int* b = new cl_int[vectorSize];
    cl_int* c = new cl_int[vectorSize];
	cl_int* d = new cl_int[4];

    randomizeArray(a, vectorSize);
    randomizeArray(b, vectorSize);
	zeroArray(c, vectorSize);
	zeroArray(d, 4);

    // Utworzenie kontekstu OpenCL. Um o¿liwia on wykonywanie poleceñ na zadanym urz¹dzeniu.
    cl_context context = clCreateContext(0, deviceNumber, deviceIds, NULL, NULL, NULL);

    if (NULL == context)
    {
        std::cout << "Failed to create OpenCL context." << std::endl;
    }

    // Utworzenie kolejki poleceñ na wybranym urz¹dzeniu.
    cl_command_queue commandQueue = clCreateCommandQueue(context, deviceIds[0], 0, &error);

    // Allokowanie pamiêci dla naszych wektorów na urz¹dzeniu na którym bêdziemy pracowaæ.
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY , sizeof(cl_int) * vectorSize, NULL, &error);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY , sizeof(cl_int) * vectorSize, NULL, &error);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * vectorSize, NULL, &error);
	cl_mem bufferD = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * 4, NULL, &error);

    // Wczytywanie kodu programu z pliku, ³adowanie do rz¹dzenia i kompilacja.
//Program Cmp
	std::ifstream file1(".\\bin\\Cmp.cl", std::ifstream::in);
    std::string str1;

	file1.seekg(0, std::ios::end);
	size_t programSize1 = (size_t)file1.tellg();

	str1.reserve((unsigned int)file1.tellg());
	file1.seekg(0, std::ios::beg);

	str1.assign(std::istreambuf_iterator<char>(file1), std::istreambuf_iterator<char>());
	const char* source1 = str1.c_str();

	// Kompilacja.
	cl_program program1 = clCreateProgramWithSource(context, 1, &source1, &programSize1, &error);

	error = clBuildProgram(program1, 0, NULL, NULL, NULL, NULL);
	std::cout << error << std::endl;

	// Utworzenie kernela.
	cl_kernel kernel1 = clCreateKernel(program1, "Cmp", &error);

//Program Add
	std::ifstream file2(".\\bin\\Add.cl", std::ifstream::in);
	std::string str2;

	file2.seekg(0, std::ios::end);
	size_t programSize2 = (size_t)file2.tellg();
    
	str2.reserve((unsigned int)file2.tellg());
	file2.seekg(0, std::ios::beg);    
 
	str2.assign(std::istreambuf_iterator<char>(file2), std::istreambuf_iterator<char>());
	const char* source2 = str2.c_str();

    // Kompilacja.
   	cl_program program2 = clCreateProgramWithSource(context, 1, &source2, &programSize2, &error);
    
	error = clBuildProgram(program2, 0, NULL, NULL, NULL, NULL); 
	std::cout << error << std::endl;

    // Utworzenie kernela.
    cl_kernel kernel2 = clCreateKernel(program2, "Add", &error);

	start = std::chrono::system_clock::now();
// Kernel 1
    // Ustawienie wartoœci argumentów.
    error = clSetKernelArg(kernel1, 0, sizeof(cl_mem), (void*)&bufferA);
	std::cout << error << std::endl;
    error = clSetKernelArg(kernel1, 1, sizeof(cl_mem), (void*)&bufferB);
	std::cout << error << std::endl;
    error = clSetKernelArg(kernel1, 2, sizeof(cl_mem), (void*)&bufferC);
	std::cout << error << std::endl;
    error = clSetKernelArg(kernel1, 3, sizeof(cl_int), (void*)&vectorSize);
	std::cout << error << std::endl;

    // Asynchroniczny zapis danych do urz¹dzenia.
    error = clEnqueueWriteBuffer(commandQueue, bufferA, CL_FALSE, 0, sizeof(cl_int) * vectorSize, a, 0, NULL, NULL);
	std::cout << error << std::endl;
    error = clEnqueueWriteBuffer(commandQueue, bufferB, CL_FALSE, 0, sizeof(cl_int) * vectorSize, b, 0, NULL, NULL);
	std::cout << error << std::endl;

    // Uruchomienie kernela.
    error = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, &vectorSize, &localWorkSize, 0, NULL, NULL);
	std::cout << error << std::endl;

    // Wczytanie danych wynikowych.
    error = clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 0, sizeof(cl_int) * vectorSize, c, 0, NULL, NULL);
	std::cout << error << std::endl;
	
// Kernel 2
	// Ustawienie wartoœci argumentów.
	error = clSetKernelArg(kernel2, 0, sizeof(cl_mem), (void*)&bufferA);
	std::cout << error << std::endl;
	error = clSetKernelArg(kernel2, 1, sizeof(cl_mem), (void*)&bufferD);
	std::cout << error << std::endl;
	error = clSetKernelArg(kernel2, 2, sizeof(cl_int), (void*)&vectorSize);
	std::cout << error << std::endl;

	// Asynchroniczny zapis danych do urz¹dzenia.
	error = clEnqueueWriteBuffer(commandQueue, bufferA, CL_FALSE, 0, sizeof(cl_int)* vectorSize, c, 0, NULL, NULL);
	std::cout << error << std::endl;

	// Uruchomienie kernela.
	error = clEnqueueNDRangeKernel(commandQueue, kernel2, 1, NULL, &vectorSize, &localWorkSize, 0, NULL, NULL);
	std::cout << error << std::endl;

	// Wczytanie danych wynikowych.
	error = clEnqueueReadBuffer(commandQueue, bufferD, CL_TRUE, 0, sizeof(cl_int)* 4, d, 0, NULL, NULL);
	std::cout << error << std::endl;
	end = std::chrono::system_clock::now();
	alg1 = end - start;
	//Wypisanie wyniku.
	/*
	for (size_t i = 0; i < vectorSize; ++i)
	{
		std::cout << a[i] << "\t" << b[i] << "\t" << c[i] << std::endl;
	}*/
	std::cout << "Wynik na GPU:" << std::endl;
	for (size_t i = 0; i < 4; ++i)
	{
		std::cout << d[i] << std::endl;
	}

//CPU 
	zeroArray(d, 4);
	start = std::chrono::system_clock::now();
	compareArray(a, b, d, vectorSize);
	end = std::chrono::system_clock::now();
	alg2 = end - start;
	std::cout << "Wynik na CPU:\n";
	for (size_t i = 0; i < 4; ++i)
	{
		std::cout << d[i] << std::endl;
	}
	std::cout << "GPU czas :" << alg1.count() << "s\n Za to na CPU: " << alg2.count() << "s\n";
    // Czyszczenie i zwalnianie pamiêci.
    clFlush(commandQueue);
    clFinish(commandQueue);
    clReleaseKernel(kernel1);
    clReleaseProgram(program1);
	clReleaseKernel(kernel2);
	clReleaseProgram(program2);
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
	clReleaseMemObject(bufferD);
    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);

    delete[] a;
    delete[] b;
    delete[] c;
	delete[] d;

    delete[] platformIds;
    delete[] deviceIds;

    // Wciœnij Enter by zakoñczyæ.
    std::cin.get();

    return 0;
}
