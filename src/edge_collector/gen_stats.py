#!/usr/bin/env python3

from math import fabs
import sys
import numpy

ALLOWED_ACTIONS = ('both', 'diff', 'sec')

if len(sys.argv) != 4:
  print('USAGE: ' + sys.argv[0] + ' (both|diff|sec) <dir-with-results.raw> <freq>')
  exit(0)

action, res_dir, freq = sys.argv[1], sys.argv[2], float(sys.argv[3])

if action not in ALLOWED_ACTIONS:
  print('Unknown action: ' + action, file=sys.stderr)
  exit(1)

output_degree = 1000  # 1 ~ seconds
                      # 1000 ~ ms
                      # 1000000 ~ us
                      # ...

# mode = 'r'
with open(res_dir + '/results.raw') as f:
  i = 1
  all_results = {}
  last_master = None
  last_slave = None

  for line in f:
    if line.startswith('M'):
      l = line.split(sep = None)

      if action == 'both':
        if 'master' not in all_results:
          all_results['master'] = []
        all_results['master'].append(int(l[1]))
        if 'slave' not in all_results:
          all_results['slave'] = []
        all_results['slave'].append(int(l[3]))
        print(str(i) + ' ' + str((float(l[1]) * output_degree)/freq) + ' ' +
                             str((float(l[3]) * output_degree)/freq))

      elif action == 'diff':
        diff = int(l[3]) - int(l[1])
        #diff = fabs(int(l[3]) - int(l[1]))
        if 'diff' not in all_results:
          all_results['diff'] = []
        all_results['diff'].append(diff)
        print(str(i) + ' ' + str((float(diff) * output_degree)/freq))

      if action == 'sec':
        if 'master' not in all_results:
          all_results['master'] = []
        tmp = int(l[1])
        if last_master != None:
          all_results['master'].append(tmp - last_master)
        last_master = tmp
        if 'slave' not in all_results:
          all_results['slave'] = []
        tmp = int(l[3])
        if last_slave != None:
          all_results['slave'].append(tmp - last_slave)
        last_slave = tmp
        if all_results['master'] and all_results['slave']:
          print(str(i) + ' ' +
              str((float(all_results['master'][-1]) * output_degree)/freq) + ' ' +
              str((float(all_results['slave'][-1])  * output_degree)/freq))

      i += 1

with open(res_dir + '/stats', mode = 'a') as f:
  print(action, file = f)
  for k, l in all_results.items():
    #if len(l) == 0:
    #  continue
    mean = numpy.mean(l)/float(freq)
    median = numpy.median(l)/float(freq)
    max_seconds = float(max(l))/float(freq)
    min_seconds = float(min(l))/float(freq)

    print('  ' + k + '\n' +
        '    MAX: ' + str(max_seconds * output_degree) + 'ms\n' +
        '    MIN: ' + str(min_seconds * output_degree) + 'ms\n' +
        '    MEAN: ' + str(mean * output_degree) + 'ms\n' +
        '    MEDIAN: ' + str(median * output_degree) + 'ms',
        file = f)
