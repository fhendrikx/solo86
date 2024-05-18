library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Control286 is
  port (

    -- Misc
    i_clk           : in std_logic;
    i_reset_n       : in std_logic;
    -- i_jp1           : in std_logic;

    -- o_clkout        : buffer std_logic;
    o_led_latch     : out std_logic;
    o_ale           : buffer std_logic;
    o_warning       : out std_logic;

    -- Bit banged SD card
    --i_sd_miso       : in std_logic;

    --o_sd_sck        : out std_logic;
    --o_sd_cs         : out std_logic;
    --o_sd_mosi       : out std_logic;

    -- CPU
    i_addr_high     : in std_logic_vector(3 downto 0); -- A19..A16
    i_addr_low      : in std_logic_vector(7 downto 0); -- A7..A0
    io_data         : inout std_logic_vector(7 downto 0); -- D7..D0

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

    o_addr_high     : buffer std_logic_vector(3 downto 0); -- AC19..AC16

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

    -- i_pint_n        : in std_logic;
    i_pwait         : in std_logic

    );

  constant led_latch_addr   : std_logic_vector(7 downto 0) := x"08";
  constant bank_ctrl_addr   : std_logic_vector(7 downto 4) := "0001"; -- 0x10-1F
  constant piuart_addr      : std_logic_vector(7 downto 5) := "001";  -- 0x20->3F

end Control286;

architecture rtl of Control286 is

  -- physical pin mappings
  attribute chip_pin : string;

  -- Misc
  attribute chip_pin of i_clk           : signal is "83";
  attribute chip_pin of i_reset_n       : signal is "1";
  -- attribute chip_pin of i_jp1           : signal is "2";
  -- attribute chip_pin of o_clkout        : signal is "24";
  attribute chip_pin of o_led_latch     : signal is "4";
  attribute chip_pin of o_ale           : signal is "65";
  attribute chip_pin of o_warning       : signal is "75";

  -- Bit banged SD card
  --attribute chip_pin of i_sd_miso       : signal is "9";
  --attribute chip_pin of o_sd_sck        : signal is "5";
  --attribute chip_pin of o_sd_cs         : signal is "6";
  --attribute chip_pin of o_sd_mosi       : signal is "8";

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
  -- attribute chip_pin of i_pint_n        : signal is "81";
  attribute chip_pin of i_pwait         : signal is "84";

  -- signals
  signal iorq_n               : std_logic;
  signal event_start          : std_logic;
  signal piuart_wait_n        : std_logic;
  signal inta_cycle           : std_logic;

  signal irq0_latch           : std_logic;
  signal irq1_latch           : std_logic;
  signal irq2_latch           : std_logic;
  signal irq3_latch           : std_logic;
  -- signal pint_latch           : std_logic;

  signal irq0_clear           : std_logic;
  signal irq1_clear           : std_logic;
  signal irq2_clear           : std_logic;
  signal irq3_clear           : std_logic;
  -- signal pint_clear           : std_logic;

  signal wait_states          : integer range 0 to 3;

  type t_bank_table is array (0 to 7) of std_logic_vector(4 downto 0);
  signal bank_table           : t_bank_table;
  signal bank_write           : std_logic;

  type t_ctrl_state is (TS1, TS2, TC1, TC2);
  signal ctrl_state           : t_ctrl_state;

  type t_piuart_state is (WAIT_FOR_EVENT_START, WAIT_FOR_PI_READY, WAIT_FOR_PI_DONE, WAIT_FOR_EVENT_END);
  signal piuart_state         : t_piuart_state;

  signal ram_enable           : std_logic;
  signal rom_enable           : std_logic;

