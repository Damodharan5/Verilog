#ifndef ANN_H_
#define ANN_H_
#include<hls_stream.h>
typedef struct {
	double data;
	bool last;
}intsdch;

#define numI 28*28

#define WDATA_MODE 1
#define T_MODE 2

#define SIGMOID 0
#define RELU 1
#define SOFTMAX 3

int N_Layers = 2;
double buff[3][numI];
int NumN[] = { 784,12,10};

// Layer Structure
template <const int N_In, const int N_N>
struct synth_layer
{
	int func;

	double W[N_N][N_In];
	double b[N_N];
	double y[N_N];
	double z[N_N];

};

typedef synth_layer<784, 12> type_0;
typedef synth_layer<12, 10> type_1;

void doANNL(hls::stream<intsdch> &inputs, double outputs[10],int mode );

typedef struct {

	unsigned int magic_number;
	unsigned int number_images;
	unsigned int row_size;
	unsigned int col_size;

}myData;

typedef struct {

	unsigned int magic_number;
	unsigned int number_labels;

}myLabel;

myData tr_data;
myLabel tr_label;
char *train_img;
char train_label;
double y;

#endif
