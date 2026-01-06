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
        

        self.b1 = QtWidgets.QPushButton(self)
        self.b1.setText("Lock Status")
        self.b1.clicked.connect(self.clicked)
        self.b1.move(50,100)

    def clicked(self):
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
    

