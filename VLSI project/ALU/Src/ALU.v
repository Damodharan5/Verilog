`timescale 1ns / 1ps
////////////////////////////////////////////////////////////
module aluu(
    a,
    b,
    status_in,
    op,
    clk,
	 rst,
    status_out,
//c0 is the Output bits for all operations. For Mul operations, output is {c1,c0}
//as multiplication of two 16 bit number resuts in maximum of 32 bit number.
    c0,  
    c1
    );
	 input [15:0] a,b;
	 input [4:0] status_in;
	 input [4:0] op;
	 input clk,rst;
	 output [15:0] c0;
	 output [15:0] c1;
	 output [4:0] status_out;
	reg [4:0] status_out;
// status register in test_bench - 0:Carry bit 1:Zero bit 2:Borrow bit 3:Parity bit 4:NaN
	reg [15:0] c0,c1;
	reg [33:0] temp; // For the rotate function
	
//Opcode written as parameters.
	parameter
	Add=5'b00000,
	Adc=5'b00001,
	Sub=5'b00010,
	Sbb=5'b00011,
	inc=5'b00100,
	dec=5'b00101,
	Mul=5'b00110,
	Div=5'b00111,
	And=5'b01000,
	Or =5'b01001,
	Not=5'b01010,
	Cpr=5'b01011,
	Cmp=5'b01100,
	Xor=5'b01101,
	Lst=5'b01110,
	Rst=5'b01111,
	Clf=5'b10000, // Clear status_reg
	Stf=5'b10001, // Set Status_reg with a as input
	Clc=5'b10010, // Clear Carry bit in the status_reg
	Clp=5'b10011, // Clear Parity bit in the status_reg
	Clb=5'b10100, // Clear Borrow bit in the status_reg
	Clz=5'b10101, // Clear Zero bit in the status_reg
	Stc=5'b10110,
	Stp=5'b10111,
	Stb=5'b11000,
	Stz=5'b11001,
	Inx=5'b11010, // Increment 32 bit combined a nd b as 32 bit number
	Dcx=5'b11011, // Decrement 32 bit combined a nd b as 32 bit number
	Rol=5'b11100, // Rotate left
	Rcl=5'b11101, // Rotate Left with Carry
	Ror=5'b11110, // Rotate Right
	Rcr=5'b11111; // Rotate right with Carry
//Beginning of the always block.	
	always@(posedge clk) begin
	
		if(rst) begin
			c0=0;
			c1=0;
			status_out = status_in;
		end
		else begin
	
			case(op)
				Add:begin{status_out[0],c0} = a+b; c1=0; end
				Adc:begin {status_out[0],c0} = a+b+status_in[0]; c1=0; end
				Sub:begin c0 = a-b;c1=0;if(b>a)begin status_out[2]=1; end else begin status_out[2]=0; end end
				Sbb:begin c0 = a-b-status_in[2]; c1=0;if(b+status_in[2]>a)begin status_out[2]=1; end else begin status_out[2]=0; end end
				inc:begin {status_out[0],c0} = a+1;c1=0;end
				dec:begin c0 =a-1; c1=0;end
				Mul:begin {c1,c0} = a*b;end
				Div:begin if(b==0) begin status_out[4] = ~(status_in[4] ^ status_in[4]);c0=0;c1=0; end
							 else begin c0=a/b; c1=a%b; end
				end
				And:begin c0=a & b;c1=0; end
				Or: begin c0=a | b; c1=0;end
				Not:begin c0=~a; c1=0;end
				Cpr:begin 
					if(a==b) begin status_out[0]=1;status_out[2]=0;end 
					else if(a>b) begin status_out[2]=0;status_out[0]=0; end 
					else begin status_out[2]=1;status_out[0]=0; end
					c0=0;c1=0;
				end
				Cmp:begin c0=(~a)+1;c1=0; end
				Xor:begin c0=a ^ b; c1=0; end
				Lst:begin c0=a<<b;c1=0; end
				Rst:begin c0=a>>b; c1=0;end
				Clf:begin status_out = status_in^status_in; c0=0;c1=0;end
				Stf:begin status_out[3:0] = a[3:0];c0=0;c1=0; end
				Clc:begin status_out[0] = status_in[0] ^ status_in[0];c0=0;c1=0; end
				Clp:begin status_out[3] = status_in[3] ^ status_in[3]; c0=0;c1=0;end
				Clb:begin status_out[2] = status_in[2] ^ status_in[2];c0=0;c1=0; end
				Clz:begin status_out[1] = status_in[1] ^ status_in[1]; c0=0;c1=0;end
				Stc:begin status_out[0]=~(status_in[0] ^ status_in[0]);c0=0;c1=0;end
				Stp:begin status_out[3]=~(status_in[3] ^ status_in[3]);c0=0;c1=0;end
				Stb:begin status_out[2]=~(status_in[2] ^ status_in[2]);c0=0;c1=0;end
				Stz:begin status_out[1]=~(status_in[1] ^ status_in[1]);c0=0;c1=0;end
				Inx:begin {status_out[0],c1,c0} = {b,a}+1'b1; end
				Dcx:begin {c1,c0} = {b,a}-1'b1; end
				Rol:begin 
					temp=({a,a}<<b[3:0]);
					c0=temp[31:16];
					c1=0;
					end
				Rcl:begin
					temp={a,status_in[0],a,status_in[0]}<<(b[3:0]);c1=0;
					c0=temp[33:18];
				end
				Ror:begin
					c0={a,a}>>b[3:0];c1=0;
					end
				Rcr:begin
					{c0,status_out[0]}={a,status_in[0],a,status_in[0]}>>(b[3:0]);c1=0;
					end
				default: $display("invalid entry");
			endcase
		
				if((op<16||op>25)) begin
					status_out[3] = c0[0]^c0[1]^c0[2]^c0[3]^c0[4]^c0[5]^c0[6]^c0[7]^c0[8]^c0[9]^c0[10]^c0[11]^c0[12]^c0[13]^c0[14]^c0[15];
					end
				if(op!=7)begin status_out[4] = 0;end
			   if((op<16||op>25)) begin status_out[1]= (status_in[1]&0) | (c0==0 && c1 ==0); end
			
		end
   end

endmodule
//Test Bench Module begins here
module test_bench1;

wire [15:0]c0;
wire [15:0]c1;
wire [4:0]st_out;
reg [15:0]a;
reg [15:0]b;
reg [4:0]st_reg;
reg [4:0] op;
reg clk;
reg rst;

integer f;
aluu uut(
	a,b,st_reg,op,clk,rst,st_out,c0,c1);

initial begin
f = $fopen("output_beha.txt","w"); // To get the simulation result in a text documents.
a = 0;
b = 0;
rst = 1;
clk = 0;
st_reg = 0;
op = 0;
#50 rst = 0;
a = 65535;
b = 65530;
#20 op = 1;
#20 op = 2;
#20 op = 3;
#20 op = 4;
#20 op = 5;
#20 op = 6;
#20 op = 7;
#40 a=0;b=0;op=7;
#20 a=5;b=10;op = 8;
#20 op = 9;
#20 op = 16;
#20 op = 10;
#20 op = 11;
#20 op = 12;
#20 op = 13;
#20 a=9;b=1;op = 14;
#20 op = 15;
#20 a=45;b=2;op = 17;
#20 a=65535;op = 18;
#20 op=19;
#20 op=20;
#20 op=21;
#20 op=22;
#20 op=23;
#20 op=24;
#20 op=25;
#20 op = 26;
#20 a=0;b=2;op = 27;
#20 a = 13741;b=0;op=28;
#20 b = 15; op=28;
#20 b = 17; op = 28;
#20 b=2;op=29;
#20 op = 30;
#20 b=15;op = 30;
#20 b=1;op=31;
#45 $finish;
end
always begin
#10 clk=~clk;
end
always@(st_out) begin
st_reg = st_out;
end
always@(posedge clk) begin
$fwrite(f,"%b  %b  %b  %d  %b  %b  %b\n",a,b,st_reg,op,c0,c1,st_out); // For every Opcode 
//it will write the file.
end

endmodule
