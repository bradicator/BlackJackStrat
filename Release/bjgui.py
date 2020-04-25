
  
#!/usr/bin/env python


#############################################################################
##
## Copyright (C) 2013 Riverbank Computing Limited.
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
##
## This file is part of the examples of PyQt.
##
## $QT_BEGIN_LICENSE:BSD$
## You may use this file under the terms of the BSD license as follows:
##
## "Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
##   * Redistributions of source code must retain the above copyright
##     notice, this list of conditions and the following disclaimer.
##   * Redistributions in binary form must reproduce the above copyright
##     notice, this list of conditions and the following disclaimer in
##     the documentation and/or other materials provided with the
##     distribution.
##   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
##     the names of its contributors may be used to endorse or promote
##     products derived from this software without specific prior written
##     permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
## $QT_END_LICENSE$
##
#############################################################################


from PyQt5.QtCore import QDateTime, Qt, QTimer
from PyQt5.QtWidgets import (QApplication, QCheckBox, QComboBox, QDateTimeEdit,
        QDial, QDialog, QGridLayout, QGroupBox, QHBoxLayout, QLabel, QLineEdit,
        QProgressBar, QPushButton, QRadioButton, QScrollBar, QSizePolicy,
        QSlider, QSpinBox, QStyleFactory, QTableWidget, QTabWidget, QTextEdit,
        QVBoxLayout, QWidget, QShortcut)
from PyQt5.QtGui import QKeySequence
import os


class WidgetGallery(QDialog):
    def __init__(self, parent=None):
        super(WidgetGallery, self).__init__(parent)

        self.originalPalette = QApplication.palette()
        QApplication.setStyle(QStyleFactory.create("Fusion"))



        self.createPlayerHandGroupBox()
        self.createDealerHandGroupBox()
        self.createResultGroupBox()
        self.createNewAndComputeGroupBox()



        mainLayout = QGridLayout()
        #mainLayout.addWidget(self.topLeftGroupBox, 1, 0)
        mainLayout.addWidget(self.PlayerHandGroupBox, 2, 0)
        mainLayout.addWidget(self.DealerHandGroupBox, 1, 0)
        mainLayout.addWidget(self.ResultGroupBox, 3, 0)
        mainLayout.addWidget(self.NewAndComputeGroupBox, 4, 0)
        
        mainLayout.setRowStretch(1, 1)
        mainLayout.setRowStretch(2, 1)
        mainLayout.setColumnStretch(0, 1)
        mainLayout.setColumnStretch(1, 1)
        self.setLayout(mainLayout)

        self.setWindowTitle("BlackJackHelper")
        self.phand = []
        self.dhand = []
        self.nround = 0

    
    def createNewAndComputeGroupBox(self):
        self.NewAndComputeGroupBox = QGroupBox('Clear/Compute:')
        bnew = QPushButton('New Round')
        bclear = QPushButton('Clear')
        bcompute = QPushButton('Go')
        
        bnew.clicked.connect(self.newfunction)
        bclear.clicked.connect(self.clearfunction)
        bcompute.clicked.connect(self.computefunction)
        
        clearseq = QShortcut(QKeySequence("c"), bclear)
        clearseq.activated.connect(bclear.click)
        
        newroundseq = QShortcut(QKeySequence("n"), bnew)
        newroundseq.activated.connect(bnew.click)
        
        big_enter_seq = QKeySequence("g")
        big_enter = QShortcut(big_enter_seq, bcompute)
        # Linking
        big_enter.activated.connect(bcompute.click)
        big_enter.activatedAmbiguously.connect(bcompute.click)
        
        layout = QHBoxLayout()
        layout.addWidget(bnew)
        layout.addWidget(bclear)
        layout.addWidget(bcompute)
        layout.addStretch(5)
        self.NewAndComputeGroupBox.setLayout(layout)
        
    def clearfunction(self):
        self.dealerline.clear()
        self.playerline.clear()
        self.sbutton.setChecked(False)
        self.phand = []
        self.dhand = []
    
    def newfunction(self):
        self.nround += 1
        self.setWindowTitle(f'BlackJackHelper nrounds: {self.nround}')
        self.resulttext.setText("waiting for inputs")
        self.clearfunction()
    
    def computefunction(self):
        pfinal = self.playerline.text().split(',')
        dfinal = self.dealerline.text().split(',')
        if len(pfinal) < 2 or len(dfinal) > 1:
            self.resulttext.setText("wrong hand input!")
            return
        if self.sbutton.isChecked():
            cmdstring = ' '.join(['./blackjack s'] + dfinal + pfinal)
        else:
            cmdstring = ' '.join(['./blackjack n'] + dfinal + pfinal)

        print(cmdstring)
        stream = os.popen(cmdstring)
        output = stream.read()
        self.resulttext.setText(output)
        stream.close()
        return

    def clickfunction(self, card, widget):
        def f():
            self.phand.append(str(card))
            widget.setText(','.join(self.phand))
        return f
            
    def clickfunction2(self, card, widget):
        def f():
            self.dhand.append(str(card))
            widget.setText(','.join(self.dhand))
        return f
        
    def createResultGroupBox(self):
        self.ResultGroupBox = QGroupBox("Results:")
        self.resulttext = QLineEdit('waiting for inputs')
        self.resulttext.setFixedWidth(400)
        f = self.resulttext.font()
        f.setPointSize(27) # sets the size to 27
        self.resulttext.setFont(f)
        layout = QHBoxLayout()
        layout.addWidget(self.resulttext)
        layout.addStretch(1)
        self.ResultGroupBox.setLayout(layout)
        
        
    def createPlayerHandGroupBox(self):
        self.PlayerHandGroupBox = QGroupBox("Player's Hand:")
        

        buttonlist = []
        for i in ["A","2","3","4","5","6","7","8","9","10+"]:
            buttonlist.append(QPushButton(i))
        [b.setFixedWidth(25) for b in buttonlist]
        self.playerline = QLineEdit('')
        self.playerline.setFixedWidth(100)
        for i in range(1, 11):
            buttonlist[i-1].clicked.connect(self.clickfunction(i, self.playerline))
        
        self.sbutton = QRadioButton("Splited")
        layout = QHBoxLayout()
        [layout.addWidget(b) for b in buttonlist]
        layout.addWidget(self.playerline)
        layout.addWidget(self.sbutton)
        layout.addStretch(1)
        self.PlayerHandGroupBox.setLayout(layout)
    
    def createDealerHandGroupBox(self):
        self.DealerHandGroupBox = QGroupBox("Dealer's Up Card:")

        buttonlist = []
        for i in ["A","2","3","4","5","6","7","8","9","10+"]:
            buttonlist.append(QPushButton(i))
        [b.setFixedWidth(25) for b in buttonlist]
        self.dealerline = QLineEdit('')
        self.dealerline.setFixedWidth(100)
        for i in range(1, 11):
            buttonlist[i-1].clicked.connect(self.clickfunction2(i, self.dealerline))
        layout = QHBoxLayout()
        [layout.addWidget(b) for b in buttonlist]
        layout.addWidget(self.dealerline)
        layout.addStretch(1)
        self.DealerHandGroupBox.setLayout(layout)  

    

if __name__ == '__main__':

    import sys

    app = QApplication(sys.argv)
    gallery = WidgetGallery()
    gallery.show()
    sys.exit(app.exec_()) 

