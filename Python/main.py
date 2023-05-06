import sys
import os
import serial
import serial.tools.list_ports
import time
from PyQt5 import uic
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.uic import *
from PyQt5.QtGui import QPainter
import paho.mqtt.client as mqtt


## python실행파일 디렉토리
BASE_DIR = os.path.dirname(os.path.abspath(__file__)) 
Ui_MainWindow, QtBaseClass = uic.loadUiType(BASE_DIR + r'\MainWidget.ui')

class MainWidget(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        uic.loadUi(BASE_DIR + r'\MainWidget.ui', self)

        self.mqttWorker = MqttWorker(self)
        self.mqttWorker.start()

        self.mqttWorker.leftWeightSignal.connect(self.leftLabel.setText)
        self.mqttWorker.rightWeightSignal.connect(self.rightLabel.setText)
        # self.setWindowFlag(Qt.FramelessWindowHint)
        # self.showFullScreen()

class MqttWorker(QThread):
    leftWeightSignal = pyqtSignal(str)
    rightWeightSignal = pyqtSignal(str)

    def __init__(self, parent):
        super().__init__(parent)
        self.broker_address = "138.2.126.137"
        self.client1 = mqtt.Client("client1")
        self.client1.connect(self.broker_address)
        self.client1.subscribe("WEIGHT_230507/LEFT")
        self.client1.subscribe("WEIGHT_230507/RIGHT")
        self.client1.on_message = self.on_message

    def run(self):
        self.client1.loop_forever()

    def on_message(self, client, userdata, message):
        print("message received ", str(message.payload.decode("utf-8")))
        print("message topic= ", message.topic)
        print("message qos=", message.qos)
        print("message retain flag= ", message.retain)
        if message.topic == "WEIGHT_230507/LEFT":
            self.leftWeightSignal.emit(str(message.payload.decode("utf-8")))
        else:
            self.rightWeightSignal.emit(str(message.payload.decode("utf-8")))


if __name__ == '__main__':
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QApplication(sys.argv)
    w = MainWidget()
    w.show()
    # w.showFullScreen()
    sys.exit(app.exec())
