#include <stdio.h>
#include <stdlib.h>

void multiply(int m1rows, int m1cols, int m2rows, int m2cols, double matrix1[][m1cols], double matrix2[][m2cols], double product[][m2cols]);
void transpose(int m1rows, int m1cols, double matrix1[][m1cols], double m1Transposed[][m1rows]);
void inverse(int m1rows, int m1cols, double matrix1[][m1cols], double identityMatrix[][m1cols]);
void printMatrix(int rows, int cols, double matrix1[][cols]);
void createIdentityMatrix(int m1rows, int m1cols, double identityMatrix[][m1cols]);

int main(int argc, char* argv[]){
	int xrows, xcols;
	char trainordata[10];
	FILE *train;
	train = fopen(argv[1], "r");
	if (train == NULL){
		printf("file doesn't exist\n");
		exit(1);
	}
	fscanf(train, "%s", trainordata); //dummy variable 
	fscanf(train, "%d", &xcols); //first line contains attributes so columns
	xcols++; //to account for the column of 1s
	fscanf(train, "%d", &xrows); //second line contains number of houses so rows

	double xmat[xrows][xcols];
	double ymat[xrows][1];
	int i, j;
	double currentNum;
	for (i=0; i<xrows; i++){ //fill the first column with 1
		xmat[i][0] = 1;
	}
	for (i=0; i<xrows; i++){ //fills xmat and ymat 
		for (j=1; j<=xcols; j++){
		  fscanf(train, "%lf", &currentNum);
		  if (j == xcols){ //house price, add to y
		    ymat[i][0] = currentNum;
		  }else{ //not house price, add to x
		    xmat[i][j] = currentNum;
		  }
		}
	}

	double wmat[xcols][1]; //weight matrix
	//CALCULATE WEIGHT TIME: (xmatTransposed*xmat)^-1*xmatTransposed*ymat
	double xmatTransposed[xcols][xrows];
	transpose(xrows, xcols, xmat, xmatTransposed); //SUCCESSFUL

	double xmatTransposedtimesxmat[xcols][xcols];
	multiply(xcols, xrows, xrows, xcols, xmatTransposed, xmat, xmatTransposedtimesxmat); //SUCCESSFUL

	double xmatTxmatInverse[xcols][xcols];
	createIdentityMatrix(xcols, xcols, xmatTxmatInverse);
	inverse(xcols, xcols, xmatTransposedtimesxmat, xmatTxmatInverse); //SUCCESSFUL

	double inverseTimesxmatT[xcols][xrows]; //inverse is 5x5 and transpose is 5x7 so 5x7
	multiply(xcols, xcols, xcols, xrows, xmatTxmatInverse, xmatTransposed, inverseTimesxmatT);


	multiply(xcols, xrows, xrows, 1, inverseTimesxmatT, ymat, wmat);

	//DOUBLE CHECKING
	double checkY[xrows][1];
	multiply(xrows, xcols, xcols, 1, xmat, wmat, checkY);
	fclose(train);
	
	int x2rows, x2cols;
	FILE *data;
	data = fopen(argv[2], "r");
	if (data == NULL){
		printf("file doesn't exist\n");
		exit(1);
	}
	fscanf(data, "%s", trainordata); //dummy variable 
	fscanf(data, "%d", &x2cols); //first line contains attributes so columns
	x2cols++; //to account for the column of 1s
	fscanf(data, "%d", &x2rows); //second line contains number of houses so rows

	double xmat2[x2rows][x2cols];

	for (i=0; i<x2rows; i++){ //fill the first column with 1
		xmat2[i][0] = 1;
	}
	for (i=0; i<x2rows; i++){ //fills xmat and ymat 
		for (j=1; j<x2cols; j++){
		  fscanf(data, "%lf", &currentNum);
		  xmat2[i][j] = currentNum;
		}
	}

        double ymat2[x2rows][1];
	multiply(x2rows, x2cols, xcols, 1, xmat2, wmat, ymat2);
	printMatrix(x2rows, 1, ymat2);
	
	exit(EXIT_SUCCESS);
}

void multiply(int m1rows, int m1cols, int m2rows, int m2cols, double matrix1[][m1cols], double matrix2[][m2cols], double product[][m2cols]){ //** WORKS CORRECTLY
	if (m1cols != m2rows){
		printf("Error multiplying - m1 rows MUST be equal to m2 columns");
	}else{
		int i, j, k;
		double elementSum = 0;
		for (i=0; i<m1rows; i++){//iterates each ROW of m1
			for (j=0; j<m2cols; j++){ //iterates each COLUMN of m1
				for (k=0; k<m1cols; k++){ //can be either m1cols or m2rows
					elementSum += matrix1[i][k] * matrix2[k][j]; //matrix multiplication woot woot
				}
				product[i][j] = elementSum;
				elementSum = 0; //reset sum back to 0 for next element calculation
			}
		}
	}

}

