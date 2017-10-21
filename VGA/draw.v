module draw(
    input pixel_clock,
    input rst,
    input sw0,
    input sw1,
    input [9:0] hPos,
    input [9:0] vPos,
    input videoOn,
    output reg [4:0] R,
    output reg [5:0] G,
    output reg [4:0] B
    );
    
wire [15:0] bgclr;
reg [9:0] x;
reg [9:0] y;
reg [9:0] x0;
reg [9:0] y0;
reg [9:0] x1;
reg [9:0] y1;
reg x_flag;
reg y_flag;

integer counter;

parameter radius_sq = 400;
parameter radius = 20;

initial begin 

x <= 100;
y <= 200;
x0 <= 100; 
y0 <= 425;
x1 <= 200;
y1 <= 450;
x_flag <= 0;
y_flag <= 0;
counter <= 0;

end
//Kinda reset

//Ball animation and bounce block and paddle move
always @(posedge pixel_clock) begin
if(counter > 961762) begin    // This number facilitate smooth flow.
    if(x < radius)
    begin
        x_flag <= 1;
    end
    if(y < radius)
    begin
        y_flag <= 1;
    end
    if(x > 640 - radius )
    begin
        x_flag <= 0;
    end
    if(x >= x0 && x < x1 && y > y0 - radius) 
    begin
        y_flag <= 0;
    end
    if(sw1 ) begin
        if(x0 > 5)
            x0 <= x0 - 10;
    end
    else begin
        if(x1 < 635)
            x0 <= x0 + 10;
    end

x1 <= x0 + 100;

x <= (x_flag?x+5:x-5);
y <= (y_flag?y+5:y-5);

counter <= 0;
end
else begin
counter <= counter + 1;
end
end
//
assign bgclr = sw0?16'hFFFF:16'h0000;    

always @(posedge pixel_clock) begin

    if(rst == 1) begin
        R <= 0;
        G <= 0;
        B <= 0;
    end
    else begin
        if (videoOn == 1) begin
           if((hPos-x)**2 + (vPos-y)**2 <= radius_sq) begin  // Ball
                R <= 5'h1F;
                G <= 0;
                B <= 0;
            end
            else if(hPos >= x0 && hPos < x1 && vPos >= y0 && vPos < y1 ) begin // Paddle
                R <= 0;
                G <= 6'h3f;
                B <= 5'h1F;
            end 
            else begin
                R <= bgclr[15:11];
                G <= bgclr[10:5];
                B <= bgclr[4:0];
            end
            
        end
        else begin
            R <= 0;
            G <= 0;
            B <= 0;        
        end
    end
end
endmodule
