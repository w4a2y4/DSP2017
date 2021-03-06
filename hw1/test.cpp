#include "hmm.h"
#include <math.h>

#define MODEL_MAX_NUM 10

// ./test modellist.txt testing_data.txt result.txt

int main( int argc, char* argv[] ) {

	HMM model[MODEL_MAX_NUM];
	char model_name[MODEL_MAX_NUM][256];
	
	// read models
	FILE *list = open_or_die(argv[1], "r");
	int model_num = 0;
	while ( fscanf( list, "%s", model_name[model_num] ) != EOF ) {
		loadHMM( &model[model_num], model_name[model_num] );
		model_num++;
	}
	fclose(list);

	// process testing data
	FILE *testing_data = open_or_die(argv[2], "r");
	FILE *result = open_or_die(argv[3], "w");
	FILE *ans = open_or_die("testing_answer.txt", "r");
	char buff[MAX_SEQ];
	double acc = 0, cnt = 0; 

	while ( fscanf( testing_data, "%s", buff ) != EOF ) {

		int T = strlen(buff);
		int data[MAX_SEQ];
		for ( int i = 0; i < T; i++ )
			data[i] = buff[i] - 'A';

		// viterbi for each model
		double max_prob = 0;
		int max_num = -1;
		for ( int x = 0; x < model_num; x++ ) {

			// calculate delta
			double delta[MAX_SEQ][MAX_STATE] = {{0}};
			for ( int t = 0; t < T; t++ ) {
				for ( int j = 0; j < model[x].state_num; j++ ) {
					double prev = 0;
					if ( t == 0 )
						prev = model[x].initial[j];
					else {
						for ( int i = 0; i < model[x].state_num; i++ )
							if ( delta[t-1][i] * model[x].transition[i][j] > prev )
								prev = delta[t-1][i] * model[x].transition[i][j];
					}
					delta[t][j] = prev * model[x].observation[data[t]][j];
				}
			}			

			// terminate
			for ( int i = 0; i < model[x].state_num; i++ )
				if ( delta[T-1][i] > max_prob ) {
					max_prob = delta[T-1][i];
					max_num = x;
				}

		}

		fscanf( ans, "%s", buff );
		if( buff[7] == model_name[max_num][7] ) acc++;
		cnt++; 

		fprintf(result, "%s\n", model_name[max_num]);
	}

	fclose(testing_data);
	fclose(result);
	fclose(ans);

	printf("acc: %f\n", (acc/cnt));

	return 0;
}
