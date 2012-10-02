""" Definiation of Different Types Random Variable

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



##########################################
##        Random Generator            ####
##########################################

import random
import math
def randomunifint(lo, hi):
    while True:
        yield random.randint(lo, hi)

def randomuniffloat(lo, hi):
    while True:
        yield random.random()*(hi-lo)+lo

def randomchoice(*choices):
    while True:
        yield random.choice(choices)

def randomchoicefile(infilename):
    xlist = []
    with open(infilename) as inf:
        for line in inf:
            for value in line.strip().split():
                try:
                    xlist.append(float(value))
                except:
                    pass
    index = 0
    while True:
        yield xlist[index]
        index = (index + 1) % len(xlist)

def pareto(offset,alpha):
    while True:
        # yield offset*random.paretovariate(alpha)
        yield (offset * ((1.0/math.pow(random.random(), 1.0/alpha)) - 1.0));

#def mypareto(scale, shape):
#    return (scale * ((1.0/math.pow(random.random(), 1.0/shape)) - 1.0));

def exponential(lam):
    while True:
        yield random.expovariate(lam)

def normal(mean, sdev):
    while True:
        yield random.normalvariate(mean, sdev)

def lognormal(mean, sdev):
    while True:
        yield random.lognormvariate(mean, sdev)

def gamma(alpha, beta):
    while True:
        yield random.gammavariate(alpha, beta)

def weibull(alpha, beta):
    while True:
        yield random.weibullvariate(alpha, beta)

def mkdict(s):
    xdict = {}
    if isinstance(s, str):
        s = s.split()
    for kvstr in s:
        k,v = kvstr.split('=')
        xdict[k] = v
    return xdict



