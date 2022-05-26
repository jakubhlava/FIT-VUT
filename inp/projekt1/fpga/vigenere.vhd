library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- rozhrani Vigenerovy sifry
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;

-- V souboru fpga/sim/tb.vhd naleznete testbench, do ktereho si doplnte
-- znaky vaseho loginu (velkymi pismeny) a znaky klice dle vaseho prijmeni.

architecture behavioral of vigenere is

    -- Sem doplnte definice vnitrnich signalu, prip. typu, pro vase reseni,
    -- jejich nazvy doplnte tez pod nadpis Vigenere Inner Signals v souboru
    -- fpga/sim/isim.tcl. Nezasahujte do souboru, ktere nejsou explicitne
    -- v zadani urceny k modifikaci.
	
	 type stateType is (default, plus, minus);
	 
	 signal spocitanyPosuv : std_logic_vector(7 downto 0);
	 signal pismenoVpred : std_logic_vector(7 downto 0);
	 signal pismenoVzad : std_logic_vector(7 downto 0);
	 signal mealyState : stateType;

	 signal mealyOut : std_logic_vector(1 downto 0);
 
	 signal mealyNextState : stateType;

begin

    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.
	 
	vypocet: process (DATA, KEY) is
		begin
		spocitanyPosuv <= KEY - 64;
	end process;
	
	kladny: process (spocitanyPosuv, DATA) is
		variable vyp : std_logic_vector(7 downto 0);
	begin
		vyp := DATA;
		if (vyp > 47 and vyp < 58) then
			vyp := "00100011"; -- 35, #
		else
			vyp := vyp + spocitanyPosuv;
		end if;
		if (vyp > 90) then
			vyp := vyp - 26;
		end if;
		pismenoVpred <= vyp;
	end process;
	
	zaporny: process (spocitanyPosuv, DATA) is
		variable vyp : std_logic_vector(7 downto 0);
	begin
		vyp := DATA;
		if (vyp > 47 and vyp < 58) then
			vyp := "00100011"; -- 35, #
		else
			vyp := vyp - spocitanyPosuv;
			if (vyp < 65) then
				vyp := vyp + 26;
			end if;
		end if;
		pismenoVzad <= vyp;
	end process;
	
	mealyStates: process (RST, CLK) is
	begin
		if (CLK'event and CLK = '1') then
			mealyState <= mealyNextState;
		end if;
	end process;
	
	fsm_mealy: process (mealyState, DATA, RST) is
	begin
		
		mealyNextState <= default;
		mealyOut <= "00";
		
		case mealyState is
			when plus => 
				mealyOut <= "01";
				mealyNextState <= minus;
			when minus => 
				mealyOut <= "10";
				mealyNextState <= plus;
			when default => 
				mealyOut <= "00";
				mealyNextState <= plus;
		end case;

	end process;
	
	codemux: process (pismenoVpred, pismenoVzad, mealyOut, RST) is
	begin
		case mealyOut is
			when "01" => CODE <= pismenoVpred;
			when "10" => CODE <= pismenoVzad;
			when others => CODE <= "00100011"; 
		end case;
		if (RST = '1') then
			CODE <= "00100011";
		end if;
	end process;

end behavioral;
