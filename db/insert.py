#!/usr/bin/env python

import sys
import subprocess
import sqlalchemy
from sqlalchemy import *
import json
import datetime

# Get config data
with open('config.json') as infile:
    config = json.load(infile)

# Create DB connection
db = create_engine(config['db_url'], echo=False)
md = MetaData(db)

# Get table objects
accl = Table('Accelerators', md, autoload=True)
hosts = Table('Hosts', md, autoload=True)
perf = Table('Perf', md, autoload=True)
results = Table('Results', md, autoload=True)
sources = Table('Sources', md, autoload=True)
tests = Table('Tests', md, autoload=True)
systems = Table('Systems', md, autoload=True)

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
i = tests.insert()
now = datetime.datetime.now()
ret = i.execute(date=now,
                name=data['name'],
                input=data['input'],
                datatype=data['datatype'],
                dim_x=data['dim_x'],
                dim_y=data['dim_y'],
                NNZ=data['NNZ'],
                host_id = config['host_id'],
                accl_id = config['accl_id'],
                source_id=config['source_id'],
                system_id=config['system_id'])
print "test_id = %s" % ret.inserted_primary_key[0]

# Results
i = results.insert()
ret = i.execute(value_type=data['value_type'],
                value=data['value'],
                perf_id=config['perf_id'],
                test_id=ret.inserted_primary_key[0])
print "results_id = %s" % ret.inserted_primary_key[0]

print "DONE"
