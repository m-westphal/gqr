from ctypes import *
from ctypes.util import find_library
import sys

DEBUG = False
_libraries = {}

try:
    # Search for lib in LD_LIBRARY_PATH
    if sys.platform.startswith('darwin'):
        _libgqr_path = 'libgqr.dylib'
    else: # assume linux, not windows
        _libgqr_path = 'libgqr.so'
    # OSError is thrown if the lib is not found
    _gqrdll = CDLL(_libgqr_path)
    if DEBUG: print _gqrdll

except OSError:
    raise
else:
    _libraries['libgqr.so'] = _gqrdll

try:
    if sys.platform == 'darwin':
        _libraries['libc'] = CDLL('libc.dylib')
    else:  # assume linux
        _libraries['libc'] = CDLL('libc.so.6')
except OSError:
    raise

class _Calculus(Structure):
    pass
class _CSP(Structure):
    pass
class _Solver(Structure):
    pass
PCalculus = POINTER(_Calculus)
PCSP = POINTER(_CSP)
PSolver = POINTER(_Solver)

gqr_calculus_new = _libraries['libgqr.so'].gqr_calculus_new
gqr_calculus_new.restype = PCalculus
gqr_calculus_new.argtypes = [c_char_p, c_char_p]
gqr_calculus_unref = _libraries['libgqr.so'].gqr_calculus_unref
gqr_calculus_unref.restype = None
gqr_calculus_unref.argtypes = [PCalculus]
gqr_calculus_get_base_relations = _libraries['libgqr.so'].gqr_calculus_get_base_relations
gqr_calculus_get_base_relations.restype = POINTER(c_char)
gqr_calculus_get_base_relations.argtypes = [PCalculus] 
gqr_calculus_get_composition = _libraries['libgqr.so'].gqr_calculus_get_composition
gqr_calculus_get_composition.restype = POINTER(c_char)
gqr_calculus_get_composition.argtypes = [PCalculus, c_char_p, c_char_p]
gqr_calculus_get_converse = _libraries['libgqr.so'].gqr_calculus_get_converse
gqr_calculus_get_converse.restype = POINTER(c_char)
gqr_calculus_get_converse.argtypes = [PCalculus, c_char_p]

gqr_csp_new = _libraries['libgqr.so'].gqr_csp_new
gqr_csp_new.restype = PCSP
gqr_csp_new.argtypes = [c_int, PCalculus]
gqr_csp_unref = _libraries['libgqr.so'].gqr_csp_unref
gqr_csp_add_constraint = _libraries['libgqr.so'].gqr_csp_add_constraint
gqr_csp_add_constraint.restype = c_bool
gqr_csp_add_constraint.argtypes = [PCSP, c_int, c_int, c_char_p]
gqr_csp_check_equals = _libraries['libgqr.so'].gqr_csp_check_equals
gqr_csp_check_equals.restype = c_bool
gqr_csp_check_equals.argtypes = [PCSP, PCSP]
gqr_csp_check_refines = _libraries['libgqr.so'].gqr_csp_check_refines
gqr_csp_check_refines.restype = c_bool
gqr_csp_check_refines.argtypes = [PCSP, PCSP]
gqr_csp_get_constraint = _libraries['libgqr.so'].gqr_csp_get_constraint
gqr_csp_get_constraint.restype = POINTER(c_char)
gqr_csp_get_constraint.argtypes = [PCSP, c_int, c_int]
gqr_csp_get_name = _libraries['libgqr.so'].gqr_csp_get_name
gqr_csp_get_name.restype = POINTER(c_char)
gqr_csp_get_name.argtypes = [PCSP]
gqr_csp_get_size = _libraries['libgqr.so'].gqr_csp_get_size
gqr_csp_get_size.restype = c_int
gqr_csp_get_size.argtypes = [PCSP]
gqr_csp_set_constraint = _libraries['libgqr.so'].gqr_csp_set_constraint
gqr_csp_set_constraint.restype = c_bool
gqr_csp_set_constraint.argtypes = [PCSP, c_int, c_int, c_char_p]
gqr_csp_set_name = _libraries['libgqr.so'].gqr_csp_set_name
gqr_csp_set_name.restype = None
gqr_csp_set_name.argtypes = [PCSP, c_char_p]

