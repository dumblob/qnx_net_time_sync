#!/usr/bin/env python3

from PythonQt import *

# Globalni objekt
mWidget = QDockWidget()

# Inicializace modulu
# @return True|False
def init():
  mLabel = QLabel()
  mLabel.text = "edge_collector module loaded"
  mLabel.alignment = Qt.AlignCenter
  mWidget.windowTitle = "Edge collector"
  mWidget.setWidget(mLabel)
  mWidget.hide()
  mainWindow.addToDock(Qt.BottomDockWidgetArea, mWidget)
  return True

# Spusteni modulu
# @return True|False
def load():
  mWidget.show()
  # Ziska soucasne pripojeni
  conn = tabs.current()
  if conn is None:
    return

  ## Ziska zarizeni pro exkluzivni pristup
  ## True = s notifikaci, False = bez notifikace
  #dev = conn.detach(True)
  #channel = dev.channelB()              # Kanal B
  ##text = mText.text.encode('ascii')     # Text pole
  ##channel.write('CLEAR\r\n')            # Vymaze displej
  ##channel.write('STRING %s\r\n' % text) # Nastavi text
  #conn.attach()                         # Uvolni pristup terminalu

  conn = tabs.current()
  if conn is None:
    return None # Neni otevreno zadne spojeni
  kit = conn.device()
  channel = kit.channelB()
  res = ''
  while True:
    res += channel.read(1, 0)

  return True

# Ukonceni cinnosti
# @return True|False
def unload():
  mWidget.hide()
  return True
