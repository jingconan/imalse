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


