#!/usr/bin/env python3

from math import fabs
import sys
import numpy

# length of one second on FITkit (a mean from means from the GPS measurements)
FITKIT_SECOND = 100008701811 + \
                100008696049 + \
                100008691020 + \
                100008687052
# we must divide by this if we want to use the FITKIT_SECOND
# useful to gain higher precision (int in Python has indefinite precision)
FITKIT_MULT   = 100000000000 * 4

ALLOWED_ACTIONS = ('both', 'diff', 'sampling_period')

if len(sys.argv) != 4:
  print('USAGE: ' + sys.argv[0] + ' (both|diff|sampling_period) <dir-with-results.raw> <freq>')
  exit(0)

action, res_dir, freq = sys.argv[1], sys.argv[2], float(sys.argv[3])

if action not in ALLOWED_ACTIONS:
  print('Unknown action: ' + action, file = sys.stderr)
  exit(1)

# normalize to the GPS time (from the FITkit time)
# 1 ~ seconds; 1000 ~ ms; 1000000 ~ us; ...
unit_degree = float(1000 * FITKIT_MULT) / float(FITKIT_SECOND)
unit = 'ms'

def print_master_slave(i, M, S, **for_print):
  print(str(i) + ' ' + str((float(M) * unit_degree)/freq) + ' ' +
                       str((float(S) * unit_degree)/freq), **for_print)

def print_diff(i, diff, **for_print):
  print(str(i) + ' ' + str((float(diff) * unit_degree)/freq), **for_print)

# mode = 'r'
with open(res_dir + '/results.raw') as f:
  i = 1
  all_results = {}
  last_master = None
  last_slave = None

  _M_orig = 0
  _S_orig = 0
  M_overflows = 0
  S_overflows = 0

  for line in f:
    if line.startswith('M'):
      l = line.split(sep = None)

      M_orig = int(l[1])  # master
      S_orig = int(l[3])  # slave

      # our counter in FitKit is 32bit (simulated from 16bit hardware counter)
      #   => correct the values (we would need 64b counter)
      if M_orig < _M_orig:
        M_overflows += 1
        #M = ((_M_count * (2**32)) - _M) + M

      if S_orig < _S_orig:
        S_overflows += 1
        #S = ((_S_count * (2**32)) - _S) + S

      _M_orig = M_orig
      _S_orig = S_orig

      M = M_overflows * (2**32) + M_orig
      S = S_overflows * (2**32) + S_orig

      if action == 'both':
        if 'master' not in all_results:
          all_results['master'] = []
        all_results['master'].append(M)
        if 'slave' not in all_results:
          all_results['slave'] = []
        all_results['slave'].append(S)
        print_master_slave(i, M, S)

      elif action == 'diff':
        diff = S - M
        #diff = fabs(S - M)
        if 'diff' not in all_results:
          all_results['diff'] = []
        all_results['diff'].append(diff)
        print_diff(i, diff)

      elif action == 'sampling_period':
        if 'master' not in all_results:
          all_results['master'] = []
        tmp = M
        if last_master != None:
          all_results['master'].append(tmp - last_master)
        last_master = tmp
        if 'slave' not in all_results:
          all_results['slave'] = []
        tmp = S
        if last_slave != None:
          all_results['slave'].append(tmp - last_slave)
        last_slave = tmp
        if all_results['master'] and all_results['slave']:
          print_master_slave(i, all_results['master'][-1],
                                all_results['slave'][-1])

      i += 1

with open(res_dir + '/results.' + action + '.filtered', mode = 'w') as f:
  if action == 'both' or action == 'sampling_period':
    M_two_standard_deviations = 2 * numpy.std(all_results['master'])
    M_mean = numpy.mean(all_results['master'])
    S_two_standard_deviations = 2 * numpy.std(all_results['slave'])
    S_mean = numpy.mean(all_results['slave'])

    for M, S, i in zip(
        all_results['master'], all_results['slave'], range(sys.maxsize)):
      if (M_mean - M_two_standard_deviations) < M < \
         (M_mean + M_two_standard_deviations) and \
         (S_mean - S_two_standard_deviations) < S < \
         (S_mean + S_two_standard_deviations):
        print_master_slave(i, M, S, file = f)

  if action == 'diff':
    D_two_standard_deviations = 2 * numpy.std(all_results['diff'])
    D_mean = numpy.mean(all_results['diff'])
    for D, i in zip(all_results['diff'], range(sys.maxsize)):
      if (D_mean - D_two_standard_deviations) < D < \
         (D_mean + D_two_standard_deviations):
        print_diff(i, D, file = f)

with open(res_dir + '/stats', mode = 'a') as f:
  print(action, file = f)
  for k, l in all_results.items():
    #if len(l) == 0:
    #  continue
    stddev = numpy.std(l)/float(freq)
    mean = numpy.mean(l)/float(freq)
    median = numpy.median(l)/float(freq)
    max_seconds = float(max(l))/float(freq)
    min_seconds = float(min(l))/float(freq)

    if M_overflows > 0 or S_overflows > 0:
      overflow = 'Yes'
    else:
      overflow = 'No'

    print('  ' + k + '\n' +
        '    FitKit 32bit counter overflow occured: ' + overflow + '\n' +
        '    MAX: ' + str(max_seconds * unit_degree) + unit + '\n' +
        '    MIN: ' + str(min_seconds * unit_degree) + unit + '\n' +
        '    STDDEV: ' + str(stddev * unit_degree) + unit + '\n' +
        '    MEAN: ' + str(mean * unit_degree) + unit + '\n' +
        '    MEDIAN: ' + str(median * unit_degree) + unit,
        file = f)
