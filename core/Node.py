#!/usr/bin/env python

import socket

class Node:
    """Node can maintain a lot of sockets"""
    def __init__(self):
        self.sockets = []

    def create_server_socket(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock = { server_socket : {'type':'server'} }
        self.sockets.append(sock)

    def create_client_socket(self):
        client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock = { client_sock : {'type':'client'} }
        self.sockets.append(sock)

    def sock_bind(self, sock, port):
        assert( self.sockets[sock]['type'] == 'server')
        sock.bind(("", port))

    def sock_listen(self, sock, backlog):
        assert( self.sockets[sock]['type'] == 'server')
        sock.listen(backlog)

    def sock_accept(self, sock):
        assert( self.sockets[sock]['type'] == 'server')
        return sock.accept()

    def sock_recv(self, sock, bufsize):
        return sock.recv(bufsize)

    def sock_send(self, sock, data):
        sock.send(data)

    def create_udp_socket(self):
        client_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock = { client_sock : {'type':'udp'} }
        self.sockets.append(sock)

    # for stateless socket
    def sock_close(self, sock):
        sock.close()


