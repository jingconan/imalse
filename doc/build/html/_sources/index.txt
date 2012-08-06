.. imalse documentation master file, created by
   sphinx-quickstart on Thu Aug  2 18:21:28 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to IMALSE's documentation!
==================================
Introduction
------------------------------
IMALSE (**I**\ ntegrated   **MAL**\ ware **S**\ imulator and **E**\ mulator)
is a framework to help researchers to implement prototype of botnet based
network malware. Researchers just need to implement the malware behaviour once
and then it can run the following three modes:

    - **emulation mode**: In this mode, each copy of imalse will behave exactly like a 
      real malware. You can install it in a real machine, or in a virtual machine and
      set up a testbed to test the characteristic of the malware.(Don't use it
      to attack other people's machines;) ) [**Note**: you can
      potentially work with Common Open Research Emulator to emulate a lot of nodes in
      one machine]
    - **netns3 simulation mode**:  You can specifiy the topology of the network and the ip addresses of 
      each node in this mode. IMALSE will launch virtual machines (linux namespace) for each 
      node in the network and construct  the network automatically. All virtualized nodes 
      will connect to **NS3** through tapbridge and all traffic will consume
      there. The simulation will be in real time. It is based on `netns3
      <http://www.nsnam.org/wiki/index.php/HOWTO_use_Linux_namespaces_with_ns-3>`_
      project.
    - **pure ns3 simulation mode**: No virtual machince will be launched for the
      pure ns3 simulation mode, the whole simulation will be done in `ns3
      <http://www.nsnam.org/>`_. ns3
      default scheduler will be used instead of the real time scheduler in
      netns3 case, which saves much time. One simulation day may only consume
      several real seconds.
    - **hybrid approach**: of pure ns3 mode and netns3 mode

.. image:: ./figure/imalse-abstract.png
    :align: center 

..  Advantage:
..  ------------------------------
..  good

Typical Use Case
++++++++++++++++++++++++++++++

Suppose Conan is a Ph.D student who has proposed a novel anomaly
detection technique for Internet traffic. He wants to demostrate the usefulness of this approach. To
do this, he designed a scenario that 100 client computers accessing a server through the internet, 10 of
which had already been compromised and controlled by botmaster through botnet. At some point, the botmaster
will initiate a ddos attack by asking all compromised computers to send ping
requests to the servers. The anomaly detection technique requires all the
incoming and outcoming traffic of the server for at least two days. 

How can he collect the data he want? imalse provides different solutions at
different abstract level. He decide to use **ImalseTopoSimExperiment** and select **ddos_ping_attack** 
attacking scenario from the imalse software which provide exactly what he wants.

The first question is since the method is not mature, Conan want to test it
under different parameter combinations. It will be forever if each simulation
takes more than two days. Fortunately, by running the simulation under **pure ns3 simulation mode**
Conan can finish one simulation with less 100 real seconds, though the time
has past for more than two days in the simulator.

After extensive testing, Conan has been quite confident about the performance of
the anomaly detection techinique now. But he is still a little bit worried about
whether the result of ns3 is convincing enough. As a result, he run a complete
simulation under **netns3 simulation model** and collect data. Of course, this
time it runs more than two days, but he doesn't care that much because he only
need to run it for very few times. Conan generate some plots and write a paper
with data of **netns3 simulation model** and satisfied with this.

A rich company named NetSecurity read this paper and think it is a good method. They want to 
deploy it but need more realistic test before deployment, so they decide
to test it under their intranet. They ask Conan for a copy of the code and
select several computer in the intranet to join the botnet, each computer run an independent copy of
imalse under **emulation client mode**, there is a computer serving as botmster
and running a imalse under **emulation server model**\ (the server refers to the C&C
server in the botnet). The data of attacked server is recorded and analyzed with
Conan's tools. It turns out to be good, and the Company decide to use this
method.

As a lazy Ph.D student, Conan just need to write one copy of code to describe
the secnario during the whole process. With the help of imalse, he can have more
time to sleep and enjoy the classical music. :)


Description:
------------------------------

..  Imalse has well designed structure to support 
To support the variety of modes noted above, Imalse design in a way that to
seperate the botnet mechanism and the network.

The general structure of Imalse is shown in the following figure:

.. image:: ./figure/genearal-structure.png
    :align: center 

**Node** and **C**\ ommand **M**\ eta **D**\ escription are the two key concepts in the design.
**Node** is the abstraction of a real computer. A node should support:

    - **Basic Utility Functions**: serveral basic utility calls, including
      getting the system time, make node sleep, so on so forth
    - **Socket API** this is require to implement basic
    - **File System**
    - **Higher Level Application** 

There is a abstract base class in core module named **BaseNode** that define the
all APIs a node need to overload. If you want to extend the framework to support
other type of simulator, please subclass **BaseNode** and implement all the
virtual functions.

a command is a basic event in the botnet. **C**\ ommand **M**\ eta **D**\ escription defines a set of
commands for a node, namely it defines what event a node can generate. There are three types of **CMD**\ s:
    1. Server **CMD**
    2. Client **CMD**
    3. Botmaster **CMD**

The basic information flow is, botmaster send commands to server, server will
translate the commands and send commands to Clients.

Basic Botnet Mechanism:
++++++++++++++++++++++++++++++

In **core** module, imalse provides a basic framework for the botnet and the
support to real network, netns3 and NS3. User can create their own flavor of
botnet by subclassing **ServerCMD**, **ClientCMD** and **BotmasterCMD**, each
flavor is called a new **scenario**.

The basic botnet mechanism can be described as **F**\ inite **S**\ tate **M**\
achine. The **FSM** for the **ClientCMD** is as follows:

.. image:: ./figure/client_fsm.svg
    :align: center 
    :height: 400px

Scenario:
++++++++++++++++++++++++++++++

As a noted above, user can create their own flavor of **Botnet**, which is so
called **scenario**. Currently, Imalse provides two sample **scenarios**: 

    1. **ddos_ping_flooding**: in this scenario, botmaster can issue **send_pings** 
           command to initiate a ddos ping flooding attack to a specific server.
    2. **file_exfiltration**: in this scenario, botmaster can request bots to search
           in the file system with any file that contains a certain pattern,
           like **password**. Whenver an interesting is found, the bot will
           upload the file to a ftp server.

Experiment
++++++++++++++++++++++++++++++

Experiment is only used in **simulation mode**. Experiments need to do Topology and network
configuration, user behaviour specification. This is usually the only part user need to code if he is 
using an existing scenario. 

Two types of experiments are avaliable in Imalse. 1. **ImalseNetnsExperiment** and 2. **ImalsePureSimExperiment**. 
**ImalseNetnsExperiment** is for **netns3 mode** simulation. **ImalsePureSimExperiment** is
for pure simulation. For both experiments, you can load topology. If you want to implement your own experiment, 
you can subclass corresponding classes to implement different **mode**.

The experiments folder contains several examples of experiment
    1. **StaticRouteExperiment**
    2. **TopoExperiment.py**

Demo:
------------------------------
this is the link for a demo
`video <http://www.youtube.com/watch?v=CZ91McFlIvo&feature=plcp>`_




Extension:
------------------------------

To implement a new scenario, you need to create a new folder in scenario
folder. This folder actually is a new module in python. This module should
provide the following classes: **BotMaster**, **ClientCMD**, **ServerCMD**.

You can subclass the BotMaster ClientCMD and ServerCMD in core, which provide a
botnet communication scheme and make the difference of real node, netns3 node
and ns3 sim node to be transparent. 

.. toctree::
   :maxdepth: 3

Contents:

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

