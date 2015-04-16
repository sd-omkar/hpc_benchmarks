import sqlalchemy
from sqlalchemy import *

db = create_engine('sqlite:///perfdb', echo=False)
md = MetaData(db)
hosts = Table('Hosts', md, autoload=True)
accl = Table('Accelerators', md, autoload=True)
systems = Table('Systems', md, autoload=True)

'''
i = hosts.insert()
i.execute(hostname = 'lagrange')
i.execute(hostname = 'euler99')
i.execute(hostname = 'euler30')
i.execute(hostname = 'tesla')
i.execute(hostname = 'kaveri')

i = accl.insert()
i.execute(type='CPU', accl_name='Xeon E5-2690v2', arch='Ivy Bridge-EP', memory=64, cores=40, details='http://ark.intel.com/products/75279/Intel-Xeon-Processor-E5-2690-v2-25M-Cache-3_00-GHz')
i.execute(type='CPU', accl_name='Core i7-5960X', arch='Haswell-E', memory=32, cores=16, details='http://ark.intel.com/products/82930/Intel-Core-i7-5960X-Processor-Extreme-Edition-20M-Cache-up-to-3_50-GHz')
i.execute(type='CPU', accl_name='Opteron 6274', arch='Bulldozer', memory=128, cores=64, details='http://products.amd.com/pages/OpteronCPUDetail.aspx?id=760')
i.execute(type='GPU', accl_name='GTX 770', arch='Kepler', memory=4, cores=1536, details='http://www.nvidia.com/gtx-700-graphics-cards/gtx-770/')
i.execute(type='GPU', accl_name='Tesla K20Xm', arch='Kepler', memory=5.76, cores=2688, details='http://www.nvidia.com/content/PDF/kepler/Tesla-K20-Active-BD-06499-001-v04.pdf')
i.execute(type='GPU', accl_name='Tesla K40c', arch='Kepler', memory=11.52, cores=2880, details='http://www.nvidia.com/object/tesla-servers.html')
i.execute(type='APU', accl_name='A10-7850K', arch='Kaveri', memory=16, cores=512, details='http://www.amd.com/en-us/products/processors/desktop/a-series-apu')
i.execute(type='MIC', accl_name='Xeon Phi 5110P', arch='Knights Corner', memory=8, cores=60, details='http://ark.intel.com/products/71992/Intel-Xeon-Phi-Coprocessor-5110P-8GB-1_053-GHz-60-core')

i = systems.insert()
i.execute(OS='Linux', compiler='g++', version=4.81, flags='-O3', library='Blaze', library_ver=2.2, backend='OpenMP', backend_ver=3.1)
i.execute(OS='Linux', compiler='g++', version=4.81, flags='-O3', library='Blaze', library_ver=2.2, backend='C++', backend_ver=4.81)
i.execute(OS='Linux', compiler='g++', version=4.81, flags='-O3', library='Blaze', library_ver=2.2, backend='Boost', backend_ver=1.53)
i.execute(OS='Linux', compiler='icc', version=14.0, flags='-O3', library='MKL', library_ver=11.1, backend='OpenMP', backend_ver=3.1)
i.execute(OS='Linux', compiler='g++', version=4.81, flags='-O3', library='Thrust', library_ver=1.7, backend='OpenMP', backend_ver=3.1)
i.execute(OS='Linux', compiler='icpc', version=14.0, flags='-O3', library='Thrust', library_ver=1.7, backend='OpenMP', backend_ver=3.1)
i.execute(OS='Linux', compiler='nvcc', version=6.5, flags='-O3', library='Thrust', library_ver=1.7, backend='CUDA', backend_ver=6.5)
i.execute(OS='Linux', compiler='g++', version=4.81, flags='-O3', library='VexCL', library_ver=1.32, backend='OpenCL', backend_ver=1.2)
'''
