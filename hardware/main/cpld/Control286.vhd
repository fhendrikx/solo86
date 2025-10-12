library ieee;
use ieee.STD_LOGIC_1164.all;
use ieee.numeric_std.all;

entity Control286 is
  port (

    -- Misc
    i_clk           : in STD_LOGIC;
    i_reset_n       : in STD_LOGIC;
    i_jp1           : in STD_LOGIC;

    o_ale           : buffer STD_LOGIC;
    o_warning       : out STD_LOGIC;
    o_bhe_n         : out STD_LOGIC;

    -- CPU
    i_addr_high     : in STD_LOGIC_VECTOR(3 downto 0); -- A19..A16
    i_addr_low      : in STD_LOGIC_VECTOR(7 downto 0); -- A7..A0
    io_data         : inout STD_LOGIC_VECTOR(7 downto 0); -- D7..D0

    i_s0_n          : in STD_LOGIC;
    i_s1_n          : in STD_LOGIC;
    i_bhe_n         : in STD_LOGIC;
    i_m_io          : in STD_LOGIC;

    o_ready_n       : buffer STD_LOGIC;
    o_nmi           : out STD_LOGIC;
    o_intr          : out STD_LOGIC;

    -- Memory
    i_rom_wr_en     : in STD_LOGIC;

    o_mem_ce_n      : out STD_LOGIC;
    o_rom_oe_n      : out STD_LOGIC;
    o_ram_oe_n      : out STD_LOGIC;
    o_rom_we_low_n  : out STD_LOGIC;
    o_rom_we_high_n : out STD_LOGIC;
    o_ram_we_low_n  : out STD_LOGIC;
    o_ram_we_high_n : out STD_LOGIC;

    o_addr_high     : buffer STD_LOGIC_VECTOR(3 downto 0); -- AC19..AC16

    -- Expansion slot
    o_memrd_n       : out STD_LOGIC;
    o_memwr_n       : out STD_LOGIC;
    o_iord_n        : buffer STD_LOGIC;
    o_iowr_n        : buffer STD_LOGIC;

    i_wait0         : in STD_LOGIC;
    i_wait1         : in STD_LOGIC;

    i_irq0          : in STD_LOGIC;
    i_irq1          : in STD_LOGIC;
    i_irq2          : in STD_LOGIC;
    i_irq3          : in STD_LOGIC;

    i_user1         : in STD_LOGIC;
    i_user2         : in STD_LOGIC;
    i_user3         : in STD_LOGIC;
    i_user4         : in STD_LOGIC

    );

  constant bank_ctrl_addr   : STD_LOGIC_VECTOR(7 downto 3) := "00010"; -- 0x10-17
  constant irq_ctrl_addr    : STD_LOGIC_VECTOR(7 downto 1) := "0001100"; -- 0x18-19
  constant irq_eoi_addr     : STD_LOGIC_VECTOR(7 downto 1) := "0001101"; -- 0x1A-1B
  constant irq_eoi_addr_alt : STD_LOGIC_VECTOR(7 downto 1) := "1010000"; -- 0xA0-A1

end Control286;

