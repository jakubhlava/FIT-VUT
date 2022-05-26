# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'alphacalc.ui'
##
## Created by: Qt User Interface Compiler version 5.14.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import (QCoreApplication, QDate, QDateTime, QMetaObject,
    QObject, QPoint, QRect, QSize, QTime, QUrl, Qt)
from PySide2.QtGui import (QBrush, QColor, QConicalGradient, QCursor, QFont,
    QFontDatabase, QIcon, QKeySequence, QLinearGradient, QPalette, QPainter,
    QPixmap, QRadialGradient)
from PySide2.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(421, 600)
        palette = QPalette()
        brush = QBrush(QColor(33, 33, 33, 255))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Button, brush)
        palette.setBrush(QPalette.Active, QPalette.Base, brush)
        palette.setBrush(QPalette.Active, QPalette.Window, brush)
        brush1 = QBrush(QColor(30, 30, 30, 255))
        brush1.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.ToolTipBase, brush1)
        palette.setBrush(QPalette.Inactive, QPalette.Button, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Base, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Window, brush)
        palette.setBrush(QPalette.Inactive, QPalette.ToolTipBase, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Button, brush)
        palette.setBrush(QPalette.Disabled, QPalette.Base, brush)
        palette.setBrush(QPalette.Disabled, QPalette.Window, brush)
        palette.setBrush(QPalette.Disabled, QPalette.ToolTipBase, brush1)
        MainWindow.setPalette(palette)
        MainWindow.setWindowOpacity(1.000000000000000)
        MainWindow.setAutoFillBackground(False)
        MainWindow.setStyleSheet(u"QMainWindow {\n"
"  background-color: rgb(33, 33, 33);\n"
"}\n"
"\n"
"")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.Button_sin = QPushButton(self.centralwidget)
        self.Button_sin.setObjectName(u"Button_sin")
        self.Button_sin.setGeometry(QRect(0, 250, 71, 71))
        font = QFont()
        font.setFamily(u"Tahoma")
        font.setPointSize(15)
        self.Button_sin.setFont(font)
        self.Button_sin.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.display = QLabel(self.centralwidget)
        self.display.setObjectName(u"display")
        self.display.setGeometry(QRect(0, 30, 421, 101))
        font1 = QFont()
        font1.setFamily(u"Roboto Mono")
        font1.setPointSize(30)
        self.display.setFont(font1)
        self.display.setAutoFillBackground(False)
        self.display.setStyleSheet(u"QLabel {\n"
"  background-color: rgb(35, 35, 35);\n"
"  color: white; \n"
"  qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"  border: 1px solid 	#232323;\n"
" font-family: Roboto Mono;\n"
"}\n"
"")
        self.Button_cos = QPushButton(self.centralwidget)
        self.Button_cos.setObjectName(u"Button_cos")
        self.Button_cos.setGeometry(QRect(70, 250, 71, 71))
        self.Button_cos.setFont(font)
        self.Button_cos.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_percent = QPushButton(self.centralwidget)
        self.Button_percent.setObjectName(u"Button_percent")
        self.Button_percent.setGeometry(QRect(280, 250, 71, 71))
        self.Button_percent.setFont(font)
        self.Button_percent.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_tan = QPushButton(self.centralwidget)
        self.Button_tan.setObjectName(u"Button_tan")
        self.Button_tan.setGeometry(QRect(140, 250, 71, 71))
        self.Button_tan.setFont(font)
        self.Button_tan.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_9 = QPushButton(self.centralwidget)
        self.Button_9.setObjectName(u"Button_9")
        self.Button_9.setGeometry(QRect(140, 320, 71, 71))
        self.Button_9.setFont(font)
        self.Button_9.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_8 = QPushButton(self.centralwidget)
        self.Button_8.setObjectName(u"Button_8")
        self.Button_8.setGeometry(QRect(70, 320, 71, 71))
        self.Button_8.setFont(font)
        self.Button_8.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_sqrt = QPushButton(self.centralwidget)
        self.Button_sqrt.setObjectName(u"Button_sqrt")
        self.Button_sqrt.setGeometry(QRect(280, 320, 71, 71))
        self.Button_sqrt.setFont(font)
        self.Button_sqrt.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_7 = QPushButton(self.centralwidget)
        self.Button_7.setObjectName(u"Button_7")
        self.Button_7.setGeometry(QRect(0, 320, 71, 71))
        self.Button_7.setFont(font)
        self.Button_7.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_6 = QPushButton(self.centralwidget)
        self.Button_6.setObjectName(u"Button_6")
        self.Button_6.setGeometry(QRect(140, 390, 71, 71))
        self.Button_6.setFont(font)
        self.Button_6.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_5 = QPushButton(self.centralwidget)
        self.Button_5.setObjectName(u"Button_5")
        self.Button_5.setGeometry(QRect(70, 390, 71, 71))
        self.Button_5.setFont(font)
        self.Button_5.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_pow = QPushButton(self.centralwidget)
        self.Button_pow.setObjectName(u"Button_pow")
        self.Button_pow.setGeometry(QRect(210, 320, 71, 71))
        self.Button_pow.setFont(font)
        self.Button_pow.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_pow.setFlat(False)
        self.Button_4 = QPushButton(self.centralwidget)
        self.Button_4.setObjectName(u"Button_4")
        self.Button_4.setGeometry(QRect(0, 390, 71, 71))
        self.Button_4.setFont(font)
        self.Button_4.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_3 = QPushButton(self.centralwidget)
        self.Button_3.setObjectName(u"Button_3")
        self.Button_3.setGeometry(QRect(140, 460, 71, 71))
        self.Button_3.setFont(font)
        self.Button_3.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_2 = QPushButton(self.centralwidget)
        self.Button_2.setObjectName(u"Button_2")
        self.Button_2.setGeometry(QRect(70, 460, 71, 71))
        self.Button_2.setFont(font)
        self.Button_2.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_1 = QPushButton(self.centralwidget)
        self.Button_1.setObjectName(u"Button_1")
        self.Button_1.setGeometry(QRect(0, 460, 71, 71))
        self.Button_1.setFont(font)
        self.Button_1.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_dot = QPushButton(self.centralwidget)
        self.Button_dot.setObjectName(u"Button_dot")
        self.Button_dot.setGeometry(QRect(140, 530, 71, 71))
        self.Button_dot.setFont(font)
        self.Button_dot.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_equals = QPushButton(self.centralwidget)
        self.Button_equals.setObjectName(u"Button_equals")
        self.Button_equals.setGeometry(QRect(210, 530, 141, 71))
        self.Button_equals.setFont(font)
        self.Button_equals.setLayoutDirection(Qt.LeftToRight)
        self.Button_equals.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: #FF8000; \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #FF8000, stop: 1 #FF8000);\n"
