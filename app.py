from flask import Flask, request, render_template, jsonify
import json

app = Flask(__name__)

weightData = {'left_0': 0,
    'left_1': 0,
    'left_2': 0,
    'left_3': 0,
    'left_4': 0,
    'right_0': 0,
    'right_1': 0,
    'right_2': 0,
    'right_3': 0,
    'right_4': 0,
    }

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        data = request.form['data']
        # 데이터를 처리하는 코드
        return jsonify({'result': 'success'})
    else:
        return render_template('index.html')

@app.route('/requestData', methods=['POST'])
def requestData():
    global weightData
    return jsonify(weightData)

@app.route('/update_left', methods=['POST'])
def update_left():
    recvData = json.loads(request.data.decode('utf-8'))
    global weightData
    weightData['left_0'] = recvData['left_0']
    weightData['left_1'] = recvData['left_1']
    weightData['left_2'] = recvData['left_2']
    weightData['left_3'] = recvData['left_3']
    weightData['left_4'] = recvData['left_4']
    return ''

@app.route('/update_right', methods=['POST'])
def update_right():
    recvData = json.loads(request.data.decode('utf-8'))
    global weightData
    weightData['right_0'] = recvData['right_0']
    weightData['right_1'] = recvData['right_1']
    weightData['right_2'] = recvData['right_2']
    weightData['right_3'] = recvData['right_3']
    weightData['right_4'] = recvData['right_4']
    return ''

if __name__ == '__main__':
	app.run(host='0.0.0.0', port=3004)
 