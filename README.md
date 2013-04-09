QNX network time synchronization and evaluation of available methods with their QNX implementations
--------

### get & apply the patch, compile & run the PTPd

create a new directory
~~~~~~
  mkdir ptp_qnx
  cd ptp_qnx
~~~~~~
clone a github project (almost 30MB)
~~~~~~
  git clone --depth 1 https://github.com/dumblob/qnx_net_time_sync.git
~~~~~~
download `PTPd` version 2.2.2
~~~~~~
  http://sourceforge.net/projects/ptpd/files/ptpd/2.2.2/ptpd-2.2.2.tar.gz
~~~~~~
unpack the recently downloaded archive with PTPd
~~~~~~
  tar -xzf ptpd-2.2.2.tar.gz
  cd ptpd-2.2.2
~~~~~~
apply the patch
~~~~~~
  patch -p 1 < ../qnx_net_time_sync/src/ptp/patch.ptpd-2.2.2
~~~~~~
compile `PTPd`
~~~~~~
  cd src/
  make
~~~~~~
run `PTPd` as `master` on interface `eth0` with maximum verbosity
~~~~~~
  ./ptpd2 -G -b eth0 -C -DVfS
~~~~~~
or as slave
~~~~~~
  ./ptpd2 -g -b eth0 -C -DVfS
~~~~~~