"	color: rgb(0, 0, 0); \n"
"}")
        self.Button_0 = QPushButton(self.centralwidget)
        self.Button_0.setObjectName(u"Button_0")
        self.Button_0.setGeometry(QRect(0, 530, 71, 71))
        self.Button_0.setFont(font)
        self.Button_0.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_add = QPushButton(self.centralwidget)
        self.Button_add.setObjectName(u"Button_add")
        self.Button_add.setGeometry(QRect(210, 460, 71, 71))
        self.Button_add.setFont(font)
        self.Button_add.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_mul = QPushButton(self.centralwidget)
        self.Button_mul.setObjectName(u"Button_mul")
        self.Button_mul.setGeometry(QRect(210, 390, 71, 71))
        self.Button_mul.setFont(font)
        self.Button_mul.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_div = QPushButton(self.centralwidget)
        self.Button_div.setObjectName(u"Button_div")
        self.Button_div.setGeometry(QRect(280, 390, 71, 71))
        self.Button_div.setFont(font)
        self.Button_div.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_sub = QPushButton(self.centralwidget)
        self.Button_sub.setObjectName(u"Button_sub")
        self.Button_sub.setGeometry(QRect(280, 460, 71, 71))
        self.Button_sub.setFont(font)
        self.Button_sub.setLayoutDirection(Qt.LeftToRight)
        self.Button_sub.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_fac = QPushButton(self.centralwidget)
        self.Button_fac.setObjectName(u"Button_fac")
        self.Button_fac.setGeometry(QRect(210, 250, 71, 71))
        self.Button_fac.setFont(font)
        self.Button_fac.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_ans = QPushButton(self.centralwidget)
        self.Button_ans.setObjectName(u"Button_ans")
        self.Button_ans.setGeometry(QRect(210, 130, 71, 51))
        self.Button_ans.setFont(font)
        self.Button_ans.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_ln = QPushButton(self.centralwidget)
        self.Button_ln.setObjectName(u"Button_ln")
        self.Button_ln.setGeometry(QRect(70, 180, 71, 71))
        self.Button_ln.setFont(font)
        self.Button_ln.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_del = QPushButton(self.centralwidget)
        self.Button_del.setObjectName(u"Button_del")
        self.Button_del.setGeometry(QRect(350, 130, 71, 51))
        self.Button_del.setFont(font)
        self.Button_del.setToolTipDuration(-1)
        self.Button_del.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: #FF8000; \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}\n"
