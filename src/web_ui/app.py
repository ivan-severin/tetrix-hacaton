from flask import Flask, render_template, request
import json
import pandas as pd

"""
A example for creating a Table that is sortable by its header
"""

app = Flask(__name__)

# # The path where you want the extension to create the needed javascript files
# # DON'T put any of your files in this directory, because they'll be deleted!
# app.config["SIJAX_STATIC_PATH"] = os.path.join('.', os.path.dirname(__file__), 'static/js/sijax/')

# # You need to point Sijax to the json2.js library if you want to support
# # browsers that don't support JSON natively (like IE <= 7)
# app.config["SIJAX_JSON_URI"] = '/static/js/sijax/json2.js'


data = pd.read_csv("../../data/wine.csv", delimiter=",")

data = data.head(200)
colomns = []
for i in data[:]:
    d =   {
            "field": i, # which is the field's name of data key 
            "title": i, # display as the table header's name
            "sortable": True,
            }
    colomns.append(d)
colomns[0].update({"checkbox" : True})
colomns[0]["sortable"] = False

new_dat =[];
for j in range(len(data)):
    d = {} 
    for i in range(len(colomns)):
        d.update( {colomns[i]["field"] : data[colomns[i]["field"]][j] })
#         print(d)
#         print(i, j , dat[colomns[i]["field"]][j])
    new_dat.append(d)
# print(new_dat)

# other column settings -> http://bootstrap-table.wenzhixin.net.cn/documentation/#column-options

#jdata=json.dumps(data)

@app.route('/' )
def index():

	# data = request.data
	# print(data,"\n\n")
	return render_template("table.html",
      data=new_dat[:],
      columns=colomns,
      title='Flask Bootstrap Table')

@app.route('/checked', methods=['GET', 'POST'] )
def checked():

	response = request.get_data()
	print(response)
	jdata = json.loads(response)
	
	for d in jdata:
		for key, value in d.iteritems():
			print key, value
	return json.dumps(jdata)
	
if __name__ == '__main__':
	#print jdata
  app.run(debug=True)