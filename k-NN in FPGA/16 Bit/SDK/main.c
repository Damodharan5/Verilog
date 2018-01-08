// Code for 16-Dimensions
#include<stdio.h>
#include<xparameters.h>
#include "xdoknn.h"
#include "xaxidma.h"
#include "DataSet16.h"
#include "platform.h"
#include <xil_printf.h>
#include <xtime_l.h>

//DMA and Our KNN IP variables for configuration the IP cores.
XDoknn doknn;
XDoknn_Config *doknn_cfg;
XAxiDma axidma;
XAxiDma_Config *axidma_cfg;

void initperipheral(){

	xil_printf("Initializing the doKnn\n");
	doknn_cfg = XDoknn_LookupConfig(XPAR_DOKNN_0_DEVICE_ID);
	if(doknn_cfg){

		int status = XDoknn_CfgInitialize(&doknn,doknn_cfg);
		if(status != XST_SUCCESS){

			xil_printf("Error in knn\n");
		}

	}

	xil_printf("Initializing the dma\n");
	axidma_cfg = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
	if(axidma_cfg){

		int status = XAxiDma_CfgInitialize(&axidma,axidma_cfg);
		if(status != XST_SUCCESS){

			xil_printf("Error in knn\n");
		}

	}
	XAxiDma_IntrDisable(&axidma,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);


}

int main()
{
		XTime tStart, tEnd;
	    unsigned long long int result_data[9],result_loc[9];
	    initperipheral();

	    printf("-- Input array address %p\n\r", m_data);
	    XTime_GetTime(&tStart);

	    XDoknn_WriteReg(XPAR_DOKNN_0_S_AXI_CRTL_BUS_BASEADDR, XDOKNN_CRTL_BUS_ADDR_AP_CTRL, 1);

	    XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET, XAXIDMA_CR_RESET_MASK);
	    while((XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET) & XAXIDMA_CR_RESET_MASK) == 1) {}


	    u32 reg = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET);
	    XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET, reg | XAXIDMA_CR_RUNSTOP_MASK);
	    XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_DESTADDR_OFFSET, (u32)m_data);

	    XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_BUFFLEN_OFFSET, 65537*16*4);

	    //while(XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET) != 4098) {}
	    while(XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET) != 4098) {}
	    // Clear Status register
	    XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET, 4098);

	    // Print out the outputs.
	    // Better way is to check the HW outputs with the SW code outputs.
	    while(!XDoknn_IsDone(&doknn));
	    XTime_GetTime(&tEnd);

	        printf("Output took %llu clock cycles.\n", 2*(tEnd - tStart));
	        printf("Output took %.2f s.\n",
	               1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));

	    printf("-- Printing outputs -- \n\r");
	    u32 outbuff = 0x40000000; // BRAM starting address for the output. Both Distance and the index of the distance.
	    u32 *outbuff1= (u32*)(outbuff);
	    for(int i=0;i<18;i=i+2){

	    	xil_printf("Index - [%ld] | Distance %ld \n\r",*(outbuff1+i+18),*(outbuff1+i));

	    }

	    printf("\r\n --- Test HW Finished ---\n\r");

	    printf("\r\n ---Software Test Started --- \n\r");
	    XTime_GetTime(&tStart);

	    unsigned int ref[16];
	    unsigned int valIn;

	    for(int idfill = 0;idfill<9;idfill++)
	    {
	    		result_data[idfill] = 0xFFFFFFFFFFFFFFFF;
	    		result_loc[idfill] = 0;
	    }
	    //Reference filling
	    for(int idd=0;idd<16;idd++)
	    {
	    	ref[idd] = m_data[idd];

	    }
	    for(int idx=1;idx<=65536;idx++){

	    	unsigned long long int sum = 0;
	    	for(int idz=0;idz<16;idz++){

	    			valIn  = m_data[(idx*16)+idz];
	    			unsigned long long int temp;
	    			temp = (valIn>ref[idz])?valIn-ref[idz]:ref[idz]-valIn;
	    			sum = sum + (temp * temp);
	    //			valOut = valIn;
	    //			valOut.data = sum;
	    //			outStream.write(valOut);

	    }//for

	    	unsigned long long int prev_data;
	    	unsigned long long int prev_loc;
	    	int flag = 0;
	    	for(int idy=0;idy<9;idy++){
	    		if(sum < result_data[idy] && flag ==0)
	    		{
	    			prev_data = result_data[idy];
	    			prev_loc = result_loc[idy];
	    			result_data[idy] = sum;
	    			result_loc[idy] = idx-1;
	    			flag = 1;
	    		}
	    		else if(flag == 1)
	    		{
	    			unsigned long long int temp_data,temp_loc;

	    			temp_data = prev_data;
	    			prev_data = result_data[idy];
	    			result_data[idy] = temp_data;

	    			temp_loc = prev_loc;
	    			prev_loc = result_loc[idy];
	    			result_loc[idy] = temp_loc;

	    		}

	    	}//for

	    }
	    XTime_GetTime(&tEnd);
	    for(int i=0;i<9;i=i+1){

	    	    	printf("Index - [%ld] | Distance %ld \n\r",(unsigned int)result_loc[i],(unsigned int)result_data[i]);

	   }

	    printf("\r\n --- Test SW Finished ---\n\r");
	    printf("Output took %llu clock cycles.\n", 2*(tEnd - tStart));
	    	        printf("Output took %.2f s.\n",
	    	               1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));


	    return 0;
}

