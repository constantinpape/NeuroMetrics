import numpy as np

from . _NeuroMetrics import *


def metrics(gt, seg):
    gtType  = gt.dtype
    segType = seg.dtype
    assert gtType == segType, "Inputs must have the same data type!"
    assert gt.shape == seg.shape, "Inputs must have the same shape!"
    dim = gt.ndim
    if gtType == np.uint32:
        if dim == 1:
            m = Metrics1dUInt32()
        elif dim == 2:
            m = Metrics2dUInt32()
        elif dim == 3:
            m = Metrics3dUInt32()
        else:
            raise AttributeError("Only up to 3 dimensional input is supported")
    elif gtType == np.uint64:
        if dim == 1:
            m = Metrics1dUInt64()
        elif dim == 2:
            m = Metrics2dUInt64()
        elif dim == 3:
            m = Metrics3dUInt64()
        else:
            raise AttributeError("Only up to 3 dimensional input is supported")
    else:
        raise AttributeError("Datatype %s not supported" % str(gtType))

    m.computeContingencyTable(gt, seg)
    return m
