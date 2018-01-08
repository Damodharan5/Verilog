#include<hls_stream.h>
#include<ap_axi_sdata.h>
#include<math.h>
#include<stdio.h>
#include<ap_int.h>
#define Dim 8



typedef ap_axiu<32,2,5,6> intSdch;
void doKNN(hls::stream<intSdch> &inStream, ap_uint<64> result[18]);
//void doKNN(hls::stream<intSdch> &inStream, hls::stream<intSdch> &outStream);

int main()
{

hls::stream<intSdch> inputStream,outputStream;
ap_uint<64> result[18];

for(int idx=0;idx<65537*Dim;idx++){

	intSdch valIn;
	valIn.data = m_data[idx];
	valIn.keep = 1;valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
	inputStream << valIn;

}

doKNN(inputStream,result);


for(int idx=0;idx<18;idx++){

	printf("\nValue is %u %u",(unsigned int)result[idx].range(63,32),(unsigned int)result[idx].range(31,0));
}


}