architecture rtl of Control286 is

  -- physical pin mappings
  attribute chip_pin : string;

  -- Misc
  attribute chip_pin of i_clk           : signal is "83";
  attribute chip_pin of i_reset_n       : signal is "1";
  attribute chip_pin of i_jp1           : signal is "15";

  attribute chip_pin of o_ale           : signal is "81";
  attribute chip_pin of o_warning       : signal is "12";
  attribute chip_pin of o_bhe_n         : signal is "24";

  -- CPU
  attribute chip_pin of i_addr_high     : signal is "70, 69, 68, 67"; -- A19..A16
  attribute chip_pin of i_addr_low      : signal is "65, 64, 63, 61, 60, 58, 57, 56"; -- A7..A0
  attribute chip_pin of io_data         : signal is "46, 48, 49, 50, 51, 52, 54, 55"; -- D7..D0

  attribute chip_pin of i_s0_n          : signal is "73";
  attribute chip_pin of i_s1_n          : signal is "74";
  attribute chip_pin of i_bhe_n         : signal is "75";
  attribute chip_pin of i_m_io          : signal is "76";

  attribute chip_pin of o_ready_n       : signal is "77";
  attribute chip_pin of o_nmi           : signal is "79";
  attribute chip_pin of o_intr          : signal is "80";

  -- Memory
  attribute chip_pin of i_rom_wr_en     : signal is "2";

  attribute chip_pin of o_mem_ce_n      : signal is "4";
  attribute chip_pin of o_rom_oe_n      : signal is "6";
  attribute chip_pin of o_ram_oe_n      : signal is "10";
  attribute chip_pin of o_rom_we_low_n  : signal is "5";
  attribute chip_pin of o_rom_we_high_n : signal is "11";
  attribute chip_pin of o_ram_we_low_n  : signal is "8";
  attribute chip_pin of o_ram_we_high_n : signal is "9";

  attribute chip_pin of o_addr_high     : signal is "30, 31, 33, 34";

  -- Expansion slot
  attribute chip_pin of o_memrd_n       : signal is "35";
  attribute chip_pin of o_memwr_n       : signal is "36";
  attribute chip_pin of o_iord_n        : signal is "21";
  attribute chip_pin of o_iowr_n        : signal is "22";

  attribute chip_pin of i_wait0         : signal is "18";
  attribute chip_pin of i_wait1         : signal is "20";

  attribute chip_pin of i_irq0          : signal is "29";
  attribute chip_pin of i_irq1          : signal is "28";
  attribute chip_pin of i_irq2          : signal is "27";
  attribute chip_pin of i_irq3          : signal is "25";

  attribute chip_pin of i_user1         : signal is "41";
  attribute chip_pin of i_user2         : signal is "39";
  attribute chip_pin of i_user3         : signal is "40";
  attribute chip_pin of i_user4         : signal is "37";

  -- signals
  signal inta_cycle           : STD_LOGIC;

  -- irq edge triggered latches
  signal irq0_latch           : STD_LOGIC;
  signal irq1_latch           : STD_LOGIC;
  signal irq2_latch           : STD_LOGIC;
  signal irq3_latch           : STD_LOGIC;

  -- signal to clear the irq latch during an interrupt acknowledge cycle
  signal irq0_clear           : STD_LOGIC;
  signal irq1_clear           : STD_LOGIC;
  signal irq2_clear           : STD_LOGIC;
  signal irq3_clear           : STD_LOGIC;

  -- mask input, 0 == input disabled, 1 == enabled
  signal irq0_mask            : STD_LOGIC;
  signal irq1_mask            : STD_LOGIC;
  signal irq2_mask            : STD_LOGIC;
  signal irq3_mask            : STD_LOGIC;

  -- in service register
  signal irq0_isr             : STD_LOGIC;
  signal irq1_isr             : STD_LOGIC;
  signal irq2_isr             : STD_LOGIC;
  signal irq3_isr             : STD_LOGIC;

  -- irq input synchroniser to avoid metastable signals
  signal irq0_meta            : STD_LOGIC;
  signal irq0_sync            : STD_LOGIC;
  signal irq1_meta            : STD_LOGIC;
  signal irq1_sync            : STD_LOGIC;
  signal irq2_meta            : STD_LOGIC;
  signal irq2_sync            : STD_LOGIC;
  signal irq3_meta            : STD_LOGIC;
  signal irq3_sync            : STD_LOGIC;

  signal irq_ctrl_write       : STD_LOGIC;

  signal wait_states          : INTEGER range 0 to 3;

  type t_bank_table is array (0 to 3) of STD_LOGIC_VECTOR(3 downto 0);
  signal bank_table           : t_bank_table;
  signal bank_write           : STD_LOGIC;

  type t_ctrl_state is (TS1, TS2, TC1, TC2);
  signal ctrl_state           : t_ctrl_state;

  signal ram_enable           : STD_LOGIC;
  signal rom_enable           : STD_LOGIC;

  -- latch all 8 low address bits and the optimiser will remove those
  -- we don't need
  signal i_addr_low_latch     : STD_LOGIC_VECTOR(7 downto 0);

