proc isim_script {} {

   add_divider "Signals of the Vigenere Interface"
   add_wave_label "" "CLK" /testbench/clk
   add_wave_label "" "RST" /testbench/rst
   add_wave_label "-radix ascii" "DATA" /testbench/tb_data
   add_wave_label "-radix ascii" "KEY" /testbench/tb_key
   add_wave_label "-radix ascii" "CODE" /testbench/tb_code

   add_divider "Vigenere Inner Signals"
   # sem doplnte vase vnitrni signaly. chcete-li v diagramu zobrazit desitkove
   # cislo, vlozte do prvnich uvozovek: -radix dec
   add_wave_label "-radix dec" "spocitanyPosuv" /testbench/uut/spocitanyPosuv
   add_wave_label "-radix ascii" "pismenoVpred" /testbench/uut/pismenoVpred
   add_wave_label "-radix ascii" "pismenoVzad" /testbench/uut/pismenoVzad
   add_wave_label "" "mealyState" /testbench/uut/mealyState
   add_wave_label "" "mealyNextState" /testbench/uut/mealyNextState
   add_wave_label "" "mealyOut" /testbench/uut/mealyOut


   run 8 ns
}
