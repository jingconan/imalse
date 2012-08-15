botmaster:
	./emulate.py -s ddos_ping_flooding -r botmaster
server:
	./emulate.py -s ddos_ping_flooding -r server
client:
	sudo ./emulate.py -s ddos_ping_flooding -r client 
clean:
	find . -iname "*.pyc" -exec rm '{}' ';'



