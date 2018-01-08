#include<hls_stream.h>
#include<ap_axi_sdata.h>
#include<math.h>
#include<ap_int.h>
#define K 9
#define Dim 8

typedef ap_axiu<32,2,5,6> intSdch;

void doKNN(hls::stream<intSdch> &inStream, ap_uint<64> result[18])
{

#pragma HLS INTERFACE bram port=result
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE axis port=inStream

unsigned int ref[Dim];
ap_uint<64> sum;
ap_uint<64> result_data[K];
ap_uint<64> result_loc[K];
intSdch valIn;

//fill the slots with maximum number
for(int idfill = 0;idfill<K;idfill++)
{
		result_data[idfill] = 0x7FFFFFFFFFFF;
		result_loc[idfill] = 0;
}
//Reference filling
for(int idd=0;idd<Dim;idd++)
{	valIn  = inStream.read();
	ref[idd] = valIn.data;

}
for(int idx=0;idx<65536;idx++){
#pragma HLS PIPELINE

	sum = 0;
  //Distance calculation
	for(int idz=0;idz<Dim;idz++){

			valIn  = inStream.read();
			ap_uint<64> temp;
			temp = (valIn.data>ref[idz])?valIn.data-ref[idz]:ref[idz]-valIn.data;
			sum = sum + (temp * temp);
//			valOut = valIn;
//			valOut.data = sum;
//			outStream.write(valOut);

}//for

	ap_uint<64> prev_data;
	ap_uint<64> prev_loc;
	int flag = 0;
	//Sorting here
	for(int idy=0;idy<K;idy++){
		if(sum < result_data[idy] && flag ==0)
		{
			prev_data = result_data[idy];
			prev_loc = result_loc[idy];
			result_data[idy] = sum;
			result_loc[idy] = idx;
			flag = 1;
		}
		else if(flag == 1)
		{
			ap_uint<64> temp_data,temp_loc;

			temp_data = prev_data;
			prev_data = result_data[idy];
			result_data[idy] = temp_data;

			temp_loc = prev_loc;
			prev_loc = result_loc[idy];
			result_loc[idy] = temp_loc;

		}

	}//for

}//external for


for(int idf=0;idf<K*2;idf++){
	result[idf] = (idf<9)?result_data[idf]:result_loc[idf-9];

}//for

}//Main
