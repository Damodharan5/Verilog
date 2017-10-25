$a sample hspice file
.include "/home/cad/kits/IBM_CMRF8SF-LM013/IBM_PDK/cmrf8sf/relLM/HSPICE/models/model013.lib_inc"
.include invv.sp
.option post
.Param wn='0.3U'
xi in out invv        $ Call your system to simulate. put I/O info first and the ends with subckt name
vdd vdd! gnd! 1.2V          $ Your VDD voltage value
        Vin in gnd! pulse (0V 1.2V 0ns 85ps 85ps 1000ps 2000ps) $ How input varies during simulation
        Cout_b out 0 75f $ output load
.tr 0.01ns 4ns sweep wn 0.28u 5.00u 0.01u
                      .measure tran t1 when v(in)='0.9*1.2' fall=1 $ Measures start time t1
                      .measure tran t2 when v(out)='0.9*1.2' rise=1 $ Measure end time t2
                      .measure tran t3 when v(in)='0.1*1.2' rise=1 $ Measures start time t3
                      .measure tran t4 when v(out)='0.1*1.2' fall=1 $ Measures end time t4
                      .measure tplh trig v(in) val =0.6 fall=1 targ v(out) val=0.6 rise=1
.print tplh
.measure tphl trig v(in) val =0.6 rise=1 targ v(out) val=0.6 fall=1
.print tphl
.measure delay param='max(tplh,tphl)' $ Measures Delay(worst case delay)
 .print delay
.measure delaydiff param='max(tplh,tphl)-min(tplh,tphl)' $ Measures Delay Difference

.print delaydiff
.measure tran current avg I(vdd) from=t1 to=t2
.measure tran current_2 avg I(vdd) from=t3 to=t4
$.print current_of_inverter
.print tran current
.print tran current_2
.measure e1 param='1.2v*current*(t1-t2)' $ Measure Energy1
.measure e2 param='1.2v*current_2*(t3-t4)' $ Measures Energy2
.print e1
.print e2
.measure energy param='max(e1,e2)' $ Measures Energy ( worst case energy)
.print energy
.measure edp param='energy*delay' $ Measure Energy delay product
.print edp
.DC Vin 0V 1.2V 0.1V
.end 