"\n"
"QWhatThis {\n"
"	backgroound-color:#FFFFFF;\n"
"}\n"
"\n"
"QToolTip {\n"
"	background-color:#FFFFFF;\n"
"}")
        self.Button_log = QPushButton(self.centralwidget)
        self.Button_log.setObjectName(u"Button_log")
        self.Button_log.setGeometry(QRect(0, 180, 71, 71))
        self.Button_log.setFont(font)
        self.Button_log.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_ac = QPushButton(self.centralwidget)
        self.Button_ac.setObjectName(u"Button_ac")
        self.Button_ac.setGeometry(QRect(280, 130, 71, 51))
        self.Button_ac.setFont(font)
        self.Button_ac.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: #FF8000; \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_abs = QPushButton(self.centralwidget)
        self.Button_abs.setObjectName(u"Button_abs")
        self.Button_abs.setGeometry(QRect(350, 390, 71, 71))
        self.Button_abs.setFont(font)
        self.Button_abs.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_pi = QPushButton(self.centralwidget)
        self.Button_pi.setObjectName(u"Button_pi")
        self.Button_pi.setGeometry(QRect(350, 530, 71, 71))
        font2 = QFont()
        font2.setFamily(u"Times New Roman")
        font2.setPointSize(19)
        self.Button_pi.setFont(font2)
        self.Button_pi.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_inverse = QPushButton(self.centralwidget)
        self.Button_inverse.setObjectName(u"Button_inverse")
        self.Button_inverse.setGeometry(QRect(350, 320, 71, 71))
        self.Button_inverse.setFont(font)
        self.Button_inverse.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_2sqrt = QPushButton(self.centralwidget)
        self.Button_2sqrt.setObjectName(u"Button_2sqrt")
        self.Button_2sqrt.setGeometry(QRect(350, 250, 71, 71))
        self.Button_2sqrt.setFont(font)
        self.Button_2sqrt.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_2pow = QPushButton(self.centralwidget)
        self.Button_2pow.setObjectName(u"Button_2pow")
        self.Button_2pow.setGeometry(QRect(350, 180, 71, 71))
        self.Button_2pow.setFont(font)
        self.Button_2pow.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_e = QPushButton(self.centralwidget)
        self.Button_e.setObjectName(u"Button_e")
        self.Button_e.setGeometry(QRect(350, 460, 71, 71))
        self.Button_e.setFont(font)
        self.Button_e.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_converter = QPushButton(self.centralwidget)
        self.Button_converter.setObjectName(u"Button_converter")
        self.Button_converter.setGeometry(QRect(0, 130, 141, 51))
        font3 = QFont()
        font3.setFamily(u"Tahoma")
        font3.setPointSize(14)
        self.Button_converter.setFont(font3)
        self.Button_converter.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_converter.setCheckable(True)
        self.Button_msub = QPushButton(self.centralwidget)
        self.Button_msub.setObjectName(u"Button_msub")
        self.Button_msub.setGeometry(QRect(210, 180, 71, 71))
        self.Button_msub.setFont(font)
        self.Button_msub.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_m = QPushButton(self.centralwidget)
        self.Button_m.setObjectName(u"Button_m")
        self.Button_m.setGeometry(QRect(140, 130, 71, 51))
        self.Button_m.setFont(font)
        self.Button_m.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(33, 33, 33);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
" qproperty-alignment: 'AlignVCenter | AlignRight';\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}")
        self.Button_mdel = QPushButton(self.centralwidget)
        self.Button_mdel.setObjectName(u"Button_mdel")
        self.Button_mdel.setGeometry(QRect(280, 180, 71, 71))
        self.Button_mdel.setFont(font)
        self.Button_mdel.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_madd = QPushButton(self.centralwidget)
        self.Button_madd.setObjectName(u"Button_madd")
        self.Button_madd.setGeometry(QRect(140, 180, 71, 71))
        self.Button_madd.setFont(font)
        self.Button_madd.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(38, 38, 38);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid #1C1C1C;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35));\n"
"}")
        self.Button_00 = QPushButton(self.centralwidget)
        self.Button_00.setObjectName(u"Button_00")
        self.Button_00.setGeometry(QRect(70, 530, 71, 71))
        self.Button_00.setFont(font)
        self.Button_00.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(30, 30, 30);\n"
