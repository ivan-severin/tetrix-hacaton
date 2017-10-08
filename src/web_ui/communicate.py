import socket
import threading
from app import data, colomns, index
import os, time
import threading, Queue

users = []
current_user = str()
previous_user = str()
def parse_recived_socket(dat):

    dat =  dat.split(" ")
    user = dat[0]
    wines = [int(x) for x in dat[1:] ]
    return user, wines

def get_wine_data(ids):
    new_dat =[];
    for j in ids:
      d = {} 
      for i in range(1,len(colomns)):
          d.update( {colomns[i]["field"] : data[colomns[i]["field"]][j] })
    #         print(d)
            print(i, j , dat[colomns[i]["field"]][j])
      new_dat.append(d)
    return new_dat



class ThreadedServer(threading.Thread):

    def __init__(self, host, port):
        super(ThreadedServer, self).__init__()
        
        self.host = host
        self.port = port
        self.sock = socket.socket()
        # self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
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
        while not self.stoprequest.isSet():
            try:
                msg= client.recv(size)
                
                

                if msg:
                    # Set the response to echo back the recieved data 
                    print(msg)
                    current_user, dat = parse_recived_socket(msg)
                    # response = data
                    if (current_user not in users):
                        users.append(current_user)
                        print(current_user)
                    else:
                        if (previous_user != current_user):
                            data = get_wine_data(dat)
                            index(current_user)
                    # client.send(response)
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



# if __name__ == '__main__':
#     # thr = WorkerThread('10.1.3.46', 31285)
#     server  = ThreadedServer('10.1.3.46', 31285)
#     server.start()
#     while(True):
#         server.send_client("Hi, Mykola!")
#         time.sleep(2)
#     # thr.start()
#     # thr.run()
#     input()
#     # setver.stoprequest.
#     # thr.join()
#     server.join()