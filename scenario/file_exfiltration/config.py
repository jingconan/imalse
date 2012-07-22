SRV_ADDR = '127.0.0.1'
# SRV_ADDR = '10.0.0.1'
botmaster_desc = {
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        # 'srv_addr':'127.0.0.1',
        'srv_addr':SRV_ADDR,
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
        }

client_desc = {
        'initial' : 'disconn',
        'start_action' : 'request_connect',
        # 'srv_addr':'127.0.0.1',
        'srv_addr':SRV_ADDR,
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
        }

server_desc = {
        'initial' : 'waiting',
        'start_action' : 'initialize',
        # 'srv_addr':'127.0.0.1',
        'srv_addr':SRV_ADDR,
        'srv_port':3333,
        'request_timeout':10,
        'ftp':{
            'host':'thales.bu.edu',
            'user':'imalse-ftp',
            'password':'imalse',
            },
        'file_filter':{
            'suffix':['.txt'],
            'pattern':'assword',
            'directory':'.'
            # 'directory':'/Users/wangjing/Dropbox/GSOC/'
            }
        }




