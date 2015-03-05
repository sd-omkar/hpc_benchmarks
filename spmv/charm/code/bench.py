#!/usr/bin/python
import os, math, time, sys

inFiles= os.listdir("/home/idetc-heller/spmv_charm/ang")
			
default_runs = 50000
desiredTimePerFile = 180. # seconds
count = 0

if len(sys.argv) < 2: cores = 32
else: cores = int(sys.argv[1])

sliceBase = 8192 # will be lowered automatically if necessary, i.e. if segfaults/bad_alloc occur


outdir = "bench"+str(cores) # logs will be written here, make sure it exists.

TIMEFMT = "(%H:%M:%S)"

# This estimation of runs per seconds will be used
# to determine the number of runs for a given file.
# If no data is available, 'default_runs' will be used.

runsPerSec ={"2cubes_sphere.csr":766.1467219,
"2D_54019_highK.csr":1039.068994,
"31770-lhs.csr":2214.574468,
"88950-lhs.csr":890.1861393,
"a2nnsnsl.csr":4146.942469,
"a5esindl.csr":4542.789552,
"ABACUS_shell_ud.csr":2613.136214,
"af_5_k101.csr":93.58657244,
"af23560.csr":2181.644577,
"ancfBigDan.csr":1149.530493,
"Andrews.csr":1261.048689,
"apache1.csr":1165.339099,
"apache2.csr":99.93026499,
"appu.csr":783.7272113,
"ASIC_100k.csr":760.0690794,
"ASIC_100ks.csr":829.3235866,
"av41092.csr":956.937799,
"bauru5727.csr":1938.429182,
"bayer01.csr":1332.267519,
"bcircuit.csr":1188.623083,
"bcsstk39.csr":1103.859798,
"blockqp1.csr":1292.88488,
"bmw3_2.csr":159.4467641,
"bmwcra_1.csr":99.37101014,
"boyd1.csr":999.80004,
"bratu3d.csr":5808.931992,
"bundle1.csr":3094.059406,
"c-59.csr":1832.281496,
"c-61.csr":1642.842024,
"c-62.csr":1537.956924,
"cant.csr":703.2065623,
"case39.csr":2136.679953,
"case39_A_01.csr":1856.37492,
"c-big.csr":249.3437965,
"cfd1.csr":934.4047842,
"cfd2.csr":551.3895015,
"circuit_4.csr":1189.626457,
"ckt11752_tr_0.csr":1248.627889,
"cond-mat-2005.csr":5114.099307,
"cont-201.csr":1161.421731,
"cont-300.csr":567.4080799,
"copter2.csr":1400.841055,
"crankseg_1.csr":118.481717,
"d_pretok.csr":479.2944785,
"dawson5.csr":1325.908247,
"dc1.csr":770.7723139,
"dixmaanl.csr":1367.834514,
"Dubcova2.csr":1032.844454,
"dw8192.csr":5016.853933,
"ecl32.csr":1393.922498,
"epb3.csr":874.4316194,
"eurqsa.csr":5364.806867,
"ex11.csr":1478.199416,
"ex19.csr":3169.954688,
"F2.csr":117.8001183,
"FEM_3D_thermal1.csr":2228.60976,
"filter3D.csr":691.1178248,
"finan512.csr":1082.103335,
"fp.csr":1022.704029,
"G3_circuit.csr":45.9273912,
"g7jac140.csr":1262.612822,
"Ga3As3H12.csr":202.1664305,
"GaAsH6.csr":740.1549681,
"garon2.csr":2066.969822,
"gas_sensor.csr":843.4547908,
"gridgena.csr":1511.461447,
"gsm_106857.csr":46.08715755,
"H2O.csr":799.1351351,
"hcircuit.csr":863.973186,
"HTC_336_4438.csr":439.8238246,
"ibm_matrix_2.csr":1006.540572,
"inline_1.csr":56.75571041,
"jan99jac120.csr":1812.721277,
"juba40k.csr":1914.975105,
"k1_san.csr":967.0595449,
"laminar_duct3D.csr":244.220799,
"ldoor.csr":32.97688316,
"lhr10c.csr":3366.491924,
"lhr71.csr":724.3227582,
"Lin.csr":357.7552922,
"lung2.csr":793.9963373,
"mario002.csr":264.8164822,
"mark3jac100.csr":1290.322581,
"mark3jac140.csr":1030.927835,
"matrix_9.csr":578.6145979,
"minsurfo.csr":1639.132109,
"msc23052.csr":1550.823842,
"ncvxbqp1.csr":1472.385928,
"nd24k.csr":75.76569038,
"offshore.csr":290.5682194,
"oilpan.csr":762.6601586,
"olesnik0.csr":982.7044025,
"OPF_10000.csr":2112.378538,
"parabolic_fem.csr":164.9019042,
"pdb1HYS.csr":639.5343631,
"poisson3Db.csr":538.5212232,
"pwtk.csr":143.9721915,
"qa8fk.csr":879.0436006,
"qa8fm.csr":873.9730816,
"raefsky4.csr":1636.125654,
"rail_79841.csr":861.3264427,
"rajat26.csr":1407.467668,
"rajat30.csr":78.70074547,
"rajat31.csr":14.09978308,
"rma10.csr":707.0135747,
"s3dkq4m2.csr":571.0370032,
"shallow_water1.csr":1035.456077,
"shallow_water2.csr":1076.588022,
"ship_003.csr":352.1539638,
"shipsec1.csr":227.2113326,
"shipsec5.csr":115.3628857,
"Si34H36.csr":201.1122831,
"SiO2.csr":101.1451563,
"sparsine.csr":1089.087345,
"stokes128.csr":1354.783888,
"stomach.csr":174.9770537,
"t3dh.csr":598.7963181,
"t3dh_a.csr":744.7321954,
"thermal1.csr":1128.228508,
"thermal2.csr":50.38266681,
"torso3.csr":108.26819,
"TSOPF_FS_b162_c4.csr":1459.427904,
"TSOPF_FS_b39_c19.csr":1400.560224,
"turon_m.csr":470.3668862,
"vanbody.csr":1282.709082,
"venkat25.csr":746.6440678,
"vibrobox.csr":3407.312317,
"xenon1.csr":1031.130143,
"xenon2.csr":144.6607049}
			
for f in inFiles:
	if f in runsPerSec:
		runs = int(math.ceil(runsPerSec[f]*desiredTimePerFile))
	else: runs = default_runs
	
	slice=sliceBase

	cmd = "./charmrun +p%d ./spmv -I %s -n %d -m %d > %s/%s.log" % (cores, "/home/idetc-heller/spmv_charm/ang/"+f, runs, slice, outdir, f)
	print time.strftime(TIMEFMT), cmd
	
	while 0 != os.system(cmd) and slice >= 512:
		slice /= 2
		cmd = "./charmrun +p%d ./spmv -I %s -n %d -m %d > %s/%s.log" % (cores, "/home/idetc-heller/spmv_charm/ang/"+f, runs, slice, outdir, f)
		print time.strftime(TIMEFMT), cmd

	count += 1
	print time.strftime(TIMEFMT), "%s finished - %d/%d done." % (f, count, len(inFiles))