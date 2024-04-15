library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
 
entity Control286 is
  port (

    -- Misc
    i_clk           : in std_logic;
    i_reset_n       : in std_logic;
    i_jp1           : in std_logic;

    o_clkout        : buffer std_logic;
    o_led_latch     : out std_logic;
    o_ale           : out std_logic;
    o_warning       : out std_logic;

    -- Bit banged SD card
    i_sd_miso       : in std_logic;
    
    o_sd_sck        : out std_logic;
    o_sd_cs         : out std_logic;
    o_sd_mosi       : out std_logic;
    
    -- CPU
    i_addr_high     : in unsigned(3 downto 0); -- A19..A16
    i_addr_low      : in unsigned(7 downto 0); -- A7..A0
    io_data         : inout unsigned(7 downto 0); -- D7..D0

    i_s0_n          : in std_logic;
    i_s1_n          : in std_logic;
    i_bhe_n         : in std_logic;
    i_m_io          : in std_logic;

    o_reset         : out std_logic;
    o_ready_n       : buffer std_logic;
    o_nmi           : out std_logic;
    o_intr          : out std_logic;
    
    -- Memory
    o_rom_ce_n      : out std_logic;
    o_rom_oe_n      : out std_logic;

    o_ram_we_low_n  : out std_logic;
    o_ram_we_high_n : out std_logic;
    o_ram_oe_n      : out std_logic;
    o_ram_ce_n      : out std_logic;

    o_addr_high     : out unsigned(3 downto 0); -- AC19..AC16
    
    -- Expansion slot
    o_memrd_n       : out std_logic;
    o_memwr_n       : out std_logic;
    o_iord_n        : buffer std_logic;
    o_iowr_n        : buffer std_logic;

    i_wait0_n       : in std_logic;
    i_wait1_n       : in std_logic;

    i_irq0_n        : in std_logic;
    i_irq1_n        : in std_logic;
    i_irq2_n        : in std_logic;
    i_irq3_n        : in std_logic;

    -- Pi
    o_pdata_en_n    : out std_logic;
    o_prdwr         : out std_logic;
    o_pevent        : out std_logic;

    i_pint_n        : in std_logic;
    i_pwait         : in std_logic

    );

  constant clock_hz         : integer := 40000000;
  constant ticks_wrap       : integer := (clock_hz / 200) - 1; -- 100 Hz ticks

  constant led_latch_addr   : unsigned(7 downto 0) := x"08";
  constant piuart_addr      : unsigned(7 downto 0) := x"20"; -- 0x20->3F
  constant piuart_mask      : unsigned(7 downto 0) := "11100000";
  
end Control286;
 