void transpose(int m1rows, int m1cols, double matrix1[][m1cols], double m1Transposed[][m1rows]){ //flip rows with columns **WORKS CORRECTLY
	int i, j;
	for (i=0; i<m1rows; i++){ //iterates each ROW of m1
		for (j=0; j<m1cols; j++){ //iterates each COLUMN of m1
			m1Transposed[j][i] = matrix1[i][j];
		}
	}
}

void inverse(int m1rows, int m1cols, double matrix1[][m1cols], double identityMatrix[][m1cols]){
	int i, j, k;
	for (i=0; i<m1rows; i++){ //UPPER TRIANGULAR MATRIX
		if (matrix1[i][i] == 0){ //CANT DO ANYTHING, exit
			printf("Can't be inverted");
			return;
		}
		if (matrix1[i][i] != 1){ //check pivot STEP 1 **COMPLETE
			double pivot = matrix1[i][i];
			for (j=0; j<m1cols; j++){ //divide every element in that row by pivot
				matrix1[i][j] = matrix1[i][j]/pivot;
				if (matrix1[i][j] == 0){ //to remove -0s
					matrix1[i][j] = 0;
				}
			}
			for (j=0; j<m1cols; j++){ //divide every element in the same row as pivot in identity matrix
				identityMatrix[i][j] = identityMatrix[i][j]/pivot;
				if (identityMatrix[i][j] == 0){ //to remove -0s
					identityMatrix[i][j] = 0;
				}
			}
			//STEP 2 subtract the other rows by a multiple of row i to get 0s in column i to form UPPER TRIANGULAR MATRIX
			for (k=i+1; k<m1rows; k++){
				if (matrix1[k][i] != 0){ //check numbers DIRECTLY BELOW pivot
					double divisor = matrix1[k][i]; //constant we'll multiply to pivot and subtract from subsequent rows
					for (j=0; j<m1cols; j++){ //subtract divisor from each element in the row
						matrix1[k][j] = matrix1[k][j] - divisor*matrix1[i][j];
						if (matrix1[k][j] == 0){ //to remove -0s
							matrix1[k][j] = 0;
						}
						identityMatrix[k][j] = identityMatrix[k][j] - divisor*identityMatrix[i][j];
						if (identityMatrix[k][j] == 0){ //to remove -0s
							identityMatrix[k][j] = 0;
						}
					}
				}
			}
		}else{ //pivot is already 1, proceed
			for (k=i+1; k<m1rows; k++){
				if (matrix1[k][i] != 0){ //check numbers DIRECTLY BELOW pivot
					double divisor = matrix1[k][i]; //constant we'll multiply to pivot and subtract from subsequent rows
					for (j=0; j<m1cols; j++){ //subtract divisor from each element in the row
						matrix1[k][j] = matrix1[k][j] - divisor*matrix1[i][j];
						if (matrix1[k][j] == 0){ //to remove -0s
							matrix1[k][j] = 0;
						}
						identityMatrix[k][j] = identityMatrix[k][j] - divisor*identityMatrix[i][j];
						if (identityMatrix[k][j] == 0){ //to remove -0s
							identityMatrix[k][j] = 0;
						}
					}
				}
			}
		}

	}

	for (i=m1rows-1;i>=0;i--){ //LOWER TRIANGULAR MATRIX
		for (k=i-1; k>=0; k--){
			if (matrix1[k][i] != 0){ //check numbers DIRECTLY ABOVE pivot
				double divisor = matrix1[k][i]; //constant we'll multiply to pivot and subtract from subsequent rows
				for (j=m1cols-1; j>=0; j--){ //subtract divisor from each element in the row
					matrix1[k][j] = matrix1[k][j] - divisor*matrix1[i][j];
					if (matrix1[k][j] == 0){ // to remove -0s
						matrix1[k][j] = 0;
					}

					identityMatrix[k][j] = identityMatrix[k][j] - divisor*identityMatrix[i][j];
					if (identityMatrix[k][j] == 0){ //to remove -0s
						identityMatrix[k][j] = 0;
					}
				}
			}
		}
	}
}

void printMatrix(int rows, int cols, double matrix1[][cols]){
	int i, j;
	for (i=0; i<rows; i++){
			for (j=0; j<cols; j++){
			        printf("%.0f\n", matrix1[i][j]);
			}
		       
		}
}

void createIdentityMatrix(int m1rows, int m1cols, double m1Identity[][m1cols]){
	int i,j;
	for (i=0; i<m1rows; i++){ //create identity matrix
		for (j=0; j<m1cols; j++){
			if (i == j){
				m1Identity[i][j] = 1;
			}else{
				m1Identity[i][j] = 0;
			}
		}
	}
}