gqr_solver_new = _libraries['libgqr.so'].gqr_solver_new
gqr_solver_new.restype = PSolver
gqr_solver_new.argtypes = [PCalculus]
gqr_solver_unref = _libraries['libgqr.so'].gqr_solver_unref
gqr_solver_unref.restype = None
gqr_solver_unref.argtypes = [PSolver]
gqr_solver_enforce_algebraic_closure = _libraries['libgqr.so'].gqr_solver_enforce_algebraic_closure
gqr_solver_enforce_algebraic_closure.restype = c_bool
gqr_solver_enforce_algebraic_closure.argtypes = [PSolver, PCSP]
gqr_solver_get_scenario = _libraries['libgqr.so'].gqr_solver_get_scenario
gqr_solver_get_scenario.restype = PCSP
gqr_solver_get_scenario.argtypes = [PSolver, PCSP]
gqr_solver_set_tractable_subclass = _libraries['libgqr.so'].gqr_solver_set_tractable_subclass
gqr_solver_set_tractable_subclass.restype = c_bool
gqr_solver_set_tractable_subclass.argtypes = [PSolver, c_char_p]

def _pystring(ptr):
    res = cast(ptr, c_char_p).value
    _libraries['libc'].free(ptr)
    return res

class Calculus(object):
    def __init__(self, name, data_dir):
        '''initialize a new calculus; requires a valid "name".spec in "data_dir" '''
        super(Calculus, self).__init__()
        self._calculus = gqr_calculus_new(name, data_dir)
    def __del__(self):
        gqr_calculus_unref(self._calculus)
    def get_base_relations(self):
        return _pystring(gqr_calculus_get_base_relations(self._calculus))
    def get_composition(self, a_r, b_r):
        return _pystring(gqr_calculus_get_composition(self._calculus, a_r, b_r))
    def get_converse(self, r_raw):
        return _pystring(gqr_calculus_get_converse(self._calculus, r_raw))

class CSP(object):
    def __init__(self, *args):
        super(CSP, self).__init__()
        if len(args) == 1:
            self._csp = args[0]
        else:
            self._csp = gqr_csp_new(args[0], args[1]._calculus)
    def __del__(self):
        gqr_csp_unref(self._csp)
    def add_constraint(self, i, j, relation):
        return gqr_csp_add_constraint(self._csp, i, j, relation)
    def check_equals(self, other):
        return gqr_csp_check_equals(self._csp, other._csp)
    def check_refines(self, other):
        return gqr_csp_check_refines(self._csp, other._csp)
    def get_constraint(self, i, j):
        return _pystring(gqr_csp_get_constraint(self._csp, i, j))
    def get_name(self):
        return _pystring(gqr_csp_get_name(self._csp))
    def get_size(self):
        return gqr_csp_get_size(self._csp)
    def set_constraint(self, i, j, relation):
        return gqr_csp_set_constraint(self._csp, i, j, relation)
    def set_name(self, name):
        return gqr_csp_set_name(self._csp, name)

class Solver(object):
    def __init__(self, calculus):
        super(Solver, self).__init__()
        self._solver = gqr_solver_new(calculus._calculus)
    def __del__(self):
        gqr_solver_unref(self._solver)
    def enforce_algebraic_closure(self, csp_):
        return gqr_solver_enforce_algebraic_closure(self._solver, csp_._csp)
    def get_scenario(self, csp_):
        return CSP(gqr_solver_get_scenario(self._solver, csp_._csp))
    def set_tractable_subclass(self, algFilename):
        return gqr_solver_set_tractable_subclass(self._solver, algFilename)


# initialization
try:
    # Search for gobject lib
    if sys.platform.startswith('darwin'):
        _gobject_path = find_library('libgobject-2.0')
        if not _gobject_path:
            _gobject_path = 'libgobject-2.0.dylib'
    else: # assume linux, not windows
        _gobject_path = find_library('gobject-2.0')
        if not _gobject_path:
            _gobject_path = 'libgobject-2.0.so'
    _gobject = CDLL(_gobject_path)
    if DEBUG: print _gobject
except:
    raise
else:
    _gobject.g_type_init()


__all__ = ["Calculus", "CSP", "Solver"]
