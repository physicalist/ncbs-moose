/*
 * testMoose.cpp
 *
 *  Created on: Jun 4, 2013
 *      Author: saeed
 */

#include <iostream>
#include <cstdlib>
#include <Python.h>

using namespace std;

int main()
{
	Py_Initialize();
	FILE *file = fopen("testHsolve.py", "r+");
	if(file != NULL) {
	     PyRun_SimpleFile(file, "testHsolve.py");
	}

	fclose(file);
	Py_Finalize();


	return 0;
}

