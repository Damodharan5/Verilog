#include"ANN.h"
#include<iostream>
#include<algorithm>
#include<math.h>

using namespace std;

template <class Lt>
void layer_forward(Lt &Wyb, int l);

intsdch valIn;

void doANNL(hls::stream<intsdch> &inputs, double outputs[10],int mode ){
#pragma HLS INTERFACE bram port=outputs
#pragma HLS INTERFACE s_axilite port=mode bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE axis port=inputs

	type_0 layer_0;
	type_1 layer_1;
	layer_0.func = RELU;
	layer_1.func = SOFTMAX;

		if (mode == WDATA_MODE){
			for (int n = 0; n<NumN[1]; n++)
			{

				for (int i = 0; i<NumN[0]; i++)
				{
					# pragma HLS pipeline
					valIn = inputs.read();
					layer_0.W[n][i] = valIn.data;
				}

			}
			for (int n = 0; n < NumN[1]; n++)
			{
				# pragma HLS pipeline
				valIn = inputs.read();
				layer_0.b[n] = valIn.data;
			}

			for (int n = 0; n<NumN[2]; n++)
			{

				for (int i = 0; i<NumN[1]; i++)
				{
					# pragma HLS pipeline
					valIn = inputs.read();
					layer_1.W[n][i] = valIn.data;
				}

			}
			for (int n = 0; n < NumN[2]; n++)
			{
				# pragma HLS pipeline
				valIn = inputs.read();
				layer_1.b[n] = valIn.data;
			}

		}
		else if(mode == T_MODE){

			for (int i = 0;i < numI;i++) {
						# pragma HLS pipeline
						valIn = inputs.read();
						buff[0][i] = valIn.data;
			}
			layer_forward<type_0>(layer_0, 0);
			layer_forward<type_1>(layer_1, 1);
			for(int i=0;i<10;i++)
				outputs[i] = buff[2][i];

		}


}


template <class Lt>
void layer_forward(Lt &Wyb, int l){

	double sum_softmax = 0.0;
		for (int n = 0; n<NumN[l+1]; n++)
		{
			Wyb.z[n] = Wyb.b[n];
			for (int i = 0; i<NumN[l]; i++)
			{
				Wyb.z[n] += buff[l][i] * Wyb.W[n][i] ;
			}
		}
		for (int n = 0; n<NumN[l+1]; n++)
		{
#pragma HLS unroll
			if (Wyb.func == SIGMOID) {
				Wyb.y[n] = 1.0 / (1.0 + exp(-Wyb.z[n]));
			}
			else if (Wyb.func == SOFTMAX) {
				Wyb.y[n] = exp(Wyb.z[n]);
				sum_softmax += Wyb.y[n];
			}
			else if(Wyb.func == RELU)
				Wyb.y[n] = max(0.0,Wyb.z[n]);
		}
		for (int n = 0;n < NumN[l + 1];n++) {
#pragma HLS unroll
			buff[l + 1][n] = Wyb.y[n];
			if (Wyb.func == SOFTMAX)
				buff[l + 1][n] /= sum_softmax;
		}

}