"  color: rgb(235, 235, 235); \n"
"  border: 1px solid rgb(35, 35, 35);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 rgb(35, 35, 35), stop: 1 rgb(35, 35, 35))\n"
"}")
        self.Button_close = QPushButton(self.centralwidget)
        self.Button_close.setObjectName(u"Button_close")
        self.Button_close.setGeometry(QRect(380, 0, 41, 31))
        font4 = QFont()
        font4.setFamily(u"Arial Rounded MT Bold")
        font4.setPointSize(12)
        self.Button_close.setFont(font4)
        self.Button_close.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(35,35,35);\n"
"  color: #ff8000; \n"
"  border: 1px solid 	#232323;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"	color: #ff8000;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: #ff8000;\n"
"	color: rgb(30,30,30);\n"
"}\n"
"\n"
"")
        self.currFrame = QFrame(self.centralwidget)
        self.currFrame.setObjectName(u"currFrame")
        self.currFrame.setEnabled(True)
        self.currFrame.setGeometry(QRect(0, 180, 218, 81))
        self.currFrame.setAutoFillBackground(False)
        self.currFrame.setStyleSheet(u"QFrame {\n"
"background-color: \"#2b2b2b\";\n"
"color: \"#CCCCCC\";\n"
"border:  1px solid #101010;\n"
"}\n"
"QFrame QLabel {\n"
"border: 0px\n"
"}\n"
"")
        self.currFrame.setFrameShape(QFrame.StyledPanel)
        self.currFrame.setFrameShadow(QFrame.Raised)
        self.combo_currFrom = QComboBox(self.currFrame)
        self.combo_currFrom.setObjectName(u"combo_currFrom")
        self.combo_currFrom.setGeometry(QRect(10, 20, 72, 41))
        palette1 = QPalette()
        brush2 = QBrush(QColor(204, 204, 204, 255))
        brush2.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Active, QPalette.WindowText, brush2)
        palette1.setBrush(QPalette.Active, QPalette.Button, brush1)
        palette1.setBrush(QPalette.Active, QPalette.Text, brush2)
        palette1.setBrush(QPalette.Active, QPalette.ButtonText, brush2)
        palette1.setBrush(QPalette.Active, QPalette.Base, brush1)
        palette1.setBrush(QPalette.Active, QPalette.Window, brush1)
        brush3 = QBrush(QColor(128, 128, 128, 255))
        brush3.setStyle(Qt.SolidPattern)
        palette1.setBrush(QPalette.Active, QPalette.Highlight, brush3)
        brush4 = QBrush(QColor(204, 204, 204, 128))
        brush4.setStyle(Qt.NoBrush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette.Active, QPalette.PlaceholderText, brush4)
#endif
        palette1.setBrush(QPalette.Inactive, QPalette.WindowText, brush2)
        palette1.setBrush(QPalette.Inactive, QPalette.Button, brush1)
        palette1.setBrush(QPalette.Inactive, QPalette.Text, brush2)
        palette1.setBrush(QPalette.Inactive, QPalette.ButtonText, brush2)
        palette1.setBrush(QPalette.Inactive, QPalette.Base, brush1)
        palette1.setBrush(QPalette.Inactive, QPalette.Window, brush1)
        palette1.setBrush(QPalette.Inactive, QPalette.Highlight, brush3)
        brush5 = QBrush(QColor(204, 204, 204, 128))
        brush5.setStyle(Qt.NoBrush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette.Inactive, QPalette.PlaceholderText, brush5)
#endif
        palette1.setBrush(QPalette.Disabled, QPalette.WindowText, brush2)
        palette1.setBrush(QPalette.Disabled, QPalette.Button, brush1)
        palette1.setBrush(QPalette.Disabled, QPalette.Text, brush2)
        palette1.setBrush(QPalette.Disabled, QPalette.ButtonText, brush2)
        palette1.setBrush(QPalette.Disabled, QPalette.Base, brush1)
        palette1.setBrush(QPalette.Disabled, QPalette.Window, brush1)
        palette1.setBrush(QPalette.Disabled, QPalette.Highlight, brush3)
        brush6 = QBrush(QColor(204, 204, 204, 128))
        brush6.setStyle(Qt.NoBrush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette.Disabled, QPalette.PlaceholderText, brush6)