architecture rtl of Control286 is

  -- physical pin mappings
  attribute chip_pin : string;

  -- Misc
  attribute chip_pin of i_clk           : signal is "83";
  attribute chip_pin of i_reset_n       : signal is "1";
  attribute chip_pin of i_jp1           : signal is "2";
  attribute chip_pin of o_clkout        : signal is "24";
  attribute chip_pin of o_led_latch     : signal is "4";
  attribute chip_pin of o_ale           : signal is "65";
  attribute chip_pin of o_warning       : signal is "75";

  -- Bit banged SD card
  attribute chip_pin of i_sd_miso       : signal is "9";
  attribute chip_pin of o_sd_sck        : signal is "5";
  attribute chip_pin of o_sd_cs         : signal is "6";
  attribute chip_pin of o_sd_mosi       : signal is "8";
  
  -- CPU
  attribute chip_pin of i_addr_high     : signal is "64, 63, 60, 61"; -- A19..A16
  attribute chip_pin of i_addr_low      : signal is "44, 45, 46, 48, 49, 50, 51, 52"; -- A7..A0
  attribute chip_pin of io_data         : signal is "33, 34, 35, 36, 37, 39, 40, 41"; -- D7..D0
  attribute chip_pin of i_s0_n          : signal is "68";
  attribute chip_pin of i_s1_n          : signal is "69";
  attribute chip_pin of i_bhe_n         : signal is "67";
  attribute chip_pin of i_m_io          : signal is "70";
  attribute chip_pin of o_reset         : signal is "55";
  attribute chip_pin of o_ready_n       : signal is "73";
  attribute chip_pin of o_nmi           : signal is "74";
  attribute chip_pin of o_intr          : signal is "76";

  -- Memory
  attribute chip_pin of o_rom_ce_n      : signal is "10";
  attribute chip_pin of o_rom_oe_n      : signal is "11";
  attribute chip_pin of o_ram_we_low_n  : signal is "12";
  attribute chip_pin of o_ram_we_high_n : signal is "15";
  attribute chip_pin of o_ram_oe_n      : signal is "16";
  attribute chip_pin of o_ram_ce_n      : signal is "17";
  attribute chip_pin of o_addr_high     : signal is "18, 22, 20, 21"; -- AC19..AC16
  
  -- Expansion slot
  attribute chip_pin of o_memrd_n       : signal is "54";
  attribute chip_pin of o_memwr_n       : signal is "56";
  attribute chip_pin of o_iord_n        : signal is "57";
  attribute chip_pin of o_iowr_n        : signal is "58";
  attribute chip_pin of i_wait0_n       : signal is "27";
  attribute chip_pin of i_wait1_n       : signal is "25";
  attribute chip_pin of i_irq0_n        : signal is "31";
  attribute chip_pin of i_irq1_n        : signal is "29";
  attribute chip_pin of i_irq2_n        : signal is "30";
  attribute chip_pin of i_irq3_n        : signal is "28";

  -- Pi
  attribute chip_pin of o_pdata_en_n    : signal is "77";
  attribute chip_pin of o_prdwr         : signal is "79";
  attribute chip_pin of o_pevent        : signal is "80";
  attribute chip_pin of i_pint_n        : signal is "81";
  attribute chip_pin of i_pwait         : signal is "84";
  
  -- signals
  signal iorq_n               : std_logic;
  signal event_start          : std_logic;
  signal piuart_wait_n        : std_logic;
  
  --signal ticks_count          : integer range 0 to ticks_wrap;

  type t_ctrl_state is (TS1, TS2, TC1, TC2, ERROR_S);
  signal ctrl_state           : t_ctrl_state;

  signal wait_states          : integer range 0 to 3;

  type t_piuart_state is (WAIT_FOR_EVENT_START, WAIT_FOR_PI_READY, WAIT_FOR_PI_DONE, WAIT_FOR_EVENT_END);
  signal piuart_state         : t_piuart_state;
  

