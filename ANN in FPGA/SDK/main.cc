/*
 * ANN on FPGA C++ Application
 */
#include<stdio.h>
#include<xparameters.h>
#include "xdoann2.h"
#include "xaxidma.h"
#include "platform.h"
#include <xil_printf.h>
#include <xtime_l.h>
#include "weights.h"
#include<math.h>

#define numI 28*28

XDoann2 doann2;
XDoann2_Config *doann2_cfg;
XTime tStart, tEnd;
XAxiDma axidma;
XAxiDma_Config *axidma_cfg;

void initperipheral(){

	xil_printf("Initializing the doann\n");
	doann2_cfg = XDoann2_LookupConfig(XPAR_DOANN2_0_DEVICE_ID);
	if(doann2_cfg){

		int status = XDoann2_CfgInitialize(&doann2,doann2_cfg);
		if(status != XST_SUCCESS){

			xil_printf("Error in Ann\n");
		}

	}

	xil_printf("Initializing the dma\n");
	axidma_cfg = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
	if(axidma_cfg){

		int status = XAxiDma_CfgInitialize(&axidma,axidma_cfg);
		if(status != XST_SUCCESS){

			xil_printf("Error in Ann\n");
		}

	}
	XAxiDma_IntrDisable(&axidma,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);


}

int main()
{
	initperipheral();

	double *weight = (double *)0x40000000; // BRAM for weights
	double *value  = (double *)0x42000000; // BRAM for outputs

	int count_zero = 0;
	int count_one = 0;

	for(int i=0; i<785; i++)
		weight[i] = weights[i];
	xil_printf("Sent the weights\n\n");

	unsigned int samples = (sizeof(data_in)/sizeof(data_in[0]));
	xil_printf("HW Implementation \n");
	XTime_GetTime(&tStart);
	for(unsigned int i = 0;i<samples;i++){
		XDoann2_Start(&doann2);
		
		//DMA to send the inputs starts here
			XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET, XAXIDMA_CR_RESET_MASK);
			while((XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET) & XAXIDMA_CR_RESET_MASK) == 1) {}


			u32 reg = XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET);
			XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET, reg | XAXIDMA_CR_RUNSTOP_MASK);
			XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_DESTADDR_OFFSET, (u32)data_in[i]);

			XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_BUFFLEN_OFFSET, sizeof(double)*numI);

			//while(XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET) != 4098) {}
			while(XAxiDma_ReadReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET) != 4098) {}
			// Clear Status register
			XAxiDma_WriteReg(XPAR_AXI_DMA_0_BASEADDR, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET, 4098);
		//DMA to send the inputs ends here
		
		while(!XDoann2_IsDone(&doann2));

		if(label[i] == 0 && value[0] <= 0.1)
			count_zero += 1;
		else if(label[i]==1 && value[0] >= 0.9)
			count_one +=1;

	}
	XTime_GetTime(&tEnd);
	xil_printf("Accuracy ");
	printf("%.5f\n",(float)(count_zero+count_one)/(float)samples);
	printf("Single Input took %llu clock cycles.\n", 2*(tEnd - tStart)/samples);
	printf("Single Input took %.5f s.\n", (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND))/samples);

	xil_printf("SW Implementation \n");
	double valOut;
	count_zero = 0;
	count_one = 0;
	double inp[numI];
	double z;

	XTime_GetTime(&tStart);
	#define split 14
	for(unsigned int j=0; j<samples; j++){
		for(int i=0;i<numI;i++)
			inp[i] = data_in[j][i];
		z = weights[numI];
		for (int i = 0;i < (numI)/split;i++) {
					z += (inp[split*i] * weights[split*i]) + (inp[split*i+1]* weights[split*i+1]) + (inp[split*i+2]* weights[split*i+2]) + \
							(inp[split*i+3]* weights[split*i+3]) + (inp[split*i+4] * weights[split*i+4]) + (inp[split*i+5]* weights[split*i+5]) + \
							(inp[split*i+6]* weights[split*i+6])+ \
							(inp[split*i+7] * weights[split*i+7]) + (inp[split*i+8]* weights[split*i+8]) + (inp[split*i+9]* weights[split*i+9]) + \
							(inp[split*i+10]* weights[split*i+10]) + (inp[split*i+11] * weights[split*i+11]) + (inp[split*i+12]* weights[split*i+12]) + \
							(inp[split*i+13]* weights[split*i+13]);
		}
		valOut = 1/(1+exp(-z));
		if(label[j] == 0 && valOut <= 0.1)
				count_zero += 1;
			else if(label[j]==1 && valOut >= 0.9)
				count_one +=1;
	}
	XTime_GetTime(&tEnd);
	xil_printf("Accuracy ");
	printf("%.5f\n",(float)(count_zero+count_one)/(float)samples);
	printf("Single Input took %llu clock cycles.\n", 2*(tEnd - tStart)/samples);
	printf("Single Input took %.5f s.\n", (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND))/samples);
	return 0;
}
