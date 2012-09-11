#!/usr/bin/env sh
python install_pack.py

sudo mkdir -p /var/empty/flowd
sudo groupadd _flowd
sudo useradd -g _flowd -c "flowd privsep" -d /var/empty/flowd _flowd
