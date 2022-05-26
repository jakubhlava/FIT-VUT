-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2020 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WE    : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti 
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 type cpustate is (Sbegin, Sfetch, Sdecode, Sincptr, Sdecptr, Sincmem0,Sincmem1,Sincmem2, Sdecmem0,Sdecmem1,Sdecmem2, Sbegwhile, Sbegwhile1, Sbegwhile2, Sendwhile,Sendwhile1, Sinwhile, Sendskip, Sread0, Swrite0,Sread1, Swrite1, Sstop, Sinfetch);
 signal state : cpustate := Sbegin;
 signal nextState : cpustate;
 
 --type retstack is array (0 to 15) of std_logic_vector(11 downto 0);
 
 signal pc : std_logic_vector(11 downto 0) := (others => '0'); -- program counter
 signal pc_inc : std_logic := '0';
 signal pc_dec : std_logic := '0';
 signal pc_ld : std_logic := '0';
 signal pc_save : std_logic := '0';
 
 --signal pc_next : std_logic_vector(11 downto 0) := (others => '0');
 
 signal ptr : std_logic_vector(9 downto 0) := (others => '0'); -- pointer do pameti
 signal ptr_inc : std_logic := '0';
 signal ptr_dec : std_logic := '0';

 signal mux_sel : std_logic_vector(1 downto 0) := (others => '0'); -- rizeni muxu
 signal mux_out : std_logic_vector(7 downto 0) := (others => '0'); -- vystup muxu do RDATA
 
-- signal rasctr : integer := 0;
-- signal ras : retstack := (others => (others => '0'));
 signal ras_drop : std_logic := '0';
-- signal new_pc : std_logic_vector(11 downto 0) := (others => '0');

 signal ras : std_logic_vector(11 downto 0) := (others => '0');

