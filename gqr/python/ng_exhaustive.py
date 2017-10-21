#!/usr/bin/env python

import gqr
import itertools

def print_csp(c):
	for i in xrange(0, c.get_size()):
		for j in xrange(i+1, c.get_size()):
			print i, j, c.get_constraint(i, j)

def cng(a,b):
	for i in xrange(0, a.get_size()):
		for j in xrange(i+1, a.get_size()):
			rel = a.get_constraint(i,j)
			if rel == "( )":
				return
			if "=" in rel:
				continue
			if not "<" in rel:
				b.add_constraint(i, j, "( = > )")
			if not ">" in rel:
				b.add_constraint(i, j, "( = < )")

def dfs(a,b):
	BACKUP_A = [ a.get_constraint(x[0], x[1]) for x in all_edges ]
	BACKUP_B = [ b.get_constraint(x[0], x[1]) for x in all_edges ]

	solver.enforce_algebraic_closure(a)
	solver.enforce_algebraic_closure(b)
	cng(a,b)
	cng(b,a)
	if not solver.enforce_algebraic_closure(a) or not solver.enforce_algebraic_closure(b):
		for r, x in zip(BACKUP_A, all_edges):
			a.set_constraint(x[0], x[1], r)
		for r, x in zip(BACKUP_B, all_edges):
			b.set_constraint(x[0], x[1], r)
		return False

	for i in xrange(0, a.get_size()):
		for j in xrange(i+1, a.get_size()):
			val = a.get_constraint(i,j)[2:-2]
			val.strip()
			val = val.split(' ')
			if len(val) == 1:
				continue

			for base in val:
				a.set_constraint(i, j, "( %s )" % base)
				ret = dfs(a,b)
				if ret == True:
					return True

			for r, x in zip(BACKUP_A, all_edges):
				a.set_constraint(x[0], x[1], r)
			for r, x in zip(BACKUP_B, all_edges):
				b.set_constraint(x[0], x[1], r)
			return False

	for i in xrange(0, b.get_size()):
		for j in xrange(i+1, b.get_size()):
			val = b.get_constraint(i,j)[2:-2]
			val.strip()
			val = val.split(' ')
			if len(val) == 1:
				continue

			for base in val:
				b.set_constraint(i, j, "( %s )" % base)
				ret = dfs(a,b)
				if ret == True:
					return True

			for r, x in zip(BACKUP_A, all_edges):
				a.set_constraint(x[0], x[1], r)
			for r, x in zip(BACKUP_B, all_edges):
				b.set_constraint(x[0], x[1], r)
			return False

	return True

SIZE = 5


point = gqr.Calculus("point", "../../data")
#powerset = [ "( < )", "( = )", "( > )", "( < = )", "( > = )", "( < > )", "( < = > )" ]
powerset = [ "( < )", "( = )", "( > )", "( < = > )" ]
all_edges = [ (a,b) for (a,b) in itertools.product(range(0, SIZE), repeat=2)]
edges = [ (a,b) for (a,b) in itertools.product(range(0, SIZE), repeat=2) if a < b]

solver = gqr.Solver(point)

ok = 0
total_networks = 0

for _ in itertools.product(powerset, repeat=(SIZE*SIZE-SIZE)/2):
	total_networks += 1

count = 0
for n1 in itertools.product(powerset, repeat=(SIZE*SIZE-SIZE)/2):
	assert len(edges) == len(n1)

	print "%.2f ..." % ((float(count) / float(total_networks))*100.0)
	count += 1

	csp_a = gqr.CSP(SIZE, point)
	for x, r in zip(edges, n1):
		csp_a.set_constraint(x[0], x[1], r)

	if not solver.enforce_algebraic_closure(csp_a):
		continue

	for n2 in itertools.product(powerset, repeat=(SIZE*SIZE-SIZE)/2):

		# reset
		csp_a = gqr.CSP(SIZE, point)
		for x, r in zip(edges, n1):
			csp_a.set_constraint(x[0], x[1], r)
		if not solver.enforce_algebraic_closure(csp_a):
			assert False

		csp_b = gqr.CSP(SIZE, point)
		for y, s in zip(edges, n2):
			csp_b.set_constraint(y[0],y[1], s)

		if not solver.enforce_algebraic_closure(csp_b):
			continue

		cng(csp_a, csp_b)
		solver.enforce_algebraic_closure(csp_b)
		cng(csp_b, csp_a)
		solver.enforce_algebraic_closure(csp_a)
		cng(csp_a, csp_b)
		cng(csp_b, csp_a)

		if not solver.enforce_algebraic_closure(csp_a) or not solver.enforce_algebraic_closure(csp_b):
			continue

		if not dfs(csp_a, csp_b):
			print
			print "[ERROR]"
			print_csp(csp_a)
			print "-|->"
			print_csp(csp_b)
			assert False
		else:
			ok += 1

print ok, "hops %d x %d OK" % (SIZE, SIZE)
