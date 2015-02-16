#!/usr/bin/python

"""

Purpose of the program:
Convert COO ascii-format input file matrices to
an efficient binary CSR storage output where just

 number of rows, number of entries
 and vals, col_idx, row_start fields
 
are stored.

"""

import os, sys, glob, struct, argparse, types
import scipy.sparse, numpy

def convert_vec(input, output):
	values = []
	
	with open(input, 'r') as f:
		for line in f:
			if(line.startswith("%")): continue
			
			try:
				val = float(line.strip())
				values.append(val)
			except ValueError:
				print "ERROR in %s: Could not read value: '%s'. Stopping." %(input, line)
				return 1
				
	N = len(values)
	with open(output, 'wb') as f:
		f.write(struct.pack("i", N))
		for v in values:
			f.write(struct.pack("d", v))
			
	return 0

def convert(input, output):
	max_row = 0
	nnz = 0
	
	with open(input, 'r') as f:
		print "Parsing %s." % input
		for line in f:
			if(line.startswith("%")): continue
			
			read_args = line.strip().split()
			
			if(len(read_args)!=3):
				print "ERROR in %s: Could not read line: '%s'. Stopping." % (input,line.strip())
				return 1
			
			try:
				row = int(read_args[0])
				col = int(read_args[1])
				value = float(read_args[2])
			except ValueError:
				print "ERROR in %s: Could not read values from line: '%s'. Stopping." % (input,line.strip())
				return 1
			
			# from 1-based to 0-based
			row -= 1; col -= 1;
			nnz += 1
			if(row>max_row): max_row = row
			
	# allocate and read a second time to save memory
	N = max_row + 1

	rows = numpy.empty(nnz, int)
	cols = numpy.empty(nnz, int)
	values = numpy.empty(nnz, float)

	with open(input, 'r') as f:
		count = 0
		for line in f:
			if(line.startswith("%")): continue
			
			read_args = line.strip().split()
	
			row = int(read_args[0])
			col = int(read_args[1])
			value = float(read_args[2])
			
			# from 1-based to 0-based
			row -= 1; col -= 1;
			rows[count] = row; cols[count] = col; values[count] = value;
			count += 1

	density = 1.*nnz/N/N
	if(density>=.001):
		print "%d rows with %d entries read (density %.2f%%), %d KB" % (N, nnz, 100.*density, (12*nnz+4*(N+1)+8)/1024)
	else:
		print "%d rows with %d entries read (density %.2E), %d KB" % (N, nnz, density, (12*nnz+4*(N+1)+8)/1024)
	M_coo = scipy.sparse.coo_matrix((values,(rows,cols)))
	print "Converting to CSR storage."
	M_csr = M_coo.tocsr()
	del M_coo
	
	# # test
	# print "values:", M_csr.data, type(M_csr.data)
	# print "col_idx:", M_csr.indices, type(M_csr.indices)
	# print "row_start:", M_csr.indptr, type(M_csr.indptr)
	
	# M_csr.data.tobytes()
			
	with open(output, 'wb') as f:
		print "Writing output %s." % output
		f.write(struct.pack("i", N))
		f.write(struct.pack("i", nnz))
		f.write(M_csr.data.tobytes())
		f.write(M_csr.indices.tobytes())
		f.write(M_csr.indptr.tobytes())
	
	return 0

def main(args):
	input = args.input
	output = args.output
	vec = args.v
	
	if os.path.isdir(output):
		count = 0
		errors = 0
		for file in input:
			base_name_with_path = os.path.splitext(file)[0]
			base_name = os.path.split(base_name_with_path)[1]
			out_name = base_name + (".csr" if not vec else ".vec")
			outfile = os.path.join(output, out_name)
			
			if not vec: res = convert(file, outfile)
			else:
				print "%s > %s" % (file, outfile)
				res = convert_vec(file, outfile)
			
			if(res==0): count += 1
			else: errors += 1
			
			print "(%d/%d, %d errors)" % (count+errors, len(input), errors)
		return (errors==0)
	
	elif len(input)==1 and (not os.path.isdir(output)):
		if not vec: return convert(input[0], output)
		else: return convert_vec(input[0], output)

	else:
		print "For multiple input files, please specify an output folder instead" \
			  +" of a filename."
	
	return 0;
	
if(__name__=="__main__"):
	ap = argparse.ArgumentParser()
	
	ap.add_argument("input", nargs='+', help="Input file in matrix market format")
	ap.add_argument("output", help="Output file to write to binary")
	ap.add_argument("-v", action='store_true', help="Vector conversion ascii to binary")
	
	args = ap.parse_args()
	
	main(args)
