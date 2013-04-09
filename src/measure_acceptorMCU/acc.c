/**
 * detector&counter of high&low levels on serial port
 * Jan Pacner xpacne00@stud.fit.vutbr.cz
 * 2013-01-06 16:48:43 CET
 */

/*
  VHDL

  rising_edge() falling_edge() functions???

  process (clk) is
    variable qr, qf : std_logic;
  begin
    if rising_edge(clk) then
      qr := d xor qf; -- registered encode of input
    elsif falling_edge(clk) then
      qf := d xor qr; -- registered encode of input
    end if;
    q <= qr xor qf; -- combinatorial decode of output
  end process;

  -- the same as the process above:
  qr <= d xor qf when rising_edge(clk);
  qf <= d xor qr when falling_edge(clk);
  q  <= qr xor qf;

  http://asicdigitaldesign.wordpress.com/2009/06/24/dual-edge-binary-counters/
  http://asicdigitaldesign.wordpress.com/2007/07/31/the-double-edge-flip-flop/
  http://www.edaboard.com/thread133082.html
  http://esd.cs.ucr.edu/labs/tutorial/ (examples of edge detection with latch...)
*/

#include <fitkit_special_headers.h>

int main()
{
  return 0;
}
