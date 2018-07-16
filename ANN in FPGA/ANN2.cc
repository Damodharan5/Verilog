#include "ANN2.h"

dble func_sig(dble z){

	return 1/(1+exp(-z));

}

void doANN2(hls::stream<intsdch> &inputs, dble W[numI + 1], dble outputs[1]){
#pragma HLS INTERFACE bram port=outputs
#pragma HLS INTERFACE bram port=W
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE axis port=inputs

	dble z;
	dble valOut;
	intsdch valIn;
	//Training
		z = W[numI];
		for (int i = 0;i < numI;i++) {
		#pragma HLS pipeline
			valIn = inputs.read();
			z += valIn.data * W[i];
		}
		valOut = func_sig(z);
		outputs[0] = valOut;


}


