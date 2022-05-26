#!/usr/local/bin/python3.8
import socket
import argparse
import sys
import os
import time
import re

def downloadFile(server, path, host, mode="file"):
    # Socket creation
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(fileserver) # connection
    # request format
    req = f"GET {path} FSP/1.0\r\nHostname: {host}\r\nAgent: xhlava52\r\n\r\n"
    try:
        sock.sendall(bytes(req,encoding="utf-8"))
    except Exception as e:
        sys.stderr.write(f"error while sending request to fileserver: {str(e)}")
        exit(1)
    # receive header, response is already split into header and data
    header = sock.recv(128).decode().split("\r\n")
    status = header[0][8:] # status of response
    datalen = int(header[1].split(":")[1]) # extract data length from header
    databuf = bytes()
    if status not in ["Success", "Bad Request", "Not Found", "Server Error"]:
        sys.stderr.write(f"Error: unexpected server response: {status}")
        exit(1)
    if status != "Success":
        sys.stderr.write(f"{status} error received from fileserver\n")
        moreinfo = bytes()
        remaining = datalen
        while remaining > 0: # receiving parts of data and storing bytes into buffer
            recvbuf = sock.recv(1024)
            remaining -= len(recvbuf) # subtracting only received bytes, because it may be less than 1024
            moreinfo += recvbuf # we have to receive error details
        sys.stderr.write(f"More info: {moreinfo.decode()}\n")
        exit(1)
    else: 
        remaining = datalen
        while remaining > 0: # receiving parts of data and storing bytes into buffer
            recvbuf = sock.recv(1024)
            remaining -= len(recvbuf) # subtracting only received bytes, because it may be less than 1024
            databuf += recvbuf
        if mode == "file": # Stores received bytes into file 
            pathsubs = path.split("/")
            f = open(os.path.join(os.getcwd(),pathsubs[-1]), "wb")
            f.write(databuf)
            f.close()
        elif mode == "fileall":
            pathsubs = path.split("/")
            if len(pathsubs) > 1:
                os.makedirs(os.path.join(os.getcwd(),"/".join(pathsubs[:-1])), exist_ok=True)
            f = open(os.path.join(os.getcwd(),path), "wb")
            f.write(databuf)
            f.close()
        elif mode == "console": # debug feature for printing received file as UTF-8 string
            print(databuf.decode())
        elif mode == "return": # returns bytes for further use (e.g. processing 'index' file)
            return databuf

# Format regexes
re_ip = re.compile(r'([0-9]{1,3}\.){3}[0-9]{1,3}:[0-9]{1,5}')
re_address = re.compile(r'fsp:\/\/[A-Za-z0-9\.\-\_]+\/.*')

# Parse command-line arguments
argparser = argparse.ArgumentParser()
argparser.add_argument("-n", type=str, help="address of nameserver")
argparser.add_argument("-f", type=str, help="SURL of file")

args = argparser.parse_args()

if args.n is None or args.f is None:
    sys.stderr.write("error: wrong arguments\nUsage: fileget -n <nameserver address> -f <file address>")
    exit(1)

# check IP format
if re_ip.fullmatch(args.n) is None:
    sys.stderr.write("error: wrong format of nameserver IP address\n")
    exit(1)
nameserver = args.n.split(":")
nameserver[1] = int(nameserver[1])
if nameserver[1] > 65535: 
    sys.stderr.write("error: wrong port number")
    exit(1)
nameserver = tuple(nameserver)

# Check file server address format
if re_address.fullmatch(args.f) is None:
    sys.stderr.write("error: wrong format of file address\n")
    exit(1)
pathparts = args.f[6:].split("/")
url = pathparts[0]
path = "/".join(pathparts[1:])

nssock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP socket for nameserver

nssock.setblocking(True)
nssock.settimeout(3)

# Retrieve IP adress and port of fileserver
try:
    nssock.sendto(bytes(f"WHEREIS {url}", encoding="utf-8"),nameserver)
except Exception as e:
    sys.stderr.write(f"error while sending request to nameserver: {str(e)}")
    exit(1)
resp = None
# Receive header
try:
    resp = nssock.recv(256)
except socket.timeout:
    sys.stderr.write("Error: nameserver didn't respond in time!\n")
    exit(1)

address = str(resp)[5:-1] # cleanup of b'OK '

# Check for error messages
if resp == b"ERR Syntax":
    sys.stderr.write("Error: syntax error received from nameserver\n")
    exit(1)
elif resp == b"ERR Not Found":
    sys.stderr.write(f"Error: server {url} not found\n")
    exit(1)
elif resp[:2] != b"OK": # Check for bad header format
    sys.stderr.write("Error: nameserver responded with invalid response\n")
    exit(1)

# Check if nameserver returned valid IP address
if re_ip.fullmatch(address) is None:
    sys.stderr.write("error: wrong format of IP address got from nameserver\n")
    exit(1)

fileserver = address.split(":")
fileserver[1] = int(fileserver[1])
fileserver = tuple(fileserver)

downloadfiles = []

mode = "file" # mode for downloadFile function
if path[-1] == "*": # download whole server wildcard
    mode = "fileall"
    index = downloadFile(fileserver, "index", url, mode="return").decode()
    downloadfiles = index.split("\r\n")[:-1]
else:
    downloadfiles.append(path)

for f in downloadfiles:
    downloadFile(fileserver, f, url, mode)