begin

  -- SD card outputs
  --o_sd_sck <= '0';
  --o_sd_cs <= '0';
  --o_sd_mosi <= '0';

  -- interrupt outputs
  o_nmi <= '0';

  o_intr <= '1' when irq0_latch = '1' or irq1_latch = '1' or
            irq2_latch = '1' or irq3_latch = '1'
            else '0';

  -- expansion slot clock
  -- o_clkout <= i_clk;

  -- CPU reset
  o_reset <= not i_reset_n;

  -- I/O request signal
  iorq_n <= '1' when o_iowr_n = '1' and o_iord_n = '1'
            else '0';

  -- PiUART read/write control
  o_prdwr <= '1' when o_iord_n = '0' else '0';

  -- edge triggered interrupt latches
  proc_irq0_latch: process(i_irq0_n, i_reset_n, irq0_clear) is
  begin

    if i_reset_n = '0' or irq0_clear = '1' then

      irq0_latch <= '0';

    elsif falling_edge(i_irq0_n) then

      irq0_latch <= '1';

    end if;

  end process;

  proc_irq1_latch: process(i_irq1_n, i_reset_n, irq1_clear) is
  begin

    if i_reset_n = '0' or irq1_clear = '1' then

      irq1_latch <= '0';

    elsif falling_edge(i_irq1_n) then

      irq1_latch <= '1';

    end if;

  end process;

  proc_irq2_latch: process(i_irq2_n, i_reset_n, irq2_clear) is
  begin

    if i_reset_n = '0' or irq2_clear = '1' then

      irq2_latch <= '0';

    elsif falling_edge(i_irq2_n) then

      irq2_latch <= '1';

    end if;

  end process;

  proc_irq3_latch: process(i_irq3_n, i_reset_n, irq3_clear) is
  begin

    if i_reset_n = '0' or irq3_clear = '1' then

      irq3_latch <= '0';

    elsif falling_edge(i_irq3_n) then

      irq3_latch <= '1';

    end if;

  end process;

  proc_addr_high: process(i_reset_n, o_ale) is
    variable bank           : std_logic_vector(4 downto 0);
    variable bank_index     : integer;
  begin

    if i_reset_n = '0' then

      o_addr_high <= "0000";
      rom_enable <= '0';
      ram_enable <= '0';

    elsif rising_edge(o_ale) then

      rom_enable <= '0';
      ram_enable <= '0';

      if i_addr_high(3) = '1' then
        -- reading from top half of memory, use bank table

        bank_index := to_integer(unsigned(i_addr_high(2 downto 0)));
        bank := bank_table(bank_index);

        o_addr_high <= bank(3 downto 0);

        if bank(4) = '0' then
          rom_enable <= '1';
        elsif bank(3) = '1' then
          ram_enable <= '1';
        end if;

      else
        -- reading from RAM, enable the RAM chips

        o_addr_high <= i_addr_high;
        ram_enable <= '1';

      end if;

    end if;

  end process;


  proc_bank_write: process(i_reset_n, bank_write) is
    variable bank_index     : integer;
  begin

    if i_reset_n = '0' then

      for i in 0 to 7 loop
        bank_table(i) <= "00000";
      end loop;

    elsif rising_edge(bank_write) then

      bank_index := to_integer(unsigned(i_addr_low(3 downto 1)));
      bank_table(bank_index) <= io_data(4 downto 0);

    end if;

  end process;

  -- proc_pint_latch: process(i_pint_n, i_reset_n, pint_clear) is
  -- begin

  --   if i_reset_n = '0' or pint_clear = '1' then

  --     pint_latch <= '0';

  --   elsif falling_edge(i_pint_n) then

  --     pint_latch <= '1';

  --   end if;

  -- end process;


  -- the 74HCT273 clk pulse min width ~25ns
  -- the 74HCT273 data min setup time ~25ns
  -- o_iowr_n will change during TS2 so will be visible TC1 meeting data setup time
  -- i_addr_low will change after TC1 meaning latch pulse will only last one clock
  -- meeting the pulse width requirement

  o_led_latch <= '1' when o_iowr_n = '0' and i_addr_low = led_latch_addr
                else '0';


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
    variable bank_index     : integer;
  begin

    if i_reset_n = '0' then

      -- initial signal state
      ctrl_state <= TS1;
      wait_states <= 0;

      event_start <= '0';
      bank_write <= '0';
      inta_cycle <= '0';

      irq0_clear <= '0';
      irq1_clear <= '0';
      irq2_clear <= '0';
      irq3_clear <= '0';
      -- pint_clear <= '0';

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

      o_memrd_n <= '1';
      o_iord_n <= '1';
      o_memwr_n <= '1';
      o_iowr_n <= '1';

      io_data <= "ZZZZZZZZ";

    elsif falling_edge(i_clk) then

      case ctrl_state is

        --
        when TS1 =>

          wait_states <= 0;

          event_start <= '0';
          bank_write <= '0';

          irq0_clear <= '0';
          irq1_clear <= '0';
          irq2_clear <= '0';
          irq3_clear <= '0';

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

          io_data <= "ZZZZZZZZ";

          if i_s0_n = '1' and i_s1_n = '1' then
            -- still waiting for something to happen

            ctrl_state <= TS1;
            o_ale <= '0';

          else
            -- the status bits represent something interesting, decode them
            -- to determine what type of bus cycle we're dealing with

            ctrl_state <= TS2;
            o_ale <= '1';

            o_warning <= '0';

            if i_m_io = '0' and i_s1_n = '0' and i_s0_n = '0' then
              -- Interrupt Ack

              -- toggle the cycle state
              inta_cycle <= not inta_cycle;

            elsif i_m_io = '0' and i_s1_n = '0' and i_s0_n = '1' then
              -- IO Read

            elsif i_m_io = '0' and i_s1_n = '1' and i_s0_n = '0' then
              -- IO Write

            elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '0' then
              -- Halt/Shutdown

              o_warning <= '1';

            elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '1' then
              -- Mem Read

              o_ram_ce_n <= '0';
              o_rom_ce_n <= '0';

            elsif i_m_io = '1' and i_s1_n = '1' and i_s0_n = '0' then
              -- Mem Write

              o_ram_ce_n <= '0';

            end if;

          end if;

        --
        when TS2 =>

          ctrl_state <= TC1;
          wait_states <= 0;

          event_start <= '0';
          bank_write <= '0';

          irq0_clear <= '0';
          irq1_clear <= '0';
          irq2_clear <= '0';
          irq3_clear <= '0';

          o_ale <= '0';
          o_ready_n <= '1';

          o_rom_oe_n <= '1';
          o_ram_we_low_n <= '1';
          o_ram_we_high_n <= '1';
          o_ram_oe_n <= '1';

          o_memrd_n <= '1';
          o_iord_n <= '1';
          o_memwr_n <= '1';
          o_iowr_n <= '1';

          io_data <= "ZZZZZZZZ";

          -- the latched address bus and data bus should now have stable values
          -- enable the expansion slot IORD/IOWR/MEMRD/MEMWR pins
          -- set the number of wait states to add

          if i_m_io = '0' and i_s1_n = '0' and i_s0_n = '0' then
            -- Interrupt Ack

            -- the datasheet says we should add one wait state
            wait_states <= 1;

            -- only output the interrupt vector on the second cycle
            if inta_cycle = '0' then

              if irq0_latch = '1' then

                io_data <= "00100000";
                irq0_clear <= '1';

              elsif irq1_latch = '1' then

                io_data <= "00100001";
                irq1_clear <= '1';

              elsif irq2_latch = '1' then

                io_data <= "00100010";
                irq2_clear <= '1';

              elsif irq3_latch = '1' then

                io_data <= "00100011";
                irq3_clear <= '1';

              -- elsif pint_latch = '1' then

              --   io_data <= "00100100";
              --   pint_clear <= '1';

              end if;

            end if;


          elsif i_m_io = '0' and i_s1_n = '0' and i_s0_n = '1' then
            -- IO Read

            o_iord_n <= '0';

            -- we need at least one wait state to allow peripherals time to
            -- assert wait if they need it
            wait_states <= 3;

            if i_addr_low(0) = '0' then
            -- only signal even numbered I/O requests

              -- PiUART
              if i_addr_low(7 downto 5) = piuart_addr then
                event_start <= '1';
              end if;

              -- memory banking control
              if i_addr_low(7 downto 4) = bank_ctrl_addr then
                bank_index := to_integer(unsigned(i_addr_low(3 downto 1)));
                -- don't use "000", adds more macro cells and results in bad mapping (wtf?)
                io_data(7 downto 5) <= "111";
                io_data(4 downto 0) <= bank_table(bank_index);
              end if;

            end if;

          elsif i_m_io = '0' and i_s1_n = '1' and i_s0_n = '0' then
            -- IO Write

            o_iowr_n <= '0';

            -- we need at least one wait state to allow peripherals time to
            -- assert wait if they need it
            wait_states <= 3;

            if i_addr_low(0) = '0' then
            -- only signal even numbered I/O requests

              -- PiUART
              if i_addr_low(7 downto 5) = piuart_addr then
                event_start <= '1';
              end if;

              -- memory banking control
              if i_addr_low(7 downto 4) = bank_ctrl_addr then
                bank_write <= '1';
              end if;

            end if;

          elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '0' then
            -- Halt/Shutdown

            wait_states <= 0;

          elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '1' then
            -- Mem Read

            wait_states <= 0;

            if rom_enable = '1' then
              o_rom_oe_n <= '0';
            end if;

            if ram_enable = '1' then
              o_ram_oe_n <= '0';
            end if;

            if rom_enable = '0' and ram_enable = '0' then
              o_memrd_n <= '0';
              wait_states <= 3;
            end if;

          elsif i_m_io = '1' and i_s1_n = '1' and i_s0_n = '0' then
            -- Mem Write

            wait_states <= 0;

            if rom_enable = '1' then
              -- do nothing, we don't write to ROM
            end if;

            if ram_enable = '1' then

              -- write enable (WE) the memory chips (e.g. tell them to read a
              -- value from the data bus)
              -- use BHE/A0 to work out which banks (high/low) need to be enabled

              if i_bhe_n = '0' then
                -- write to the high bank
                o_ram_we_high_n <= '0';
              end if;

              if i_addr_low(0) = '0' then
                -- write to the low bank
                o_ram_we_low_n <= '0';
              end if;

            end if;

            if rom_enable = '0' and ram_enable = '0' then
              -- write to external peripheral
              o_memwr_n <= '0';
              wait_states <= 3;
            end if;

          end if;

        --
        when TC1 =>

          -- work out if we're going to signal ready or not ready to the CPU
          -- during this phase peripherals are deciding what to do

          ctrl_state <= TC2;

          event_start <= '0';
          bank_write <= '0';

          irq0_clear <= '0';
          irq1_clear <= '0';
          irq2_clear <= '0';
          irq3_clear <= '0';
          -- pint_clear <= '0';

          o_ale <= '0';
          o_ready_n <= '1';

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

          event_start <= '0';
          bank_write <= '0';

          irq0_clear <= '0';
          irq1_clear <= '0';
          irq2_clear <= '0';
          irq3_clear <= '0';

          o_ale <= '0';

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

            io_data <= "ZZZZZZZZ";

          end if;

      end case;

    end if;

  end process;

end rtl;
