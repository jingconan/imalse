
NSNAM_CODE_BASE_URL = "http://code.nsnam.org/"
PYBINDGEN_BRANCH = 'https://launchpad.net/pybindgen'
LOCAL_PYBINDGEN_PATH = 'pybindgen'

#
# The directory in which the tarball of the reference traces lives.  This is
# used if Mercurial is not on the system.
#
REGRESSION_TRACES_URL = "http://www.nsnam.org/releases/"

#
# The path to the Mercurial repository used to find the reference traces if
# we find "hg" on the system.  We expect that the repository will be named
# identically to refDirName below
#
REGRESSION_TRACES_REPO = "http://code.nsnam.org/"

#
# Name of the local directory where the regression code lives.
#
REGRESSION_DIR = "regression"

#
# The last part of the path name to use to find the regression traces.  The
# path will be APPNAME + '-' + VERSION + REGRESSION_SUFFIX, e.g.,
# ns-3-dev-ref-traces
#
REGRESSION_SUFFIX = "-ref-traces"

#
# The last part of the path name to use to find the regression traces tarball.
# path will be APPNAME + '-' + VERSION + REGRESSION_SUFFIX + TRACEBALL_SUFFIX,
# e.g., ns-3-dev-ref-traces.tar.bz2
#
TRACEBALL_SUFFIX = ".tar.bz2"

# network simulation cradle
NSC_REPO = "https://secure.wand.net.nz/mercurial/nsc"
NSC_RELEASE_URL = "http://research.wand.net.nz/software/nsc"

LOCAL_NSC_PATH = "nsc"

# NetAnim
NETANIM_REPO = "http://code.nsnam.org/netanim"
NETANIM_RELEASE_URL = "http://www.nsnam.org/tools/netanim"

LOCAL_NETANIM_PATH = "netanim"
