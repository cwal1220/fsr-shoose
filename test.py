import requests
import json
import urllib
from urllib.parse import urljoin
import time

class db_requester():
	def __init__(self, host):
		self.__host = host

	def request(self, path, param):
		url = urllib.parse.urljoin(self.__host, path)
		r = requests.post(url, json=param)
		return json.loads(r.text)
	
if __name__ == '__main__':
    # db = db_requester('http://138.2.126.137:3004')
    db = db_requester('http://127.0.0.1:3004')
    for i in range(9000):
        print(db.request('/update', {'left_0':i, 'left_1':i, 'left_2':i, 'left_3':i, 'left_4':i, 
                                    'right_0':i, 'right_1':i, 'right_2':i, 'right_3':i, 'right_4':i }))
        time.sleep(0.1)