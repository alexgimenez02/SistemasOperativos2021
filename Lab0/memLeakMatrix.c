#include <stdlib.h>


int** createMatrix( int n, int m) {
    int i;
    int** mat = malloc( n * sizeof(int*) );
    for(i=0;i<n;i++) {
        mat[i] = malloc( m * sizeof(int) );
    }
    return mat;
}

void freeMatrix(int** mat, int n, int m) {
    for(int i = 0; i<n;i++){
        free(mat[i]);
    }
    free(mat);
}


int main() {
    int n = 5;
    int m = 10;
	int** mat = createMatrix(n,m);
    
    freeMatrix(mat,n,m);
    //Intenta acceder a una matriz que ha sido liberada de memoria
    //mat[4][9] = 0;   // fill two positions
    //mat[0][0] = 0;

	return 0;
}