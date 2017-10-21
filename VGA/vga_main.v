//https://www.youtube.com/watch?v=eJMYVLPX0no
module vga_main(
    input CLK,
    input sw0,
    input sw1,
    output HSYNC,
    output VSYNC,
    output [4:0] R,
    output [5:0] G,
    output [4:0] B
    );

reg rst;
wire videoOn;
wire [9:0]hPos;
wire [9:0]vPos;
wire  data;
wire [18:0] addr;
reg wea;
reg dataa;

initial begin
    rst <= 0;
    wea <= 0;
end

CLK_GEN CLK_GEN(
    CLK,
    pixel_clock
    );

HV_SYNC HV_SYNC(pixel_clock,
    rst,
    hPos,
    vPos,
    HSYNC,
    VSYNC,
    videoOn
    );
//Ball Bouncing draw block
draw draw(
        pixel_clock,
        rst,
        sw0,
        sw1,
        hPos,
        vPos,
        videoOn,
         R,
         G,
         B
        );
/*
img_ram img_ram(
pixel_clock,
wea,
addr,
dataa,

pixel_clock,
addr,
data
);
draw_ram draw_ram(
        pixel_clock,
        rst,
        sw0,
        hPos,
        vPos,
        data,
        addr,
        videoOn,
         R,
         G,
         B
        );
        */
endmodule
