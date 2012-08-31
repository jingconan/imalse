"""
Definiation of Different Types Random Variable
allow initialization with string, support extra operations
ns3 random variable class cannot be the base class
"""
import ns3
class RandomVariable(object):
    def __str__(self):
        return self.string

class NormalVariable(RandomVariable):
    def __init__(self, s):
        """s is a string e.g. normal(1, 10)"""
        m = float(s.rsplit(',')[0].rsplit('normal(')[1])
        v = float(s.rsplit(',')[1].rsplit(')')[0])
        self.para = [m, v]
        self.string = s

    def __truediv__(self, b):
        """change the mean and variance accordingly"""
        return NormalVariable('normal(%f, %f)'%(self.para[0] / b,  self.para[1] / (b**2)))

    def to_ns3(self):
        return ns3.NormalVariable(self.para[0], self.para[1])


class ExponentialVariable(RandomVariable):
    def __init__(self, s):
        self.para = (float(s.rsplit(')')[0].rsplit('(')[1]),)
        self.string = s

    def __truediv__(self, b):
        return ExponentialVariable('exponential(%f)'%(self.para[0]/b))

    def to_ns3(self):
        return ns3.ExponentialVariable(self.para[0])

class ConstantVariable(RandomVariable):
    def __init__(self, s):
        self.para = (eval(s),)
        self.string = s

    def __truediv__(self, b):
        return ConstantVariable('%f'%(self.para[0]/b))

var_map = {
        'normal': NormalVariable,
        'exponential': ExponentialVariable,
        }

def RV(s):
    """select correct variable according to string"""
    for k, v in var_map.iteritems():
        if k in s: return v(s)

    return ConstantVariable(s)


