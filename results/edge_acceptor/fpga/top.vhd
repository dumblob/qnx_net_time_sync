-- top_level.vhd : needed for proper compilation

library IEEE;
use IEEE.std_logic_1164.ALL;
use IEEE.std_logic_ARITH.ALL;
use IEEE.std_logic_UNSIGNED.ALL;

--architecture fpga_inst of tlv_bare_ifc is
--begin
--end fpga_inst;

architecture main of fpga is
begin
  RA            <= (others => 'Z');
  LE            <= 'Z';
  RCAS          <= 'Z';
  RCKE          <= 'Z';
  RCS           <= 'Z';
  RCLK          <= 'Z';
  KIN           <= (others => 'Z');
  LRS           <= 'Z';
  KIN           <= (others => 'Z');
  LRW           <= 'Z';
  KIN           <= (others => 'Z');
  RDQM          <= 'Z';
  SPI_DI        <= 'Z';
  RWE           <= 'Z';
  LEDF          <= 'Z';
  RRAS          <= 'Z';

  -- FIXME how to use user-specified ucf file?
  -- FPGAUCF=my_ucf_file.ucf

  X(42) <= AFBUS(1);
  X(43) <= AFBUS(0);
  X(44) <= AFBUS(2);
  X(45) <= AFBUS(3);
end main;

--architecture main of tlv_bare_ifc is
--begin
--end main;


--  rising_edge() falling_edge() functions???
--
--  process (clk) is
--    variable qr, qf : std_logic;
--  begin
--    if rising_edge(clk) then
--      qr := d xor qf; -- registered encode of input
--    elsif falling_edge(clk) then
--      qf := d xor qr; -- registered encode of input
--    end if;
--    q <= qr xor qf; -- combinatorial decode of output
--  end process;
--
--  -- the same as the process above:
--  qr <= d xor qf when rising_edge(clk);
--  qf <= d xor qr when falling_edge(clk);
--  q  <= qr xor qf;
--
--  http://asicdigitaldesign.wordpress.com/2009/06/24/dual-edge-binary-counters/
--  http://asicdigitaldesign.wordpress.com/2007/07/31/the-double-edge-flip-flop/
--  http://www.edaboard.com/thread133082.html
--  http://esd.cs.ucr.edu/labs/tutorial/ (examples of edge detection with latch...)
