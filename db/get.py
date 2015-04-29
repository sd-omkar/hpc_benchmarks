#!/usr/bin/env python

import sys
import sqlalchemy
from sqlalchemy import *
import json

# Get config data
with open('config.json') as infile:
    config = json.load(infile)

db = create_engine(config['db_url'], echo=False)
conn = db.connect()
md = MetaData(db)

'''
accl = Table('Accelerators', md, autoload=True)
hosts = Table('Hosts', md, autoload=True)
perf = Table('Perf', md, autoload=True)
results = Table('Results', md, autoload=True)
sources = Table('Sources', md, autoload=True)
tests = Table('Tests', md, autoload=True)
systems = Table('Systems', md, autoload=True)
'''

if len(sys.argv) != 2:
    print "Usage: %s tablename" % sys.argv[0]
    sys.exit(1)

# Get table
tablename = sys.argv[1]
table = Table(tablename, md, autoload=True)
s = select([table])

# Get all the results
rows = conn.execute(s)
keys = rows.keys()
for row in rows:
    for key in keys:
      print str(key) + ":" + str(row[key]),
    print " "