begin

  --
  -- interrupt outputs
  --

  o_nmi <= '0';

  -- o_intr <= '1' when (irq0_sync = '1' and irq0_isr = '0') or
  --                    (irq1_sync = '1' and irq0_isr = '0' and irq1_isr = '0') or
  --                    (irq2_sync = '1' and irq0_isr = '0' and irq1_isr = '0' and irq2_isr = '0') or
  --                    (irq3_sync = '1' and irq0_isr = '0' and irq1_isr = '0' and irq2_isr = '0' and irq3_isr = '0')
  --           else '0';

  proc_intr: process(i_reset_n, i_clk) is
  begin

    if i_reset_n = '0' then

      o_intr <= '0';

    elsif falling_edge(i_clk) then

      if (irq0_sync = '1' and irq0_isr = '0') or
         (irq1_sync = '1' and irq0_isr = '0' and irq1_isr = '0') or
         (irq2_sync = '1' and irq0_isr = '0' and irq1_isr = '0' and irq2_isr = '0') or
         (irq3_sync = '1' and irq0_isr = '0' and irq1_isr = '0' and irq2_isr = '0' and irq3_isr = '0') then

        o_intr <= '1';

      else

        o_intr <= '0';

      end if;

    end if;

  end process;

  --
  -- edge triggered interrupt latches
  --

  proc_irq0_latch: process(i_irq0, i_reset_n, irq0_clear, irq0_mask) is
  begin

    if i_reset_n = '0' or irq0_clear = '1' or irq0_mask = '0' then

      irq0_latch <= '0';

    elsif rising_edge(i_irq0) then

      irq0_latch <= '1';

    end if;

  end process;

  proc_irq1_latch: process(i_irq1, i_reset_n, irq1_clear, irq1_mask) is
  begin

    if i_reset_n = '0' or irq1_clear = '1' or irq1_mask = '0' then

      irq1_latch <= '0';

    elsif rising_edge(i_irq1) then

      irq1_latch <= '1';

    end if;

  end process;

  proc_irq2_latch: process(i_irq2, i_reset_n, irq2_clear, irq2_mask) is
  begin

    if i_reset_n = '0' or irq2_clear = '1' or irq2_mask = '0' then

      irq2_latch <= '0';

    elsif rising_edge(i_irq2) then

      irq2_latch <= '1';

    end if;

  end process;

  proc_irq3_latch: process(i_irq3, i_reset_n, irq3_clear, irq3_mask) is
  begin

    if i_reset_n = '0' or irq3_clear = '1' or irq3_mask = '0' then

      irq3_latch <= '0';

    elsif rising_edge(i_irq3) then

      irq3_latch <= '1';

    end if;

  end process;

  --
  -- IRQ input synchroniser
  --

  proc_irq0_synchroniser: process(i_reset_n, i_clk, irq0_clear, irq0_mask) is
  begin

    if i_reset_n = '0' or irq0_clear = '1' or irq0_mask = '0' then

      irq0_meta <= '0';
      irq0_sync <= '0';

    elsif falling_edge(i_clk) then

      irq0_meta <= irq0_latch;
      irq0_sync <= irq0_meta;

    end if;

  end process;

  proc_irq1_synchroniser: process(i_reset_n, i_clk, irq1_clear, irq1_mask) is
  begin

    if i_reset_n = '0' or irq1_clear = '1' or irq1_mask = '0' then

      irq1_meta <= '0';
      irq1_sync <= '0';

    elsif falling_edge(i_clk) then

      irq1_meta <= irq1_latch;
      irq1_sync <= irq1_meta;

    end if;

  end process;

  proc_irq2_synchroniser: process(i_reset_n, i_clk, irq2_clear, irq2_mask) is
  begin

    if i_reset_n = '0' or irq2_clear = '1' or irq2_mask = '0' then

      irq2_meta <= '0';
      irq2_sync <= '0';

    elsif falling_edge(i_clk) then

      irq2_meta <= irq2_latch;
      irq2_sync <= irq2_meta;

    end if;

  end process;

  proc_irq3_synchroniser: process(i_reset_n, i_clk, irq3_clear, irq3_mask) is
  begin

    if i_reset_n = '0' or irq3_clear = '1' or irq3_mask = '0' then

      irq3_meta <= '0';
      irq3_sync <= '0';

    elsif falling_edge(i_clk) then

      irq3_meta <= irq3_latch;
      irq3_sync <= irq3_meta;

    end if;

  end process;

  --
  -- address latch enable
  --

  proc_ale: process(i_reset_n, o_ale) is
    variable bank           : STD_LOGIC_VECTOR(3 downto 0);
    variable bank_index     : INTEGER;
  begin

    if i_reset_n = '0' then

      o_addr_high <= "0000";
      o_bhe_n <= '1';
      rom_enable <= '0';
      ram_enable <= '0';
      i_addr_low_latch <= "00000000";

    elsif rising_edge(o_ale) then

      o_bhe_n <= i_bhe_n;
      rom_enable <= '0';
      ram_enable <= '0';
      i_addr_low_latch <= i_addr_low;

      if i_addr_high(3) = '1' then
        -- accessing top half of memory, use bank table

        bank_index := to_integer(unsigned(i_addr_high(2 downto 1)));
        bank := bank_table(bank_index);

        -- o_addr_high(3 downto 1) <= bank(2 downto 0);
        o_addr_high(3) <= bank(2);
        o_addr_high(2) <= bank(1);
        o_addr_high(1) <= bank(0);

        o_addr_high(0) <= i_addr_high(0);

        if bank(3) = '0' then
          rom_enable <= '1';
        elsif bank(2) = '1' then
          ram_enable <= '1';
        end if;

      else
        -- accessing bottom half of memory, always RAM

        o_addr_high <= i_addr_high;
        ram_enable <= '1';

      end if;

    end if;

  end process;

  --
  -- update the banking table
  --

  proc_bank_write: process(i_reset_n, bank_write) is
    variable bank_index     : INTEGER;
  begin

    if i_reset_n = '0' then

      for i in 0 to 3 loop
        bank_table(i) <= "0000";
      end loop;

    elsif falling_edge(bank_write) then
      -- update the bank table on the falling edge to ensure io_data is stable
      -- use the latched version of i_addr_low as its starting to change on
      -- the falling edge

      bank_index := to_integer(unsigned(i_addr_low_latch(2 downto 1)));
      bank_table(bank_index) <= io_data(3 downto 0);

    end if;

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
    variable bank_index     : INTEGER;
  begin

    if i_reset_n = '0' then

      -- initial signal state
      ctrl_state <= TS1;
      wait_states <= 0;

      bank_write <= '0';
      irq_ctrl_write <= '0';
      inta_cycle <= '0';

      irq0_mask <= '0';
      irq1_mask <= '0';
      irq2_mask <= '0';
      irq3_mask <= '0';

      irq0_clear <= '0';
      irq1_clear <= '0';
      irq2_clear <= '0';
      irq3_clear <= '0';

      irq0_isr <= '0';
      irq1_isr <= '0';
      irq2_isr <= '0';
      irq3_isr <= '0';

      -- initial output pin state
      o_warning <= '0';
      o_ale <= '0';
      o_ready_n <= '1';

      o_rom_oe_n <= '1';
      o_rom_we_low_n <= '1';
      o_rom_we_high_n <= '1';
      o_ram_we_low_n <= '1';
      o_ram_we_high_n <= '1';
      o_ram_oe_n <= '1';
      o_mem_ce_n <= '1';

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

          bank_write <= '0';
          irq_ctrl_write <= '0';

          irq0_clear <= '0';
          irq1_clear <= '0';
          irq2_clear <= '0';
          irq3_clear <= '0';

          o_ready_n <= '1';

          o_rom_oe_n <= '1';
          o_rom_we_low_n <= '1';
          o_rom_we_high_n <= '1';
          o_ram_we_low_n <= '1';
          o_ram_we_high_n <= '1';
          o_ram_oe_n <= '1';
          o_mem_ce_n <= '1';

          o_memrd_n <= '1';
          o_iord_n <= '1';
          o_memwr_n <= '1';
          o_iowr_n <= '1';

          io_data <= "ZZZZZZZZ";

          if i_s0_n = '1' and i_s1_n = '1' then
            -- still waiting for something to happen (idle state)

            ctrl_state <= TS1;
            o_ale <= '0';

          else
            -- the status bits represent something interesting, decode them
            -- to determine what type of bus cycle we're dealing with

            ctrl_state <= TS2;

            -- tell the address latches and the o_addr_high mapper to latch.
            -- this isn't strictly needed by the Halt/Shutdown or Interrupt
            -- Ack cycles, however it makes for simpler CPLD logic to do so
            -- regardless
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

              if i_addr_low(1) = '0' then
                  o_warning <= '1';
              end if;

            elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '1' then
              -- Mem Read

              -- at this point we don't know if this read is from RAM, ROM,
              -- or an external peripheral so enable both RAM and ROM and
              -- use the OE/WE controls to determine which is used during
              -- the TS2 cycle
              o_mem_ce_n <= '0';

            elsif i_m_io = '1' and i_s1_n = '1' and i_s0_n = '0' then
              -- Mem Write

              -- at this point we don't know if this write is to RAM or an
              -- external peripheral so enable RAM and use the OE/WE
              -- controls to determine which is used during the TS2 cycle
              o_mem_ce_n <= '0';

            end if;

          end if;

        --
        when TS2 =>

          ctrl_state <= TC1;
          wait_states <= 0;

          bank_write <= '0';
          irq_ctrl_write <= '0';

          irq0_clear <= '0';
          irq1_clear <= '0';
          irq2_clear <= '0';
          irq3_clear <= '0';

          o_ale <= '0';
          o_ready_n <= '1';

          o_rom_oe_n <= '1';
          o_rom_we_low_n <= '1';
          o_rom_we_high_n <= '1';
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
          -- output an interrupt vector

          if i_m_io = '0' and i_s1_n = '0' and i_s0_n = '0' then
            -- Interrupt Ack

            -- the datasheet says we should add one wait state during the first
            -- interrupt ack to allow the intel interrupt controllers to do their
            -- thing and to add one wait state during the second interrupt ack
            -- to allow the CPU to output an address correctly.
            -- the first doesn't apply (no intel interrupt controller) but the
            -- second does so apply one wait state regardless of first/second
            -- ack to simplify CPLD logic
            wait_states <= 1;

            -- only output the interrupt vector on the second ack cycle
            if inta_cycle = '0' then

              if irq0_sync = '1' then

                io_data <= "00100000"; -- 0x20
                irq0_clear <= '1';
                irq0_isr <= '1';

              elsif irq1_sync = '1' then

                io_data <= "00100001"; -- 0x21
                irq1_clear <= '1';
                irq1_isr <= '1';

              elsif irq2_sync = '1' then

                io_data <= "00100010"; -- 0x22
                irq2_clear <= '1';
                irq2_isr <= '1';

              elsif irq3_sync = '1' then

                io_data <= "00100011"; -- 0x23
                irq3_clear <= '1';
                irq3_isr <= '1';

              end if;

            end if;


          elsif i_m_io = '0' and i_s1_n = '0' and i_s0_n = '1' then
            -- IO Read

            o_iord_n <= '0';

            -- we need at least one wait state to allow peripherals time to
            -- assert wait if they need it and even more for peripherals that
            -- don't assert wait and are designed for a slower CPU (82C55A
            -- for example)
            wait_states <= 3;

            -- Internal peripherals

            if i_addr_low(0) = '0' then
            -- only signal even numbered I/O requests

              -- memory banking control
              if i_addr_low(7 downto 3) = bank_ctrl_addr then
                -- internal peripherals don't need any wait states
                wait_states <= 0;

                -- output the bank table row
                bank_index := to_integer(unsigned(i_addr_low(2 downto 1)));
                -- don't use "0000", adds more macro cells
                io_data(7 downto 4) <= "1111";
                io_data(3 downto 0) <= bank_table(bank_index);
              end if;

              -- irq control
              if i_addr_low(7 downto 1) = irq_ctrl_addr then
                -- internal peripherals don't need any wait states
                wait_states <= 0;

                -- output the irq mask
                -- don't use "0000", adds more macro cells
                io_data(7 downto 4) <= "1111";
                io_data(3) <= irq3_mask;
                io_data(2) <= irq2_mask;
                io_data(1) <= irq1_mask;
                io_data(0) <= irq0_mask;
              end if;

              -- debugging
              -- if i_addr_low(7 downto 1) = irq_eoi_addr then
              --   -- internal peripherals don't need any wait states
              --   wait_states <= 0;
              --   io_data(7) <= irq3_sync;
              --   io_data(6) <= irq2_sync;
              --   io_data(5) <= irq1_sync;
              --   io_data(4) <= irq0_sync;
              --   io_data(3) <= irq3_isr;
              --   io_data(2) <= irq2_isr;
              --   io_data(1) <= irq1_isr;
              --   io_data(0) <= irq0_isr;
              -- end if;

            end if;

          elsif i_m_io = '0' and i_s1_n = '1' and i_s0_n = '0' then
            -- IO Write

            o_iowr_n <= '0';

            -- we need at least one wait state to allow peripherals time to
            -- assert wait if they need it and even more for peripherals that
            -- don't assert wait and are designed for a slower CPU (82C55A
            -- for example)
            wait_states <= 3;

            -- Internal peripherals

            -- at this point i_addr_low is valid but io_data seems flaky(*)
            -- (*) datasheet says 0-27ns to write data @ 20Mhz, CPLD also needs setup time
            -- which exceeds the 25ns before next cycle starts meaning io_data can't be
            -- relied on to be correct during TS2 cycle
            -- we could perform IO write to internal peripherals in the next cycle
            -- but then i_addr_low may not be valid

            -- solution, check i_addr_low here, then raise a signal to another
            -- process to read the io_data

            if i_addr_low(0) = '0' then
              -- only signal even numbered I/O requests

              -- memory banking control
              if i_addr_low(7 downto 3) = bank_ctrl_addr then
                -- internal peripherals don't need any wait states
                wait_states <= 0;
                -- signal the bank write process
                bank_write <= '1';
              end if;

              -- irq mask control
              if i_addr_low(7 downto 1) = irq_ctrl_addr then
                -- internal peripherals don't need any wait states
                wait_states <= 0;
                -- signal irq mask write process
                irq_ctrl_write <= '1';
              end if;

              -- irq eoi
              if i_addr_low(7 downto 1) = irq_eoi_addr or i_addr_low(7 downto 1) = irq_eoi_addr_alt then
                -- internal peripherals don't need any wait states
                wait_states <= 0;

                -- clear the highest priority in service interrupt
                if irq0_isr = '1' then
                  irq0_isr <= '0';
                elsif irq1_isr = '1' then
                  irq1_isr <= '0';
                elsif irq2_isr = '1' then
                  irq2_isr <= '0';
                elsif irq3_isr = '1' then
                  irq3_isr <= '0';
                end if;

              end if;

            end if;

          elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '0' then
            -- Halt/Shutdown

            wait_states <= 0;

          elsif i_m_io = '1' and i_s1_n = '0' and i_s0_n = '1' then
            -- Mem Read

            -- no wait states required for our memory @ 20Mhz
            wait_states <= 0;

            -- read from ROM
            if rom_enable = '1' then
              o_rom_oe_n <= '0';
              -- ROM is marginal @ 20Mhz, one wait state to be safe
              wait_states <= 1;
            end if;

            -- read from RAM
            if ram_enable = '1' then
              o_ram_oe_n <= '0';
            end if;

            -- read from an external peripheral
            if rom_enable = '0' and ram_enable = '0' then
              o_memrd_n <= '0';
              -- assume external peripheral is slow and requires
              -- extra wait states
              wait_states <= 3;
            end if;

          elsif i_m_io = '1' and i_s1_n = '1' and i_s0_n = '0' then
            -- Mem Write

            -- no wait states required for our memory @ 20Mhz
            wait_states <= 0;

            -- write to ROM
            if rom_enable = '1' then

              -- ROM is marginal @ 20Mhz, one wait state to be safe
              wait_states <= 1;

              if i_rom_wr_en = '1' then

                -- write enable (WE) the flash chips (e.g. tell them to read a
                -- value from the data bus)
                -- use BHE/A0 to work out which banks (high/low) need to be enabled

                if i_bhe_n = '0' then
                  -- write to the high bank
                  o_rom_we_high_n <= '0';
                end if;

                if i_addr_low(0) = '0' then
                  -- write to the low bank
                  o_rom_we_low_n <= '0';
                end if;

              else
                -- do nothing, ROM in safe mode

              end if;

            end if;

            -- write to RAM
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

            -- write to an external peripheral
            if rom_enable = '0' and ram_enable = '0' then
              o_memwr_n <= '0';
              -- assume external peripheral is slow and requires
              -- extra wait states
              wait_states <= 3;
            end if;

          end if;

        --
        when TC1 =>

          -- work out if we're going to signal ready or not ready to the CPU

          ctrl_state <= TC2;

          bank_write <= '0';

          irq0_clear <= '0';
          irq1_clear <= '0';
          irq2_clear <= '0';
          irq3_clear <= '0';

          o_ale <= '0';
          o_ready_n <= '1';

          if i_wait0 = '0' and i_wait1 = '0' and wait_states = 0 then
              -- no reason to wait, set the ready signal

            o_ready_n <= '0';

            -- finish the I/O write cycle one clock (25ns) early to allow more hold time
            -- before the CPU changes the value on the data bus
            -- this is potentially useful for the 82C54 (PIT) which requires a 25ns hold time
            -- at the end of the write cycle
            o_iowr_n <= '1';

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
            o_rom_we_low_n <= '1';
            o_rom_we_high_n <= '1';
            o_ram_we_low_n <= '1';
            o_ram_we_high_n <= '1';
            o_ram_oe_n <= '1';
            o_mem_ce_n <= '1';

            o_memrd_n <= '1';
            o_iord_n <= '1';
            o_memwr_n <= '1';
            o_iowr_n <= '1';

            io_data <= "ZZZZZZZZ";

            irq_ctrl_write <= '0';

            -- irq write event
            if irq_ctrl_write = '1' then

              irq0_mask <= io_data(0);
              irq1_mask <= io_data(1);
              irq2_mask <= io_data(2);
              irq3_mask <= io_data(3);

              if io_data(0) = '0' then
                irq0_isr <= '0';
              end if;

              if io_data(1) = '0' then
                irq1_isr <= '0';
              end if;

              if io_data(2) = '0' then
                irq2_isr <= '0';
              end if;

              if io_data(3) = '0' then
                irq3_isr <= '0';
              end if;

            end if;

          end if;

      end case;

    end if;

  end process;

end rtl;
