`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/22/2017 06:45:43 PM
// Design Name: 
// Module Name: CLK_GEN
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module CLK_GEN(
    input CLK,
    output reg pixel_clock
    );
    
        parameter N = 2;	// parameter for clock division
        //reg clk_25Mhz;
        reg [N-1:0] count;
        initial begin pixel_clock <= 0; count <= 0; end
        always @ (posedge CLK) begin
                count <= count + 1'b1;
                pixel_clock <= count[1];
            end
endmodule