begin

 

 pc_proc: process (RESET, CLK, pc_ld, pc_inc, pc_dec, ras, pc)
 begin
	if (RESET = '1') then
		pc <= (others=>'0');
	elsif (rising_edge(CLK)) then
		if (pc_ld = '1') then
			pc <= ras;
		elsif (pc_inc = '1') then
			pc <= pc + '1';
		elsif (pc_dec = '1') then
			pc <= pc - '1';
		end if;
	end if;
	CODE_ADDR <= pc;
 end process;
 
 ras_mgmt: process (RESET, pc_save, ras_drop, pc)
 begin
	if (RESET = '1') then
		ras <= (others => '0');
	elsif (pc_save = '1') then
		ras <= pc + '1';
	elsif (ras_drop = '1') then
		ras <= (others => '0');
	end if;
 end process;
 
 ptr_proc: process (RESET, CLK, ptr_inc, ptr_dec, ptr)
 begin
	if (RESET = '1') then
		ptr <= (others => '0');
	elsif (rising_edge(CLK)) then
		if (ptr_inc = '1') then
			ptr <= ptr + '1';
		elsif (ptr_dec = '1') then
			ptr <= ptr - '1';
		end if;
	end if;
	DATA_ADDR <= ptr;
 end process;
 
 mux: process (mux_sel, IN_DATA, DATA_RDATA)
 begin
	case mux_sel is
		when "01" => mux_out <= DATA_RDATA;
		when "10" => mux_out <= DATA_RDATA + 1;
		when "11" => mux_out <= DATA_RDATA - 1;
		when others => mux_out <= IN_DATA;
	end case;
 end process;
 
 fsm_states: process (RESET, CLK, EN)
 begin
	if (RESET = '1') then
		state <= Sbegin;
	elsif (rising_edge(CLK)) then
		if (EN = '1') then
			state <= nextState;
		end if;
	end if;
 end process;
 
 control: process (state, CODE_DATA, DATA_RDATA, OUT_BUSY, IN_VLD, mux_out)
 begin
	-- type cpustate is (Sbegin, Sfetch, Sdecode, Sexe, Sincptr, Sdecptr, Sincmem, Sdecmem, Sread, Swrite, Sstop);
	CODE_EN <= '0';
	DATA_EN <= '0';
	DATA_WE <= '0';
	IN_REQ <= '0';
	OUT_WE <= '0';
	pc_ld <= '0';
	pc_inc <= '0';
	pc_dec <= '0';
	pc_save <= '0';
	ras_drop <= '0';
	ptr_inc <= '0';
	ptr_dec <= '0';
	mux_sel <= "00";
	DATA_WDATA <= mux_out;
	OUT_DATA <= (others => '0');
	case state is
		when Sbegin =>
			nextState <= Sfetch;
		when Sfetch =>
			CODE_EN <= '1';
			nextState <= Sdecode;
		when Sdecode =>
			case CODE_DATA is
				when x"3E" => nextState <= Sincptr;
				when x"3C" => nextState <= Sdecptr;
				when x"2B" => nextState <= Sincmem0;
				when x"2D" => nextState <= Sdecmem0;	
				when x"5B" => nextState <= Sbegwhile;
				when x"5D" => nextState <= Sendwhile;
				when x"2E" => nextState <= Swrite0;
				when x"2C" => nextState <= Sread0;
				when x"00" => nextState <= Sstop;
				when others =>
					pc_inc <= '1';
					nextState <= Sfetch;
			end case;
		when Sbegwhile =>
			DATA_EN <= '1';
			DATA_WE <= '0';
			nextState <= Sbegwhile1;
		when Sbegwhile1 =>
			if (DATA_RDATA > 0) then
				pc_save <= '1';
				nextState <= Sbegwhile2;
			else
				nextState <= Sinwhile;
			end if;
		when Sbegwhile2 =>
			pc_inc <= '1';
			nextState <= Sfetch;
		when Sinwhile =>
			if (CODE_DATA = x"5D") then
				pc_inc <= '1';
				nextState <= Sendskip;
			else
				pc_inc <= '1';
				nextState <= Sinfetch;
			end if;
		when Sinfetch =>
			CODE_EN <= '1';
			nextState <= Sinwhile;
		when Sendskip =>
			pc_inc <= '1';
			nextState <= Sfetch;
		when Sendwhile =>
			DATA_EN <= '1';
			DATA_WE <= '0';
			nextState <= Sendwhile1;
		when Sendwhile1 =>
			if (DATA_RDATA > 0) then
				pc_ld <= '1';
			else 
				pc_inc <= '1';
				ras_drop <= '1';
			end if;
			nextState <= Sfetch;
		when Sincptr =>
			pc_inc <= '1';
			ptr_inc <= '1';
			nextState <= Sfetch;
		when Sdecptr =>
			pc_inc <= '1';
			ptr_dec <= '1';
			nextState <= Sfetch;
		when Sincmem0 =>
			pc_inc <= '1';
			DATA_EN <= '1';
			DATA_WE <= '0';
			nextState <= Sincmem1;
		when Sincmem1 =>
			mux_sel <= "10";
			nextState <= Sincmem2;
		when Sincmem2 =>
			mux_sel <= "10";
			DATA_EN <= '1';
			DATA_WE <= '1';
			--DATA_WDATA <= mux_out;
			nextState <= Sfetch;
		when Sdecmem0 =>
			pc_inc <= '1';
			DATA_EN <= '1';
			DATA_WE <= '0';
			nextState <= Sdecmem1;
		when Sdecmem1 =>
			mux_sel <= "11";
			nextState <= Sdecmem2;
		when Sdecmem2 =>
			mux_sel <= "11";
			DATA_EN <= '1';
			DATA_WE <= '1';
			--DATA_WDATA <= mux_out;
			nextState <= Sfetch;
		when Swrite0 => 
			pc_inc <= '1';
			mux_sel <= "01";
			DATA_WE <= '0';
			DATA_EN <= '1';
			nextState <= Swrite1;
		when Swrite1 =>
			if (OUT_BUSY = '1') then
				mux_sel <= "01";
				DATA_WE <= '0';
				DATA_EN <= '1';
				nextState <= Swrite1;
			else 
				OUT_WE <= '1';
				OUT_DATA <= DATA_RDATA;
				nextState <= Sfetch;
			end if;
		when Sread0 =>
			pc_inc <= '1';
			IN_REQ <= '1';
			mux_sel <= "00";
			nextState <= Sread1;
		when Sread1 =>
			if (IN_VLD = '1') then
				DATA_WE <= '1';
				DATA_EN <= '1';
				nextState <= Sfetch;
			else
				IN_REQ <= '1';
				mux_sel <= "00";
				nextState <= Sread1;
			end if;
		when Sstop =>
			nextState <= sStop;
		when others =>
			nextState <= Sstop;
	end case;
 end process;


 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --   - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --   - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly.
 
end behavioral;
 
