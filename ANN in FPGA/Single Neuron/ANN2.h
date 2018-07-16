#ifndef ANN2_H_
#define ANN2_H_

#include<hls_stream.h>
#include<ap_axi_sdata.h>

//typedef sc_fixed<64,8> dble;
typedef double dble;
typedef struct {
	dble data;
	bool last;
}intsdch;
#define WDATA_MODE 1
#define T_MODE 2

#define IDLE 3

#define NBITS 32
#define IBITS 8

#define numI 28*28

dble func_sig(dble);
void doANN2(hls::stream<intsdch> &inputs, dble W[numI + 1], dble outputs[1]);

#endif
