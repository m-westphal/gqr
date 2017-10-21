#!/usr/bin/env python

import gqr
import sys
import random

allen = gqr.Calculus("allen", "../../data")

def missing_rels(rc):
    """Returns a list of missing rels that are to be added such that rc
    becomes closed under composition"""
    c = [allen.get_composition(r1, r2) for r1 in rc for r2 in rc]
    return list(set(c) - set(rc))

def close_class_under_cmps(rc):
    """Closes the input relation class under composition:
    the function does *not* close the set under converses"""
    mrs = missing_rels(rc)
    if not mrs:
        return rc
    rc.extend(mrs)
    return close_class_under_cmps(rc)


ex_rc = ["(s < d m)"]
assert missing_rels(ex_rc) == ['( < d m o s )']

ex_rc = ["(< d s m d)"]
assert missing_rels(ex_rc) == ['( < d m o s )']

ex_rc = ["(< d s m)"]
assert missing_rels(ex_rc) == ['( < d m o s )']

ex_rc = ["( < d s m )"]
assert missing_rels(ex_rc) == ['( < d m o s )']

print "Test1: Close %s under composition:\n %s\n-----" % (ex_rc, close_class_under_cmps(ex_rc))

with open("../../data/allen/calculus/contalg") as f:
    rc = [x.strip() for x in f]
    print "Test2: Missing in %s: %s\n-----" % (f.name, missing_rels(rc))


with open("../../data/allen/calculus/contalg") as f:
    for s in (1, 5, 10, 20, 50):
        print "Test3: test on %s (sample size: %s)" % (f.name, s)
        drs = random.sample(rc, s)
        test_rc = [r for r in rc if not r in drs]
        x = close_class_under_cmps(test_rc)
        assert not missing_rels(x)
    print "-----"

with open("../../data/allen/calculus/hornalg") as f:
    rc = [x.strip() for x in f]
    print "Test4: Missing in %s: %s\n-----" % (f.name, missing_rels(rc))


with open("../../data/allen/calculus/hornalg") as f:
    for s in (1, 5, 10, 20, 50, 100):
        print "Test5: test on %s (sample size: %s)" % (f.name, s)
        drs = random.sample(rc, s)
        test_rc = [r for r in rc if not r in drs]
        x = close_class_under_cmps(test_rc)
        assert not missing_rels(x)
    print "-----"

sys.exit()