begin

  -- SD card outputs
  o_sd_sck <= '0';
  o_sd_cs <= '0';
  o_sd_mosi <= '0';

  -- interrupt outputs
  o_nmi <= '0';
  o_intr <= '0';

  -- data bus
  io_data <= "ZZZZZZZZ";

  -- expansion slot clock
  o_clkout <= i_clk;

  -- CPU reset
  o_reset <= not i_reset_n;

  -- I/O request signal
  iorq_n <= '1' when o_iowr_n = '1' and o_iord_n = '1'
            else '0';
  
  -- PiUART event start
  event_start <= '1' when iorq_n = '0' and
                 ((i_addr_low and piuart_mask) = piuart_addr)
                 else '0';

  -- PiUART read/write control

  o_prdwr <= '1' when o_iord_n = '0' else '0';


  
  -- the 74HCT273 clk pulse min width ~25ns
  -- the 74HCT273 data min setup time ~25ns
  -- o_iowr_n will change during TS2 so will be visible TC1 meeting data setup time
  -- i_addr_low will change after TC1 meaning latch pulse will only last one clock
  -- meeting the pulse width requirement
  proc_led_latch: process(i_clk, i_reset_n) is
  begin
    if i_reset_n = '0' then

      o_led_latch <= '0';

    elsif falling_edge(i_clk) then

      if o_iowr_n = '0' and i_addr_low = led_latch_addr then
        o_led_latch <= '1';
      else
        o_led_latch <= '0';
      end if;
        
    end if;

  end process;

  -- simpler alternative, not sure about the timing
  --o_led_latch <= '1' when o_iowr_n = '0' and i_addr_low = led_latch_addr
  --               else '0';

  
  --
  -- manage the PiUART
  --

  proc_piuart_state_machine: process(i_clk, i_reset_n) is
  begin

    if i_reset_n = '0' then
      
      piuart_state <= WAIT_FOR_EVENT_START;
      
    elsif rising_edge(i_clk) then

      case piuart_state is

        when WAIT_FOR_EVENT_START =>

          if event_start = '1' then
            if i_pwait = '0' then
              -- pi is ready
              piuart_state <= WAIT_FOR_PI_DONE;
            else
              -- pi is not ready
              piuart_state <= WAIT_FOR_PI_READY;
            end if;
          end if;
            
        when WAIT_FOR_PI_READY =>

          if i_pwait = '0' then
            -- pi is ready
            piuart_state <= WAIT_FOR_PI_DONE;
          end if;
            
        when WAIT_FOR_PI_DONE =>

          if i_pwait = '1' then
            -- pi is done
            piuart_state <= WAIT_FOR_EVENT_END;
          end if;

        when WAIT_FOR_EVENT_END =>

          if iorq_n = '1' then
            piuart_state <= WAIT_FOR_EVENT_START;
          end if;

      end case;
          
    end if;

  end process;

  proc_piuart_state_machine_outputs: process(piuart_state, i_pwait) is
  begin

    case piuart_state is

      when WAIT_FOR_EVENT_START =>

        o_pevent <= '0';
        o_pdata_en_n <= i_pwait;
        piuart_wait_n <= '1';

        
      when WAIT_FOR_PI_READY =>

        o_pevent <= '0';
        o_pdata_en_n <= i_pwait;
        piuart_wait_n <= '0';
        
      when WAIT_FOR_PI_DONE =>

        o_pevent <= '1';
        o_pdata_en_n <= '0';
        piuart_wait_n <= '0';
        
      when WAIT_FOR_EVENT_END =>  

        o_pevent <= '1';
        o_pdata_en_n <= '0';
        piuart_wait_n <= '1';
        
    end case;
    
  end process;

  
  -- count clock pulses until we reach ticks_wrap
  -- for now outputting this via pin o_clkout
  -- change this to an internal register to trigger an interrupt
  -- when interrupt handling is added
  -- TODO, add enable ticks option
  -- proc_ticks: process(i_clk, i_reset_n) is
  -- begin

  --   if i_reset_n = '0' then
      
  --     ticks_count <= 0;
  --     o_clkout <= '0';
      
  --   elsif rising_edge(i_clk) then
      
  --     if ticks_count = ticks_wrap then
  --       ticks_count <= 0;
  --       o_clkout <= not o_clkout;
  --     else
  --       ticks_count <= ticks_count + 1;
  --     end if;

  --   end if;

  -- end process;

  
  --
  -- The main state machine for controlling the 286 bus
  --
  
  -- During status cycle the following states apply
  -- COD/INTA disambiguates a couple cases we don't care about

  -- M/IO  S1    S0

  -- 0     0     0   => Interrupt Ack
  -- 0     0     1   => IO Read
  -- 0     1     0   => IO Write
  -- 0     1     1   => N/A, not a status cycle
  
  -- 1     0     0   => Halt/Shutdown
  -- 1     0     1   => Mem Data Read (COD/INTA = 0), Mem Instruction Read (COD/INTA = 1)
  -- 1     1     0   => Mem Data Write
  -- 1     1     1   => N/A, not a status cycle

  -- ctrl_state represents the end of the given cycle, e.g. TS1 is the falling edge
  -- at the end of the TS1 cycle
  
  proc_ctrl_state_machine: process(i_clk, i_reset_n) is
  begin

    if i_reset_n = '0' then
      
      ctrl_state <= TS1;
      
      -- initial output pin state
      o_warning <= '0';
      
      o_ale <= '0';
      o_ready_n <= '1';

      o_rom_oe_n <= '1';
      o_rom_ce_n <= '1';
      o_ram_we_low_n <= '1';
      o_ram_we_high_n <= '1';
      o_ram_oe_n <= '1';
      o_ram_ce_n <= '1';

      o_addr_high <= "1111";
      
      o_memrd_n <= '1';
      o_iord_n <= '1';
      o_memwr_n <= '1';
      o_iowr_n <= '1';
      
      wait_states <= 0;
      
    elsif falling_edge(i_clk) then

      case ctrl_state is

        --
        when TS1 =>

          if i_s0_n = '1' and i_s1_n = '1' then
            -- still waiting for something to happen
            
            ctrl_state <= TS1;

          else
            -- the status bits represent something interesting, decode them
            -- to determine what type of bus cycle we're dealing with

            if i_m_io = '0' and i_s1_n = '0' and i_s0_n = '0' then
              -- Interrupt Ack

              -- TODO, error for now
              ctrl_state <= ERROR_S;
              
            elsif i_m_io = '0' and i_s1_n = '0' and i_s0_n = '1' then
              -- IO Read

              ctrl_state <= TS2;
              
              o_ale <= '1';
              o_addr_high <= i_addr_high;
              
            elsif i_m_io = '0' and i_s1_n = '1' and i_s0_n = '0' then
              -- IO Write

              ctrl_state <= TS2;

              o_ale <= '1';
              o_addr_high <= i_addr_high;
              
            elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '0' then
              -- Halt/Shutdown

              -- TODO, error for now
              ctrl_state <= ERROR_S;

            elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '1' then
              -- Mem Read

              ctrl_state <= TS2;

              o_ale <= '1';
              o_addr_high <= i_addr_high;

              if (i_addr_high and "1000") = "1000" then
                -- reading from ROM, enable the ROM chips
                o_rom_ce_n <= '0';

              else
                -- reading from RAM, enable the RAM chips
                o_ram_ce_n <= '0';

              end if;
              
            elsif i_m_io = '1' and i_s1_n = '1' and i_s0_n = '0' then
              -- Mem Write
              
              ctrl_state <= TS2;
              
              o_ale <= '1';
              o_addr_high <= i_addr_high;

              -- all writes go to RAM, enable the RAM chips
              o_ram_ce_n <= '0';

            end if;

          end if;

        --
        when TS2 =>
          
          ctrl_state <= TC1;

          o_ale <= '0';

          -- the latched address bus and data bus should now have stable values
          -- enable the expansion slot IORD/IOWR/MEMRD/MEMWR pins
          -- set the number of wait states to add
          
          if i_m_io = '0' and i_s1_n = '0' and i_s0_n = '1' then
            -- IO Read

            -- only signal even numbered I/O requests
            if (i_addr_low and "00000001") = "00000000" then
              o_iord_n <= '0';
              -- we need at least one wait state to allow peripherals time to
              -- assert wait if they need it
              wait_states <= 1;
            end if;
            
          elsif i_m_io = '0' and i_s1_n = '1' and i_s0_n = '0' then
            -- IO Write

            -- only signal even numbered I/O requests
            if (i_addr_low and "00000001") = "00000000" then
              o_iowr_n <= '0';
              -- we need at least one wait state to allow peripherals time to
              -- assert wait if they need it
              wait_states <= 1;
            end if;
            
          elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '1' then
            -- Mem Read

            o_memrd_n <= '0';
            wait_states <= 0;

            -- output enable (OE) the memory chips (e.g. tell them to put a
            -- value on the data bus)
            if (i_addr_high and "1000") = "1000" then
              -- reading from ROM

              o_rom_oe_n <= '0';

            else
              -- reading from RAM

              o_ram_oe_n <= '0';

            end if;
            
          elsif i_m_io = '1' and i_s1_n = '1' and i_s0_n = '0' then
            -- Mem Write

            o_memwr_n <= '0';
            wait_states <= 0;

            -- write enable (WE) the memory chips (e.g. tell them to read a
            -- value from the data bus)
            -- use BHE/A0 to work out which banks (high/low) need to be enabled
            
            if i_bhe_n = '0' then
              -- write to the high bank
              o_ram_we_high_n <= '0';
            end if;

            if (i_addr_low and "00000001") = "00000000" then
              -- write to the low bank
              o_ram_we_low_n <= '0';
            end if;
            
          else

            ctrl_state <= ERROR_S;
            
          end if;
          
        --
        when TC1 =>

          -- work out if we're going to signal ready or not ready to the CPU
          -- during this phase peripherals are deciding what to do
          
          ctrl_state <= TC2;

          if i_wait0_n = '1' and i_wait1_n = '1' and piuart_wait_n = '1' and wait_states = 0 then
            -- no reason to wait, set the ready signal

            o_ready_n <= '0';            

          else
            -- wait this TC cycle
            
            if wait_states > 0 then
              -- decrement wait_states until it's zero

              wait_states <= wait_states - 1;
              
            end if;
            
          end if;

        --
        when TC2 =>

          -- last phase in the TC cycle. Either we're done or we'll repeat
          -- because of wait states
          
          if o_ready_n = '1' then
            -- the CPU has been instructed to add another TC cycle

            ctrl_state <= TC1;

          else
            -- we're all done, clean up
            
            ctrl_state <= TS1;

            o_ready_n <= '1';

            o_rom_oe_n <= '1';
            o_rom_ce_n <= '1';
            o_ram_we_low_n <= '1';
            o_ram_we_high_n <= '1';
            o_ram_oe_n <= '1';
            o_ram_ce_n <= '1';

            o_memrd_n <= '1';
            o_iord_n <= '1';
            o_memwr_n <= '1';
            o_iowr_n <= '1';
              
          end if;

        --
        when ERROR_S =>

          -- remain in error state until reset
          ctrl_state <= ERROR_S;

          o_warning <= '1';

      end case;

    end if;

  end process;
  
end rtl;
