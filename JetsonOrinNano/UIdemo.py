from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow
import sys

class windowUI(QMainWindow):
    def __init__(self):
        super(windowUI, self).__init__()
        self.setGeometry(200,200,1000,1000) #Xpos, Ypos, Width, Height
        self.setWindowTitle("Jetson Orin Nano UI Demo")
        self.initUI()

    def initUI(self):
        self.label= QtWidgets.QLabel(self)
        self.label.setText("Jetson Orin Nano UI Demo")
        self.label.move(50,50)

        self.notif= QtWidgets.QLabel(self)
        self.notif.setText("Status: No Latest Action")
        self.notif.adjustSize();
        
        #Unlock lock Button
        self.b1 = QtWidgets.QPushButton(self)
        self.b1.setText("Unlock Lock")
        self.b1.clicked.connect(self.lockClicked)
        self.b1.move(50,100)

        #Check Lock Status Button
        self.lockStatusLabel = QtWidgets.QLabel(self)

        self.lockStatusLabel.setText("Lock Status")
        self.lockStatusLabel.move(50,150)

        #load cell A (Right) display
        self.loadRightlabel= QtWidgets.QLabel(self)
        self.loadRightlabel.setText("Load Cell A (Right) Value: Not Sensed")
        self.loadRightlabel.move(50,200)

        #Load cell B (Left) display
        self.loadLeftlabel= QtWidgets.QLabel(self)
        self.loadLeftlabel.setText("Load Cell B (Left) Value: Not Sensed")
        self.loadLeftlabel.move(50,250)

    def lockClicked(self):
        self.notif.setText("Status: Lock Status Button Pressed")
        self.update()
        


    def update(self):
        self.notif.adjustSize()

def window():
    app = QApplication(sys.argv)
    win = windowUI()
    
    

  

    win.show() #end of window display
    sys.exit(app.exec_())

def lockStatus():
    print("Lock Status Button Pressed")
    