#endif
        self.combo_currFrom.setPalette(palette1)
        font5 = QFont()
        font5.setFamily(u"Arial")
        font5.setPointSize(12)
        font5.setBold(True)
        font5.setWeight(75)
        self.combo_currFrom.setFont(font5)
        self.combo_currFrom.setStyleSheet(u"QComboBox {\n"
"background-color: rgb(30, 30, 30);\n"
"color: \"#CCCCCC\";\n"
"selection-background-color: grey;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"selection-background-color: grey;\n"
"}\n"
"\n"
"QComboBox::item {\n"
"selection-background-color: grey;\n"
"}\n"
"")
        self.combo_currFrom.setEditable(False)
        self.combo_currFrom.setMaxVisibleItems(15)
        self.combo_currFrom.setFrame(False)
        self.combo_currTo = QComboBox(self.currFrame)
        self.combo_currTo.setObjectName(u"combo_currTo")
        self.combo_currTo.setGeometry(QRect(135, 20, 72, 41))
        palette2 = QPalette()
        palette2.setBrush(QPalette.Active, QPalette.WindowText, brush2)
        palette2.setBrush(QPalette.Active, QPalette.Button, brush1)
        palette2.setBrush(QPalette.Active, QPalette.Text, brush2)
        palette2.setBrush(QPalette.Active, QPalette.ButtonText, brush2)
        palette2.setBrush(QPalette.Active, QPalette.Base, brush1)
        palette2.setBrush(QPalette.Active, QPalette.Window, brush1)
        palette2.setBrush(QPalette.Active, QPalette.Highlight, brush3)
        brush7 = QBrush(QColor(204, 204, 204, 128))
        brush7.setStyle(Qt.NoBrush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette.Active, QPalette.PlaceholderText, brush7)
#endif
        palette2.setBrush(QPalette.Inactive, QPalette.WindowText, brush2)
        palette2.setBrush(QPalette.Inactive, QPalette.Button, brush1)
        palette2.setBrush(QPalette.Inactive, QPalette.Text, brush2)
        palette2.setBrush(QPalette.Inactive, QPalette.ButtonText, brush2)
        palette2.setBrush(QPalette.Inactive, QPalette.Base, brush1)
        palette2.setBrush(QPalette.Inactive, QPalette.Window, brush1)
        palette2.setBrush(QPalette.Inactive, QPalette.Highlight, brush3)
        brush8 = QBrush(QColor(204, 204, 204, 128))
        brush8.setStyle(Qt.NoBrush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette.Inactive, QPalette.PlaceholderText, brush8)
#endif
        palette2.setBrush(QPalette.Disabled, QPalette.WindowText, brush2)
        palette2.setBrush(QPalette.Disabled, QPalette.Button, brush1)
        palette2.setBrush(QPalette.Disabled, QPalette.Text, brush2)
        palette2.setBrush(QPalette.Disabled, QPalette.ButtonText, brush2)
        palette2.setBrush(QPalette.Disabled, QPalette.Base, brush1)
        palette2.setBrush(QPalette.Disabled, QPalette.Window, brush1)
        palette2.setBrush(QPalette.Disabled, QPalette.Highlight, brush3)
        brush9 = QBrush(QColor(204, 204, 204, 128))
        brush9.setStyle(Qt.NoBrush)
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette.Disabled, QPalette.PlaceholderText, brush9)
#endif
        self.combo_currTo.setPalette(palette2)
        self.combo_currTo.setFont(font5)
        self.combo_currTo.setAutoFillBackground(False)
        self.combo_currTo.setStyleSheet(u"QComboBox {\n"
"background-color: rgb(30, 30, 30);\n"
"color: \"#CCCCCC\";\n"
"selection-background-color: grey;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"selection-background-color: grey;\n"
"}\n"
"\n"
"QComboBox::item {\n"
"selection-background-color: grey;\n"
"}\n"
"")
        self.combo_currTo.setEditable(False)
        self.combo_currTo.setMaxVisibleItems(15)
        self.combo_currTo.setFrame(False)
        self.label = QLabel(self.currFrame)
        self.label.setObjectName(u"label")
        self.label.setGeometry(QRect(93, 30, 31, 21))
        font6 = QFont()
        font6.setFamily(u"Arial")
        font6.setPointSize(24)
        self.label.setFont(font6)
        self.label.setStyleSheet(u"QLabel {\n"
"color:  #FF8000; \n"
"}")
        self.display_currencyFrom = QLabel(self.centralwidget)
        self.display_currencyFrom.setObjectName(u"display_currencyFrom")
        self.display_currencyFrom.setGeometry(QRect(16, 40, 391, 20))
        self.display_currencyFrom.setFont(font5)
        self.display_currencyFrom.setStyleSheet(u"QLabel {\n"
"color: rgb(235, 235, 235);\n"
"}")
        self.display_currencyTo = QLabel(self.centralwidget)
        self.display_currencyTo.setObjectName(u"display_currencyTo")
        self.display_currencyTo.setGeometry(QRect(250, 100, 161, 20))
        font7 = QFont()
        font7.setFamily(u"Arial")
        font7.setPointSize(10)
        font7.setBold(True)
        font7.setWeight(75)
        self.display_currencyTo.setFont(font7)
        self.display_currencyTo.setStyleSheet(u"QLabel {\n"
"color: rgb(235, 235, 235);\n"
"}")
        self.display_currencyTo.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)
        self.Button_help = QPushButton(self.centralwidget)
        self.Button_help.setObjectName(u"Button_help")
        self.Button_help.setGeometry(QRect(300, 0, 41, 31))
        font8 = QFont()
        font8.setFamily(u"Tahoma")
        font8.setPointSize(11)
        self.Button_help.setFont(font8)
        self.Button_help.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(35, 35, 35);\n"
