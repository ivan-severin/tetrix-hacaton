import socket
import threading

import os, time
import threading, Queue

def get_wine_data(wine_id):
    


class ThreadedServer(threading.Thread):

    def __init__(self, host, port):
        super(ThreadedServer, self).__init__()
        
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((self.host, self.port))
        self.clients = []
        self.stoprequest = threading.Event()
    
    def run(self):
        self.sock.listen(5)
        while not self.stoprequest.isSet():
            client, address = self.sock.accept()
            self.listenToClient(client, address)
            # client.settimeout(60)
            # threading.Thread(target = self.listenToClient, args = (client,address)).start()

    def listenToClient(self, client, address):
        size = 1024
        self.clients.append(client)
        while True:
            try:
                data = client.recv(size)
                if data:
                    # Set the response to echo back the recieved data 
                    print(data)
                    response = data
                    client.send(response)
                else:
                    self.clients.remove(client)
                    raise Exception('Client disconnected')
            except Exception as e :
                print(e)
                client.close()
                return False

    def send_client(self, messege):
        while(len(self.clients) == 0):
            time.sleep(0.5)
        self.clients[-1].send((messege + "\n").encode())



if __name__ == '__main__':
    # thr = WorkerThread('10.1.3.46', 31285)
    server  = ThreadedServer('10.1.3.46', 31285)
    server.start()
    server.send_client("Hi, Mykola!")
    # thr.start()
    # thr.run()
    input()
    # setver.stoprequest.
    # thr.join()
    server.join()