-- fsm.vhd: Finite State Machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (FIRST, ZN1, ZN2, ZN3, ZN4_1, ZN4_2, ZN5_1, ZN5_2, ZN6_1, ZN6_2, ZN7_1, ZN7_2, ZN8_1, ZN8_2, ZN9_1, ZN9_2, ZN10_2, LAST, WRONG, PRINT_OK, PRINT_WRONG, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= FIRST;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FIRST =>
      next_state <= FIRST;
		if (KEY(1) = '1') then
			next_state <= ZN1;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN1 =>
      next_state <= ZN1;
		if (KEY(2) = '1') then
			next_state <= ZN2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN2 =>
      next_state <= ZN2;
		if (KEY(0) = '1') then
			next_state <= ZN3;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG;
		end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN3 =>
      next_state <= ZN3;
		if (KEY(1) = '1') then
			next_state <= ZN4_1;
		elsif (KEY(2) = '1') then
			next_state <= ZN4_2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN4_1 =>
      next_state <= ZN4_1;
		if (KEY(7) = '1') then
			next_state <= ZN5_1;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN5_1 =>
      next_state <= ZN5_1;
		if (KEY(3) = '1') then
			next_state <= ZN6_1;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN6_1 =>
      next_state <= ZN6_1;
		if (KEY(6) = '1') then
			next_state <= ZN7_1;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN7_1 =>
      next_state <= ZN7_1;
		if (KEY(4) = '1') then
			next_state <= ZN8_1;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN8_1 =>
      next_state <= ZN8_1;
		if (KEY(1) = '1') then
			next_state <= ZN9_1;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN9_1 =>
      next_state <= ZN9_1;
		if (KEY(6) = '1') then
			next_state <= LAST;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN4_2 =>
      next_state <= ZN4_2;
		if (KEY(0) = '1') then
			next_state <= ZN5_2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN5_2 =>
      next_state <= ZN5_2;
		if (KEY(8) = '1') then
			next_state <= ZN6_2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ZN6_2 =>
      next_state <= ZN6_2;
		if (KEY(3) = '1') then
			next_state <= ZN7_2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN7_2 =>
      next_state <= ZN7_2;
		if (KEY(6) = '1') then
			next_state <= ZN8_2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN8_2 =>
      next_state <= ZN8_2;
		if (KEY(9) = '1') then
			next_state <= ZN9_2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN9_2 =>
      next_state <= ZN9_2;
		if (KEY(9) = '1') then
			next_state <= ZN10_2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ZN10_2 =>
      next_state <= ZN10_2;
		if (KEY(2) = '1') then
			next_state <= LAST;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_WRONG; 
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when WRONG =>
		next_state <= WRONG;
		if (KEY(15) = '1') then
			next_state <= PRINT_WRONG;
		end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when LAST =>
		next_state <= LAST;
		if (KEY(15) = '1') then
			next_state <= PRINT_OK;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= WRONG; 
		end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when PRINT_OK =>
      next_state <= PRINT_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when PRINT_WRONG =>
      next_state <= PRINT_WRONG;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= FIRST; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= FIRST;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FIRST|ZN1|ZN2|ZN3|ZN4_1|ZN4_2|ZN5_1|ZN5_2|ZN6_1|ZN6_2|ZN7_1|ZN7_2|ZN8_1|ZN8_2|ZN9_1|ZN9_2|ZN10_2|WRONG|LAST =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK =>
      FSM_CNT_CE     <= '1';
		FSM_MX_MEM		<= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
	when PRINT_WRONG =>
      FSM_CNT_CE     <= '1';
		FSM_MX_MEM		<= '0';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
   end case;
end process output_logic;

end architecture behavioral;

