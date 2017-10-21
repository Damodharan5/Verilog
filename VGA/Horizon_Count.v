`include "SVGA_DEFINES.v"

module HV_SYNC(
     CLK,
     rst,
     hPos,
     vPos,
     HSYNC,
     VSYNC,
    videoOn 
    );
  input CLK;
  input rst;
  output reg [9:0]hPos;
  output reg [9:0]vPos;
    
  output reg HSYNC;
  output reg VSYNC;
  output reg videoOn;
initial begin hPos <= 10'h0; vPos <= 10'h0; HSYNC <= 1;VSYNC <= 1; videoOn<= 1; end
// Vertical Line count
always @(posedge CLK) begin
    if(rst == 1) begin
        vPos <= 0;
    end
    else begin
        if(hPos == `H_TOTAL) begin
            if(vPos == `V_TOTAL) begin
                vPos <= 0;
            end
            else begin
                vPos <= vPos + 1;
            end
        end  
    end
end
//Horizontal Pixel count
always @(posedge CLK) begin
    if(rst == 1) begin
        hPos <= 0;
    end
    else begin
        if(hPos == `H_TOTAL) begin
            hPos <= 0;
        end
        else begin
            hPos <= hPos + 1;
        end
    end  
end
// Hsync signal generation
always @(posedge CLK) begin
    if(rst == 1) begin
        HSYNC <= 0;
    end
    else begin
        if( (hPos <= (`H_ACTIVE + `H_FRONT_PORCH)) || (hPos > (`H_ACTIVE + `H_FRONT_PORCH + `H_SYNCH)) ) begin
            HSYNC <= 1;
        end
        else begin
            HSYNC <= 0;
        end
    end  
end
// Vsync signal generation
always @(posedge CLK) begin
    if(rst == 1) begin
        VSYNC <= 0;
    end
    else begin
        if( (vPos <= (`V_ACTIVE + `V_FRONT_PORCH)) || (vPos > (`V_ACTIVE + `V_FRONT_PORCH + `V_SYNCH)) ) begin
            VSYNC <= 1;
        end
        else begin
            VSYNC <= 0;
        end
    end  
end
//VideoOn function
always @(posedge CLK) begin
    if(rst == 1) begin
        videoOn <= 0;
    end
    else begin
        if(hPos <= `H_ACTIVE && vPos <= `V_ACTIVE) begin
            videoOn <= 1;
        end
        else videoOn <= 0;
    end
end
endmodule

