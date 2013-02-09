# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.9
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.



from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_QCNEvent', [dirname(__file__)])
        except ImportError:
            import _QCNEvent
            return _QCNEvent
        if fp is not None:
            try:
                _mod = imp.load_module('_QCNEvent', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _QCNEvent = swig_import_helper()
    del swig_import_helper
else:
    import _QCNEvent
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


class QCNEvent(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, QCNEvent, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, QCNEvent, name)
    __repr__ = _swig_repr
    VERIFICATION_TEST = _QCNEvent.QCNEvent_VERIFICATION_TEST
    DIFFUSION_TEST = _QCNEvent.QCNEvent_DIFFUSION_TEST
    WAVE_CURRENT = _QCNEvent.QCNEvent_WAVE_CURRENT
    DEPTH_INDUCED = _QCNEvent.QCNEvent_DEPTH_INDUCED
    WAVE_GROWTH = _QCNEvent.QCNEvent_WAVE_GROWTH
    def __init__(self): 
        this = _QCNEvent.new_QCNEvent()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _QCNEvent.delete_QCNEvent
    __del__ = lambda self : None;
    __swig_setmethods__["eventid"] = _QCNEvent.QCNEvent_eventid_set
    __swig_getmethods__["eventid"] = _QCNEvent.QCNEvent_eventid_get
    if _newclass:eventid = _swig_property(_QCNEvent.QCNEvent_eventid_get, _QCNEvent.QCNEvent_eventid_set)
    __swig_setmethods__["qcn_quakeid"] = _QCNEvent.QCNEvent_qcn_quakeid_set
    __swig_getmethods__["qcn_quakeid"] = _QCNEvent.QCNEvent_qcn_quakeid_get
    if _newclass:qcn_quakeid = _swig_property(_QCNEvent.QCNEvent_qcn_quakeid_get, _QCNEvent.QCNEvent_qcn_quakeid_set)
    __swig_setmethods__["longitude"] = _QCNEvent.QCNEvent_longitude_set
    __swig_getmethods__["longitude"] = _QCNEvent.QCNEvent_longitude_get
    if _newclass:longitude = _swig_property(_QCNEvent.QCNEvent_longitude_get, _QCNEvent.QCNEvent_longitude_set)
    __swig_setmethods__["latitude"] = _QCNEvent.QCNEvent_latitude_set
    __swig_getmethods__["latitude"] = _QCNEvent.QCNEvent_latitude_get
    if _newclass:latitude = _swig_property(_QCNEvent.QCNEvent_latitude_get, _QCNEvent.QCNEvent_latitude_set)
    __swig_setmethods__["depth"] = _QCNEvent.QCNEvent_depth_set
    __swig_getmethods__["depth"] = _QCNEvent.QCNEvent_depth_get
    if _newclass:depth = _swig_property(_QCNEvent.QCNEvent_depth_get, _QCNEvent.QCNEvent_depth_set)
    __swig_setmethods__["e_time"] = _QCNEvent.QCNEvent_e_time_set
    __swig_getmethods__["e_time"] = _QCNEvent.QCNEvent_e_time_get
    if _newclass:e_time = _swig_property(_QCNEvent.QCNEvent_e_time_get, _QCNEvent.QCNEvent_e_time_set)
    __swig_setmethods__["e_t_now"] = _QCNEvent.QCNEvent_e_t_now_set
    __swig_getmethods__["e_t_now"] = _QCNEvent.QCNEvent_e_t_now_get
    if _newclass:e_t_now = _swig_property(_QCNEvent.QCNEvent_e_t_now_get, _QCNEvent.QCNEvent_e_t_now_set)
    __swig_setmethods__["e_r2"] = _QCNEvent.QCNEvent_e_r2_set
    __swig_getmethods__["e_r2"] = _QCNEvent.QCNEvent_e_r2_get
    if _newclass:e_r2 = _swig_property(_QCNEvent.QCNEvent_e_r2_get, _QCNEvent.QCNEvent_e_r2_set)
    __swig_setmethods__["magnitude"] = _QCNEvent.QCNEvent_magnitude_set
    __swig_getmethods__["magnitude"] = _QCNEvent.QCNEvent_magnitude_get
    if _newclass:magnitude = _swig_property(_QCNEvent.QCNEvent_magnitude_get, _QCNEvent.QCNEvent_magnitude_set)
    __swig_setmethods__["e_std"] = _QCNEvent.QCNEvent_e_std_set
    __swig_getmethods__["e_std"] = _QCNEvent.QCNEvent_e_std_get
    if _newclass:e_std = _swig_property(_QCNEvent.QCNEvent_e_std_get, _QCNEvent.QCNEvent_e_std_set)
    __swig_setmethods__["e_cnt"] = _QCNEvent.QCNEvent_e_cnt_set
    __swig_getmethods__["e_cnt"] = _QCNEvent.QCNEvent_e_cnt_get
    if _newclass:e_cnt = _swig_property(_QCNEvent.QCNEvent_e_cnt_get, _QCNEvent.QCNEvent_e_cnt_set)
    __swig_setmethods__["e_msfit"] = _QCNEvent.QCNEvent_e_msfit_set
    __swig_getmethods__["e_msfit"] = _QCNEvent.QCNEvent_e_msfit_get
    if _newclass:e_msfit = _swig_property(_QCNEvent.QCNEvent_e_msfit_get, _QCNEvent.QCNEvent_e_msfit_set)
    __swig_setmethods__["e_t_detect"] = _QCNEvent.QCNEvent_e_t_detect_set
    __swig_getmethods__["e_t_detect"] = _QCNEvent.QCNEvent_e_t_detect_get
    if _newclass:e_t_detect = _swig_property(_QCNEvent.QCNEvent_e_t_detect_get, _QCNEvent.QCNEvent_e_t_detect_set)
    __swig_setmethods__["e_dt_detect"] = _QCNEvent.QCNEvent_e_dt_detect_set
    __swig_getmethods__["e_dt_detect"] = _QCNEvent.QCNEvent_e_dt_detect_get
    if _newclass:e_dt_detect = _swig_property(_QCNEvent.QCNEvent_e_dt_detect_get, _QCNEvent.QCNEvent_e_dt_detect_set)
    __swig_setmethods__["dirty"] = _QCNEvent.QCNEvent_dirty_set
    __swig_getmethods__["dirty"] = _QCNEvent.QCNEvent_dirty_get
    if _newclass:dirty = _swig_property(_QCNEvent.QCNEvent_dirty_get, _QCNEvent.QCNEvent_dirty_set)
QCNEvent_swigregister = _QCNEvent.QCNEvent_swigregister
QCNEvent_swigregister(QCNEvent)

# This file is compatible with both classic and new-style classes.


