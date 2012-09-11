#!/usr/bin/env python
import os
from subprocess import call, check_call
def install_pack(packages_info):
    name = packages_info['name']
    url = packages_info.get('url', None)
    path = packages_info.get('path', None)
    print 'installing package %s'%(name)
    if path is not None:
        folder_name = path
    elif url is not None:
        print 'wget %s'%(url)
        call('wget %s'%(url), shell=True)

        print 'tar -xzvf %s'%(url.rsplit('/')[-1])
        tar_file = url.rsplit('/')[-1]
        call('tar -xzvf %s'%(tar_file), shell=True)

        folder_name = tar_file.rsplit('.tar.gz')[0]
    else:
        print 'you should either specify path for url in the packeage info'

    print('cd %s && python setup.py install'%(folder_name))
    try:
        check_call('cd %s && python setup.py install'%(folder_name), shell=True)
    except:
        print 'not python installer, try configure ...'
        try:
            call('cd %s && ./configure'%(folder_name), shell=True)
            call('cd %s && make'%(folder_name), shell=True)
            call('cd %s && sudo make install'%(folder_name), shell=True)
        except:
            print "I don't know how to install this"

def install_debain(packages):
    print 'sudo apt-get install %s'%(' '.join(packages))
    call('sudo apt-get install %s'%(' '.join(packages)), shell=True)


if __name__ == "__main__":
    install_debain(['bison', 'yacc'])
    install_pack( dict(name='softflowd', path='../softflowd-0.9.8-revised-for-ns3-pcap/') )
    install_pack( dict(name='flowd', url='http://flowd.googlecode.com/files/flowd-0.9.1.tar.gz') )

