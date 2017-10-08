from flask import Flask, render_template, request
import json
import communicate
import pandas as pd

"""
A example for creating a Table that is sortable by its header
"""

app = Flask(__name__)

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

  colomns[10], colomns[2] = colomns[2], colomns[10]
  colomns[9], colomns[1] = colomns[1], colomns[9]

  # colomns[0].update()

  d =   {
        "field": "Check", # which is the field's name of data key 
        "title": "Check", # display as the table header's name
        "sortable": False,
        "checkbox" : True
        }
  colomns[0]["sortable"] = True
  # colomns[0]["field"] = "ID"
  colomns.insert(0, d)
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
# def init_data():
data, colomns = read_data("../../data/wine.csv")



# print(new_dat)

# other column settings -> http://bootstrap-table.wenzhixin.net.cn/documentation/#column-options

#jdata=json.dumps(data)

@app.route('/' )
def index():

	# data = request.data
	# print(data,"\n\n")
	return render_template("table.html",
      data=data,
      columns=colomns,
      title='Dionysus store')

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
	#print jdata
  app.run(debug=True)