"  color:#ff8000; \n"
"  border: 1px solid 	#232323;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: rgb(50,50,50);\n"
"}")
        self.Button_help.setCheckable(True)
        self.Frame_header = QFrame(self.centralwidget)
        self.Frame_header.setObjectName(u"Frame_header")
        self.Frame_header.setGeometry(QRect(0, 0, 301, 31))
        self.Frame_header.setAutoFillBackground(False)
        self.Frame_header.setStyleSheet(u"QFrame{\n"
"	 background-color: rgb(35, 35, 35);\n"
"  border: 1px solid 	#232323;\n"
"}")
        self.Frame_header.setFrameShape(QFrame.StyledPanel)
        self.Frame_header.setFrameShadow(QFrame.Raised)
        self.Button_minimize = QPushButton(self.centralwidget)
        self.Button_minimize.setObjectName(u"Button_minimize")
        self.Button_minimize.setGeometry(QRect(340, 0, 41, 31))
        self.Button_minimize.setFont(font4)
        self.Button_minimize.setStyleSheet(u"QPushButton {\n"
"  background-color: rgb(35, 35, 35);\n"
"  color:#ff8000; \n"
"  border: 1px solid 	#232323;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                      stop: 0 #1C1C1C, stop: 1 #1C1C1C);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: rgb(50,50,50);\n"
"}")
        self.notify_M = QLabel(self.centralwidget)
        self.notify_M.setObjectName(u"notify_M")
        self.notify_M.setGeometry(QRect(10, 110, 21, 21))
        font9 = QFont()
        font9.setFamily(u"Tahoma")
        font9.setPointSize(13)
        font9.setBold(True)
        font9.setWeight(75)
        self.notify_M.setFont(font9)
        self.notify_M.setStyleSheet(u"QLabel {\n"
"color: #ff8000;\n"
"}")
        self.notify_E = QLabel(self.centralwidget)
        self.notify_E.setObjectName(u"notify_E")
        self.notify_E.setGeometry(QRect(40, 110, 21, 21))
        self.notify_E.setFont(font9)
        self.notify_E.setStyleSheet(u"QLabel {\n"
"color: #ff2000;\n"
"}")
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        self.Button_close.clicked.connect(MainWindow.close)

        self.Button_equals.setDefault(True)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle("")
#if QT_CONFIG(whatsthis)
        self.Button_sin.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Vypo\u010d\u00edt\u00e1 sinus \u010d\u00edsla v radi\u00e1nech.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_sin.setText(QCoreApplication.translate("MainWindow", u"sin", None))
        self.display.setText(QCoreApplication.translate("MainWindow", u"0", None))
#if QT_CONFIG(whatsthis)
        self.Button_cos.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Vypo\u010d\u00edt\u00e1 cosinus \u010d\u00edsla v radi\u00e1nech.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_cos.setText(QCoreApplication.translate("MainWindow", u"cos", None))
#if QT_CONFIG(whatsthis)
        self.Button_percent.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>V\u00fdpo\u010det procentn\u00ed hodnoty \u010d\u00edsla ze z\u00e1kladu</p><p>Pou\u017eit\u00ed: \u010d\u00edslo -&gt; procento -&gt; z\u00e1klad</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_percent.setText(QCoreApplication.translate("MainWindow", u"%", None))
#if QT_CONFIG(whatsthis)
        self.Button_tan.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Vypo\u010d\u00edt\u00e1 tangens \u010d\u00edsla v radi\u00e1nech.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_tan.setText(QCoreApplication.translate("MainWindow", u"tan", None))
        self.Button_9.setText(QCoreApplication.translate("MainWindow", u"9", None))
        self.Button_8.setText(QCoreApplication.translate("MainWindow", u"8", None))
