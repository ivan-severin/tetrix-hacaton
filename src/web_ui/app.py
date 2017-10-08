from flask import Flask, render_template, request
import json
# from  communicate import *
import pandas as pd

import socket
import threading
# from app import data, colomns, index
import os, time
import threading

import random





"""
A example for creating a Table that is sortable by its header
"""

app = Flask(__name__)


data = []
colomns =[]

users = []
current_user =  ''
previous_user = '' 

new_data = []


def parse_recived_socket(dat):

    dat =  dat.split(" ")
    user = dat[0]

    wines = [int(x) for x in dat[1:] ]
    return user, wines

def get_wine_data(ids , data, colomns):
    new_dat =[]    
    # ids = [ str(x) for x in ids]
    print (ids)
    print (colomns)
    for j in range(1, len(ids)):
        d = {} 
        for i in range(1,len(colomns)):
            key = colomns[i]["field"]
            print(key)
            d.update( { key : data[key][j] } )
            
    #         print(j, i)
            print(d)
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
        # self.clients = []
        self.stoprequest = threading.Event()
    
    def run(self):
        self.sock.listen(1)
        while not self.stoprequest.isSet():
            client, address = self.sock.accept()
            self.listenToClient(client, address)
            # client.settimeout(60)
            # threading.Thread(target = self.listenToClient, args = (client,address)).start()

    def listenToClient(self, client, address):
        size = 1024
        # self.clients.append(client)
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
                        current_user
                        dat
                    else:
                        if (previous_user != current_user):
                           
                            data
                            # index(current_user)
                            # render_template("table.html",
                            #   data=data,
                            #   columns=colomns,
                            #   title='Dionysus for ' + current_user)
                    # client.send(response)
                else:
                    self.clients.remove(client)
                    raise Exception('Client disconnected')
            except Exception as e :
                raise e
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

def read_data(file):
  data = pd.read_csv(file, delimiter=',')
  # data = pd.read_csv("../../data/wine.csv", delimiter=",")
  data = data.head(200)
  colomns = []
  for i in data[:]:
      d =   {
              "field": i, # which is the field's name of data key 
              "title": i, # display as the table header's name
              "sortable": True,
              }
      colomns.append(d)

  # colomns[10], colomns[2] = colomns[2], colomns[10]
  # colomns[9], colomns[1] = colomns[1], colomns[9]

  # colomns[0].update()

  d =   {
        "field": "Check", # which is the field's name of data key 
        "title": "Check", # display as the table header's name
        "sortable": False,
        "checkbox" : True
        }
  colomns[0]["sortable"] = False
  colomns[0]["checkbox"] = True
  # colomns[0]["field"] = "ID"
  # colomns.insert(0, d)
  new_dat =[];

  for j in range(len(data)):
      d = {} 
      for i in range(1,len(colomns)):
          d.update( {colomns[i]["field"] : data[colomns[i]["field"]][j] })
  #         print(d)
  #         print(i, j , dat[colomns[i]["field"]][j])
      new_dat.append(d)

  return (new_dat, colomns)

# @app.before_request
# def init_data ():
data, colomns = read_data("../../data/wine.csv")
print("reading data")
# @app.after_request
# def init_server():
#     #print jdata




# print(new_dat)

# other column settings -> http://bootstrap-table.wenzhixin.net.cn/documentation/#column-options

#jdata=json.dumps(data)

@app.route('/' )
def index(user = ''):
  dat  = random.randrange(1, 50)
  # print(dat)
  # new_data = data[:dat]
  new_data = get_wine_data(ids = range(1, dat), data=data, colomns=colomns)
  return render_template("table.html",
      data=new_data,
      columns=colomns,
      title='Dionysus  Jhon')

@app.route('/checked', methods=['GET', 'POST'] )
def checked():

	response = request.get_data()
	print(response)
	jdata = json.loads(response)
	
	for d in jdata:
		for key, value in d.iteritems():
			print key, value
  # send_to_user()
	return json.dumps(jdata)
	
if __name__ == '__main__':

  server  = ThreadedServer('10.1.3.46', 31285)
  # server.start()
  print (" * Starting Socket")

  # time.sleep(20)
  # server.join()
  app.run(port=5001,debug=False, host='0.0.0.0')

