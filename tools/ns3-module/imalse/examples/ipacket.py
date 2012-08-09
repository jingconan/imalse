#!/usr/bin/env python
import ns3
p = ns3.Packet()
h = ns3.ImalseHeader()
h.SetData("connect_ack")

p.AddHeader(h)

print 'hello world'
h2 = ns3.ImalseHeader()

p.RemoveHeader(h2)
print 'data received, ', h2.GetData()

print '-' * 20

p.AddHeader(h)

print 'hello world'
h2 = ns3.ImalseHeader()

p.RemoveHeader(h2)
print 'data received, ', h2.GetData()

print '-' * 20
p.AddHeader(h)

print 'hello world'
h2 = ns3.ImalseHeader()

p.RemoveHeader(h2)
print 'data received, ', h2.GetData()