#if QT_CONFIG(whatsthis)
        self.Button_sqrt.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Obecn\u00e1 odmocnina \u010d\u00edsla.</p><p>Pou\u017eit\u00ed: n -&gt; odmocnina -&gt; \u010d\u00edslo.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_sqrt.setText(QCoreApplication.translate("MainWindow", u"\u207f\u221a", None))
        self.Button_7.setText(QCoreApplication.translate("MainWindow", u"7", None))
        self.Button_6.setText(QCoreApplication.translate("MainWindow", u"6", None))
        self.Button_5.setText(QCoreApplication.translate("MainWindow", u"5", None))
#if QT_CONFIG(whatsthis)
        self.Button_pow.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Obecn\u00e1 mocnina \u010d\u00edsla.</p><p>Pou\u017eit\u00ed: n -&gt; mocnina -&gt; \u010d\u00edslo.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_pow.setText(QCoreApplication.translate("MainWindow", u"x\u207f", None))
        self.Button_4.setText(QCoreApplication.translate("MainWindow", u"4", None))
#if QT_CONFIG(whatsthis)
        self.Button_3.setWhatsThis("")
#endif // QT_CONFIG(whatsthis)
        self.Button_3.setText(QCoreApplication.translate("MainWindow", u"3", None))
        self.Button_2.setText(QCoreApplication.translate("MainWindow", u"2", None))
#if QT_CONFIG(whatsthis)
        self.Button_1.setWhatsThis("")
#endif // QT_CONFIG(whatsthis)
        self.Button_1.setText(QCoreApplication.translate("MainWindow", u"1", None))
        self.Button_dot.setText(QCoreApplication.translate("MainWindow", u",", None))
#if QT_CONFIG(whatsthis)
        self.Button_equals.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Vypo\u010d\u00edt\u00e1 v\u00fdsledek prov\u00e1d\u011bn\u00e9 operace.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_equals.setText(QCoreApplication.translate("MainWindow", u"=", None))
#if QT_CONFIG(whatsthis)
        self.Button_0.setWhatsThis("")
#endif // QT_CONFIG(whatsthis)
        self.Button_0.setText(QCoreApplication.translate("MainWindow", u"0", None))
#if QT_CONFIG(whatsthis)
        self.Button_add.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Sou\u010det dvou \u010d\u00edsel.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_add.setText(QCoreApplication.translate("MainWindow", u"+", None))
#if QT_CONFIG(whatsthis)
        self.Button_mul.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Sou\u010din dvou \u010d\u00edsel.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_mul.setText(QCoreApplication.translate("MainWindow", u"*", None))
#if QT_CONFIG(whatsthis)
        self.Button_div.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Pod\u00edl dvou \u010d\u00edsel.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_div.setText(QCoreApplication.translate("MainWindow", u"/", None))
#if QT_CONFIG(whatsthis)
        self.Button_sub.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Rozd\u00edl dvou \u010d\u00edsel.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_sub.setText(QCoreApplication.translate("MainWindow", u"-", None))
#if QT_CONFIG(whatsthis)
        self.Button_fac.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Faktori\u00e1l \u010d\u00edsla.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_fac.setText(QCoreApplication.translate("MainWindow", u"!", None))
#if QT_CONFIG(whatsthis)
        self.Button_ans.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Vlo\u017e\u00ed v\u00fdsledek posledn\u00edho v\u00fdpo\u010dtu na displej.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_ans.setText(QCoreApplication.translate("MainWindow", u"Ans", None))
#if QT_CONFIG(whatsthis)
        self.Button_ln.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Pou\u017eit\u00ed ln &lt;x&gt;.</p><p>Vypo\u010d\u00edt\u00e1 p\u0159irozen\u00fd logaritmus z \u010d\u00edsla x.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_ln.setText(QCoreApplication.translate("MainWindow", u"ln", None))
