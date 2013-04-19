U1 = 9
U2 = 2.6  #2.2  #3.0

R1 = ''
R2 = ''

#R1 = 2000  R2 = 812.5 U1=9 U2=2.6V
#R1 = 8300  R2 = 2000 U1=9 U2=1.7V
#R1 = 10000 R2 = 2000 U1=9 U2=1.5V

resistors = (10000, 8300, 2000)

def voltage_divider(R1, R2, U1, U2):
  if U1=="":
    U1 = U2*(1.0+(R1/R2))
  elif U2=="":
    U2 = U1*(R2/(R1+R2))
  elif R1=="":
    R1 = R2*(U1/U2-1)
  elif R2=="":
    R2 = R1*(U2/(U1-U2))

  if 2.25 < U2 < 3.0:
    print('R1=' + str(R1) + ' R2=' + str(R2) +
         ' U1=' + str(U1) + ' U2=' + str(U2))

for R1 in resistors:
  for R2 in resistors:
    if R1 == R2:
      continue
  #  for x in resistors:
  #    if R1 == R2 or R1 == x or R2 == x:
  #      continue
    voltage_divider(R1, '', U1, U2)
    voltage_divider('', R2, U1, U2)

  #    # seriove zapojeni
  #    voltage_divider(R1 + x, R2, U1, U2)
  #    voltage_divider(R1, R2 + x, U1, U2)

  #    # paralelni zapojeni
  #    voltage_divider(1/R1 + 1/x, R2, U1, U2)
  #    voltage_divider(R1, 1/R2 + 1/x, U1, U2)
