//#include "Definitions.h"
#include "core/Solver.h"
#include <cassert>
#include <limits>

#include "Manager.h"
#include <Eigen/Dense>
#include <Eigen/StdVector>

using Eigen::MatrixXd;

using namespace pn2s;

int main(int argc, char **argv) {

	int matrix_size = 3;
	int batch_size = 10;

	int tMax = 1;
	double dt = 1;


	Manager::Setup(dt);

	MatrixXd mx (matrix_size,matrix_size);
	//Fill data
	for (int i = 0; i < matrix_size; ++i) {
		for (int j = 0; j < matrix_size; ++j) {
			mx(i,j) = i+j;
		}
	}

	for (int idx_batch = 0; idx_batch < batch_size; ++idx_batch) {

		models::Model<TYPE_> neutral(mx, idx_batch);

		Manager::InsertModel(neutral);
	}

	//Sync with devices
	Manager::Reinit();

	//Process simulation
	for (int t = 0; t < tMax; ++t) {
		Manager::Process();
	}

	return 0;
}
