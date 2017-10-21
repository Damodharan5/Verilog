// DEFINE THE VARIOUS PIPELINE DELAYS

//http://tinyvga.com/vga-timing/640x480@73Hz

//  640 X 480 @ 60Hz with a 25.175MHz pixel clock
`define H_ACTIVE			639	// pixels
`define H_FRONT_PORCH	24		// pixels
`define H_SYNCH			40		// pixels
`define H_BACK_PORCH		128		// pixels
`define H_TOTAL			831	// pixels

`define V_ACTIVE			479	// lines
`define V_FRONT_PORCH	9		// lines
`define V_SYNCH			2		// lines
`define V_BACK_PORCH		29		// lines
`define V_TOTAL			519	// lines



