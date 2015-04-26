import sqlalchemy
from sqlalchemy import *

db = create_engine('sqlite:///perfdb', echo=False)
md = MetaData(db)
accl = Table('Accelerators', md, autoload=True)
hosts = Table('Hosts', md, autoload=True)
perf = Table('Perf', md, autoload=True)
results = Table('Results', md, autoload=True)
sources = Table('Sources', md, autoload=True)
tests = Table('Tests', md, autoload=True)
systems = Table('Systems', md, autoload=True)
