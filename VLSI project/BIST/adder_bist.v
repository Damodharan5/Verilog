`timescale 1ns / 1ps
//Compactor MISR
module MISR(seed_in,clk,reset,out);
input [7:0]seed_in;
input clk,reset;

output reg [7:0]out;

wire c1,c2,c3,c4,c5,c6,c7,c8;

assign c1 = seed_in[0] ^ out[7] ;
assign c2 = seed_in[1] ^ out[7] ^ out[0] ;
assign c3 = seed_in[2] ^ out[1];
assign c4 = seed_in[3] ^ out[2];
assign c5 = seed_in[4] ^ out[3];
assign c6 = seed_in[5] ^ out[7] ^ out[4];
assign c7 = seed_in[6] ^ out[7] ^ out[5];
assign c8 = seed_in[7] ^ out[6];

always @(posedge clk or negedge reset)
begin 

if(~reset) out = 0;
else out = {c1,c2,c3,c4,c5,c6,c7,c8};

end
endmodule
//Mode select..Normal vs Test
module mode_select(x,y,a1,a2,mode,clk,out1,out2);

input [7:0]x;
input [7:0]y;
input [7:0]a1;
input [7:0]a2;

input mode,clk;

output reg [7:0]out1;
output reg [7:0]out2;

always @(*)
begin

if(mode) {out1,out2} = {a1,a2};
else {out1,out2} = {x,y};

end
endmodule

module LFSR_16(a,clk,reset,z
    );

input [15:0]a;
input clk,reset;

output reg [15:0]z;

wire f1,f2,f3;

assign f1 = z[0] ^ z[2];
assign f2 = z[3] ^ f1;
assign f3 = z[5] ^ f2;

always @(posedge clk or negedge reset)
begin

if(~reset)z = a;
else z = {f3,z[15],z[14],z[13],z[12],z[11],z[10],z[9],z[8],z[7],z[6],z[5],z[4],z[3],z[2],z[1]};

end
endmodule

module FA(x,y,z,s,c);
input x,y,z;
output s,c;

assign {c,s} = x + y + z;

endmodule

module FA1(x,y,z,s);
input x,y,z;
output s;

assign s = x ^ y ^ z;

endmodule

module ripple(x1,y1,z1,cin
    );
input [7:0]x1;
input [7:0]y1;
input cin;
output [7:0]z1;

wire c1,c2,c3,c4,c5,c6,c7;

FA U1(.x(x1[0]),.y(y1[0]),.z(cin),.s(z1[0]),.c(c1));
FA U2(.x(x1[1]),.y(y1[1]),.z(c1),.s(z1[1]),.c(c2));
FA U3(.x(x1[2]),.y(y1[2]),.z(c2),.s(z1[2]),.c(c3));
FA U4(.x(x1[3]),.y(y1[3]),.z(c3),.s(z1[3]),.c(c4));
FA U5(.x(x1[4]),.y(y1[4]),.z(c4),.s(z1[4]),.c(c5));
FA U6(.x(x1[5]),.y(y1[5]),.z(c5),.s(z1[5]),.c(c6));
FA U7(.x(x1[6]),.y(y1[6]),.z(c6),.s(z1[6]),.c(c7));
FA1 U8(.x(x1[7]),.y(y1[7]),.z(c7),.s(z1[7]));

endmodule

module ripple_lfsrastb(x,y,cin,mode,seed,clk,reset,out
    );
input [7:0]x;
input [7:0]y;
input mode,cin;	 
input [15:0]seed;
input clk,reset;

output [7:0]out;
//output [7:0]x2;
//output [7:0]y2;

wire [7:0]out1;
wire [7:0]out2;
wire [7:0]out3;
wire [15:0]c1;


//assign {x2,y2} = c1;
mode_select U0(.x(x),.y(y),.a1(c1[15:8]),.a2(c1[7:0]),.mode(mode),.clk(clk),.out1(out1),.out2(out2));
LFSR_16 U1(.a(seed),.clk(clk),.reset(reset),.z(c1)); // LFSR for input pattern
ripple U2(.x1(out1),.y1(out2),.z1(out3),.cin(cin)); // CUT
MISR U3(.seed_in(out3),.clk(clk),.reset(reset),.out(out)); // Compactor
endmodule