#if QT_CONFIG(whatsthis)
        self.Button_del.setWhatsThis(QCoreApplication.translate("MainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:15pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Vyma\u017ee posledn\u00ed znak z displeje.</span></p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_del.setText(QCoreApplication.translate("MainWindow", u"\u232b", None))
#if QT_CONFIG(whatsthis)
        self.Button_log.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Pou\u017eit\u00ed &lt;x&gt; log &lt;a&gt;.</p><p>Vypo\u010d\u00edt\u00e1 logaritmus z \u010d\u00edsla x o z\u00e1kladu a.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_log.setText(QCoreApplication.translate("MainWindow", u"log", None))
#if QT_CONFIG(whatsthis)
        self.Button_ac.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Vyresetuje kalkula\u010dku a cel\u00fd v\u00fdpo\u010det.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_ac.setText(QCoreApplication.translate("MainWindow", u"C", None))
#if QT_CONFIG(whatsthis)
        self.Button_abs.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Absolutn\u00ed hodnota \u010d\u00edsla.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_abs.setText(QCoreApplication.translate("MainWindow", u"|x|", None))
#if QT_CONFIG(whatsthis)
        self.Button_pi.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>\u010c\u00edslo PI se zobraz\u00ed na displeji.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_pi.setText(QCoreApplication.translate("MainWindow", u"\u03c0", None))
#if QT_CONFIG(whatsthis)
        self.Button_inverse.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Inverzn\u00ed hodnota \u010d\u00edsla.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_inverse.setText(QCoreApplication.translate("MainWindow", u"1/x", None))
#if QT_CONFIG(whatsthis)
        self.Button_2sqrt.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>V\u00fdpo\u010det druh\u00e9 odmocniny \u010d\u00edsla.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_2sqrt.setText(QCoreApplication.translate("MainWindow", u"\u221a", None))
#if QT_CONFIG(whatsthis)
        self.Button_2pow.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>V\u00fdpo\u010det druh\u00e9 mocniny \u010d\u00edsla.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_2pow.setText(QCoreApplication.translate("MainWindow", u"x\u00b2", None))
#if QT_CONFIG(whatsthis)
        self.Button_e.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Eulerovo \u010d\u00edslo ze zobraz\u00ed na displeji.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_e.setText(QCoreApplication.translate("MainWindow", u"e", None))
#if QT_CONFIG(whatsthis)
        self.Button_converter.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Otev\u0159e p\u0159evodn\u00edk m\u011bn.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_converter.setText(QCoreApplication.translate("MainWindow", u"P\u0159evod m\u011bn", None))
#if QT_CONFIG(whatsthis)
        self.Button_msub.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Ode\u010dte \u010d\u00edslo z displeje k \u010d\u00edslu v pam\u011bti.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_msub.setText(QCoreApplication.translate("MainWindow", u"M-", None))
#if QT_CONFIG(whatsthis)
        self.Button_m.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Ulo\u017e\u00ed \u010d\u00edslo z displeje do pam\u011bti.</p><p>Pokud sv\u00edt\u00ed indik\u00e1tor <span style=\" font-weight:600;\">M</span>, pak \u010d\u00edslo vlo\u017e\u00ed z pam\u011bti na displej.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_m.setText(QCoreApplication.translate("MainWindow", u"M", None))
#if QT_CONFIG(whatsthis)
        self.Button_mdel.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>Sma\u017ee z pam\u011bti ulo\u017een\u00e9 \u010dislo.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_mdel.setText(QCoreApplication.translate("MainWindow", u"MD", None))
#if QT_CONFIG(whatsthis)
        self.Button_madd.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p>P\u0159i\u010dte \u010d\u00edslo na displeji k \u010d\u00edslu v pam\u011bti.</p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_madd.setText(QCoreApplication.translate("MainWindow", u"M+", None))
#if QT_CONFIG(whatsthis)
        self.Button_00.setWhatsThis("")
#endif // QT_CONFIG(whatsthis)
        self.Button_00.setText(QCoreApplication.translate("MainWindow", u"00", None))
#if QT_CONFIG(whatsthis)
        self.Button_close.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p><span style=\" font-size:72pt;\">I &lt;3 FIT</span></p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.Button_close.setText(QCoreApplication.translate("MainWindow", u"X", None))
#if QT_CONFIG(whatsthis)
        self.combo_currFrom.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p><span style=\" font-weight:400;\">V\u00fdchoz\u00ed m\u011bna p\u0159evodu</span></p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.combo_currFrom.setCurrentText("")
#if QT_CONFIG(whatsthis)
        self.combo_currTo.setWhatsThis(QCoreApplication.translate("MainWindow", u"<html><head/><body><p><span style=\" font-weight:400;\">C\u00edlov\u00e1 m\u011bna p\u0159evodu.</span></p></body></html>", None))
#endif // QT_CONFIG(whatsthis)
        self.label.setText(QCoreApplication.translate("MainWindow", u"\u2794", None))
        self.display_currencyFrom.setText("")
        self.display_currencyTo.setText("")
#if QT_CONFIG(whatsthis)
        self.Button_help.setWhatsThis("")
#endif // QT_CONFIG(whatsthis)
        self.Button_help.setText(QCoreApplication.translate("MainWindow", u"?", None))
#if QT_CONFIG(whatsthis)
        self.Frame_header.setWhatsThis("")
#endif // QT_CONFIG(whatsthis)
        self.Button_minimize.setText(QCoreApplication.translate("MainWindow", u"_", None))
        self.notify_M.setText(QCoreApplication.translate("MainWindow", u"M", None))
        self.notify_E.setText(QCoreApplication.translate("MainWindow", u"E", None))
    # retranslateUi

