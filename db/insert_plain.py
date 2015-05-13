#!/usr/bin/env python

import sys
import subprocess
import sqlite3
import json
import datetime

# Get config data
with open('config.json') as infile:
    config = json.load(infile)

# Create DB connection
db = sqlite3.connect(config['db_name'])

# Run the executable
if len(sys.argv) != 3:
    print "Usage: %s benchmark.out size" % sys.argv[0]
    sys.exit(1)

exe = sys.argv[1]
size = sys.argv[2]

# Parse the output
out = subprocess.check_output([exe, size])

data = {}
for line in out.splitlines():
    words = line.split(" = ")
    data[words[0]] = words[1]

# Tests
cursor_test = db.cursor()
now = datetime.datetime.now()
cursor_test.execute("insert into tests (date, name, input, datatype, dim_x, dim_y, NNZ, host_id, accl_id, source_id, system_id) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", 
                    (now, data['name'], data['input'], data['datatype'], 
                      data['dim_x'], data['dim_y'], data['NNZ'], 
                      config['host_id'], config['hw_id'],
                      config['source_id'], config['system_id']))

print "test_id = " + str(cursor_test.lastrowid)

# Results
cursor_result = db.cursor()
cursor_result.execute("insert into results (value_type, value, perf_id, test_id) values (?, ?, ?, ?)", 
                      (data['value_type'], data['value'], 
                        config['perf_id'], cursor_test.lastrowid))
print "result_id = " + str(cursor_result.lastrowid)

db.commit()
db.close()
print "DONE"
