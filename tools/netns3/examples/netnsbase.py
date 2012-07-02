import signal
import atexit
import os
import subprocess
import sys
import vcmd
import tempfile
import shutil

class Netns(object):
    "Interface to a network namespace."

    SHELL = "/bin/bash"
    STATEDIR = "/var/run/netns"

    __netnspids = []

    @classmethod
    def add_netnspid(cls, pid):
        if pid in cls.__netnspids:
            return
        cls.__netnspids.append(pid)

    @classmethod
    def del_netnspid(cls, pid):
        try:
            cls.__netnspids.remove(pid)
        except ValueError:
            pass

    __netnsdirs = []

    class NetnsDir(object):

        def __init__(self, source, persistent = False):
            self.source = source
            self.persistent = persistent

    @classmethod
    def add_netnsdir(cls, nd):
        if nd in cls.__netnsdirs:
            return
        cls.__netnsdirs.append(nd)

    @classmethod
    def del_netnsdir(cls, nd):
        try:
            cls.__netnsdirs.remove(nd)
        except ValueError:
            pass

    @classmethod
    def atexit(cls, signum = signal.SIGTERM):
        while cls.__netnspids:
            pid = cls.__netnspids.pop(0)
            try:
                os.kill(pid, signum)
            except:
                pass
        while cls.__netnsdirs:
            nd = cls.__netnsdirs.pop(0)
            if not nd.persistent:
                shutil.rmtree(nd.source, ignore_errors = True)

    @classmethod
    def spawnserver(cls, name, logfile = None,
                    verbose = True, statedir = STATEDIR):
        if not os.path.isdir(statedir):
            os.makedirs(statedir)
        cmdchnl = os.path.join(statedir, name)
        cmd = ("vnoded", "-c", cmdchnl)
        if logfile:
            cmd += ("-l", logfile)
        if verbose:
            cmd += ("-v",)
        p = subprocess.Popen(cmd, stdout = subprocess.PIPE,
                             stderr = subprocess.PIPE)
        output, erroutput = p.communicate()
        p.stdout.close()
        p.stderr.close()
        sts = p.wait()
        if sts:
            raise ValueError("%s.spawnserver() failed:\n%s" %
                             (cls.__name__, erroutput.strip()))
        pid = int(output)
        cls.add_netnspid(pid)
        return pid, cmdchnl

    def __init__(self, name, logfile = None):
        self.name = name
        self.logfile = logfile
        self.netnsdirs = []
        self.client = None
        self.pid = None
        self.cmdchnl = None
        self.start()

    def __del__(self):
        self.stop()

    def start(self, boot = True):
        self.pid, self.cmdchnl = self.spawnserver(self.name, self.logfile)
        sys.stderr.write("netns %s: pid %s cmdchnl %s logfile %s\n" %
                         (self.name, self.pid, self.cmdchnl, self.logfile))
        self.client = vcmd.VCmd(self.cmdchnl)
        if boot:
            self.boot()

    def closeclient(self):
        if self.client:
            del self.client
            self.client = None

    def detach(self):
        self.closeclient()
        self.del_netnspid(self.pid)
        self.pid = None
        while self.netnsdirs:
            nd = self.netnsdirs.pop(0)
            self.del_netnsdir(nd)

    def stop(self):
        "Completely shutdown the netns."
        self.closeclient()
        if self.pid:
            try:
                os.kill(self.pid, signal.SIGTERM)
                os.waitpid(self.pid, 0)
            except:
                pass
            self.del_netnspid(self.pid)
            self.pid = None
        while self.netnsdirs:
            nd = self.netnsdirs.pop(0)
            self.del_netnsdir(nd)
            if not nd.persistent:
                shutil.rmtree(nd.source, ignore_errors = True)

    def xterm(self, cmd = SHELL):
        #----- revision By J.C.W. -----------
        try:
            user = os.getlogin()
        except:
            user = os.environ.get("USER")
        # cmd = ("su", os.getlogin(), "-c",
        # cmd = ("su", user, "-c",
               # "xterm -ut -T '%s' -e vcmd -c %s -- %s" %
               # (self.name, self.cmdchnl, cmd))
        # use gnome-term instead of xterm
        cmd = ("su", user, "-c",
               "gnome-terminal -t '%s' -e 'vcmd -c %s -- %s'" %
               (self.name, self.cmdchnl, cmd))
        #
        #----- end revision By J.C.W. -----------
        subprocess.check_call(cmd)

    def cmd(self, args):
        """\
        Run the given command with arguments.  Wait for completion and
        return the exit status.
        """
        tmp = self.client.qcmd(args)
        return tmp.wait()

    def spawn(self, args):
        tmp = self.client.qcmd(args)
        return tmp

    def popen(self, args):
        return self.client.popen(args)

    def kill(self, vcmdwait, sig = signal.SIGTERM):
        return self.client.kill(vcmdwait, sig)

    def boot(self):
        """\
        Initialize the netns.

        Bring the loopback interface up and set the host name.
        """
        cmd = ("ip", "link", "set", "lo", "up")
        tmp = self.cmd(cmd)
        if tmp:
            return tmp
        cmd = ("hostname", self.name)
        return self.cmd(cmd)

    def netnsdir(self, target, source = None, persistent = False):
        """\
        Create a per-netns (private) directory mounted at target.  If
        given, the source directory is used; otherwise a temporary
        directory is created.  The directory tree is automatically
        deleted unless persistent is true.
        """
        nd = self.NetnsDir(source, persistent)
        if not nd.source:
            nd.source = tempfile.mkdtemp(prefix = self.name + ".")
            os.chmod(nd.source, 0755)
        cmd = ("mount", "-n", "--bind", nd.source, target)
        tmp = self.cmd(cmd)
        if tmp:
            if not source:
                os.rmdir(nd.source)
            return None
        self.add_netnsdir(nd)
        return nd.source

    def acquire_netif(self, ifname, rename = None):
        """\
        Move the named network interface to this netns's network
        namespace and optionally rename it.
        """
        cmd = ("ip", "link", "set", ifname, "netns", str(self.pid))
        tmp = subprocess.call(cmd)
        if tmp:
            return tmp
        if rename:
            cmd = ("ip", "link", "set", ifname, "name", rename)
            tmp = self.cmd(cmd)
        return tmp

    def ifup(self, ifname):
        "Bring a network interface up."
        cmd = ("ip", "link", "set", ifname, "up")
        return self.cmd(cmd)

    def ifdown(self, ifname):
        "Bring a network interface down."
        cmd = ("ip", "link", "set", ifname, "down")
        return self.cmd(cmd)

    def add_ipaddr(self, ifname, ipaddr):
        """\
        Add an IP address to the given network interface, e.g:

        n.add_ipaddr("eth0", "1.2.3.4/24")
        n.add_ipaddr("eth0", "abcd::1234/64")
        """
        cmd = ("ip", "addr", "add", str(ipaddr), "dev", ifname)
        return self.cmd(cmd)

    def del_ipaddr(self, ifname, ipaddr):
        "Remove an IP address from the given network interface."
        cmd = ("ip", "addr", "del", str(ipaddr), "dev", ifname)
        return self.cmd(cmd)

    def set_macaddr(self, ifname, macaddr):
        "Set the MAC address of a network interface."
        cmd = ("ip", "link", "set", ifname, "address", str(macaddr))
        return self.cmd(cmd)

atexit.register(Netns.atexit)
