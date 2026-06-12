import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                               QHBoxLayout, QGridLayout, QLabel, QSlider, QTimeEdit, 
                               QSpacerItem, QSizePolicy)
from PySide6.QtCore import Qt, QTime
from PySide6.QtGui import QFont

# Data from wordclock.cpp
leds_time_it_is = [0, 1, 3, 4, 5]
leds_minutes = [124, 123, 122, 121]

leds_time_minutes = [
    [101, 100,  99],                                                # UHR
    [  7,   8,   9,  10,  41,  40,  39,  38],                       # FÜNF, NACH
    [ 21,  20,  19,  18,  41,  40,  39,  38],                       # ZEHN, NACH
    [ 26,  27,  28,  29,  30,  31,  32,  41,  40,  39,  38],        # VIERTEL, NACH
    [ 17,  16,  15,  15,  14,  13,  12,  11,  41,  40,  39,  38],   # ZWANZIG, NACH
    [  7,   8,   9,  10,  37,  36,  35,  44,  45,  46,  47],        # FÜNF, VOR, HALB
    [ 44,  45,  46,  47],                                           # HALB
    [  7,   8,   9,  10,  41,  40,  39,  38,  44,  45,  46,  47],   # FÜNF, NACH, HALB
    [ 17,  16,  15,  15,  14,  13,  12,  11,  37,  36,  35],        # ZWANZIG, VOR
    [ 26,  27,  28,  29,  30,  31,  32,  37,  36,  35],             # VIERTEL, VOR
    [ 21,  20,  19,  18,  37,  36,  35],                            # ZEHN, VOR
    [  7,   8,   9,  10,  37,  36,  35]                             # FÜNF, VOR
]

leds_time_hours = [
    [ 49,  50,  51,  52,  53],      # ZWÖLF
    [ 63,  62,  61,  60],           # EINS
    [ 65,  64,  63,  62],           # ZWEI
    [ 67,  68,  69,  70],           # DREI
    [ 80,  79,  78,  77],           # VIER
    [ 73,  74,  75,  76],           # FÜNF
    [108, 107, 106, 105, 104],      # SECHS
    [ 60,  59,  58,  57,  56,  55], # SIEBEN
    [ 89,  90,  91,  92],           # ACHT
    [ 84,  83,  82,  81],           # NEUN
    [ 93,  94,  95,  96],           # ZEHN
    [ 87,  86,  85]                 # ELF
]

matrix = [
    "ESXISTXFÜNF", # 0-10
    "ZEHNZWANZIG", # 21 down to 11
    "DREIVIERTEL", # 22-32
    "XXNACHVORXX", # 43 down to 33
    "HALBXZWÖLFX", # 44-54
    "ZWEINSIEBEN", # 65 down to 55
    "XDREIXXFÜNF", # 66-76
    "ELFNEUNVIER", # 87 down to 77
    "XACHTZEHNRS", # 88-98
    "XSECHSXXUHR"  # 109 down to 99
]

def get_lit_leds(hour, minute):
    lit = []
    lit.extend(leds_time_it_is)
    
    tmp_hour = hour
    tmp_minute = minute - (minute % 5)
    
    if tmp_minute >= 25:
        tmp_hour += 1
        
    tmp_minute = tmp_minute // 5
    tmp_hour = tmp_hour % 12
    minutessum = minute % 5
    
    if tmp_minute < len(leds_time_minutes):
        lit.extend(leds_time_minutes[tmp_minute])
        
    if tmp_hour < len(leds_time_hours):
        lit.extend(leds_time_hours[tmp_hour])
        
    min_dots = leds_minutes[:minutessum]
    return lit, min_dots

class WordClockGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("HA Wordclock Test GUI")
        self.setStyleSheet("background-color: #121212; color: #fff;")
        
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        
        layout = QVBoxLayout(main_widget)
        layout.setSpacing(20)
        layout.setContentsMargins(30, 30, 30, 30)
        
        # Controls Layout
        controls_layout = QHBoxLayout()
        
        self.time_edit = QTimeEdit()
        self.time_edit.setDisplayFormat("HH:mm")
        self.time_edit.setFont(QFont("Consolas", 16))
        self.time_edit.setStyleSheet("background-color: #333; color: #fff; padding: 5px;")
        self.time_edit.timeChanged.connect(self.on_time_edit_changed)
        
        self.slider = QSlider(Qt.Horizontal)
        self.slider.setRange(0, 1439)
        self.slider.valueChanged.connect(self.on_slider_changed)
        
        controls_layout.addWidget(self.time_edit)
        controls_layout.addWidget(self.slider)
        
        layout.addLayout(controls_layout)
        
        # Matrix Layout
        self.matrix_labels = {}
        grid_layout = QGridLayout()
        grid_layout.setSpacing(10)
        
        font = QFont("Consolas", 24, QFont.Bold)
        
        for row_idx, row_str in enumerate(matrix):
            start_idx = row_idx * 11
            for col, char in enumerate(row_str):
                lbl = QLabel(char)
                lbl.setFont(font)
                lbl.setAlignment(Qt.AlignCenter)
                
                # Calculate index
                if row_idx % 2 == 0:
                    idx = start_idx + col
                else:
                    idx = start_idx + 10 - col
                    
                self.matrix_labels[idx] = lbl
                grid_layout.addWidget(lbl, row_idx, col)
                
        layout.addLayout(grid_layout)
        
        # Minute Dots
        dots_layout = QHBoxLayout()
        dots_layout.addSpacerItem(QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum))
        self.dot_labels = {}
        for i in range(4):
            lbl = QLabel("●")
            lbl.setFont(QFont("Consolas", 20))
            self.dot_labels[leds_minutes[i]] = lbl
            dots_layout.addWidget(lbl)
        dots_layout.addSpacerItem(QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum))
        layout.addLayout(dots_layout)
        
        # Initialize time to 00:00
        self._updating = False
        self.slider.setValue(0)
        self.update_leds(0, 0)
        
    def on_slider_changed(self, val):
        if self._updating: return
        self._updating = True
        
        h = val // 60
        m = val % 60
        self.time_edit.setTime(QTime(h, m))
        self.update_leds(h, m)
        
        self._updating = False
        
    def on_time_edit_changed(self, time):
        if self._updating: return
        self._updating = True
        
        val = time.hour() * 60 + time.minute()
        self.slider.setValue(val)
        self.update_leds(time.hour(), time.minute())
        
        self._updating = False
        
    def update_leds(self, h, m):
        lit, dots = get_lit_leds(h, m)
        
        # Update matrix
        for idx, lbl in self.matrix_labels.items():
            if idx in lit:
                lbl.setStyleSheet("color: #00ff00;")
            else:
                lbl.setStyleSheet("color: #333333;")
                
        # Update dots
        for idx, lbl in self.dot_labels.items():
            if idx in dots:
                lbl.setStyleSheet("color: #00ff00;")
            else:
                lbl.setStyleSheet("color: #333333;")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = WordClockGUI()
    window.show()
    sys.exit(app.exec())
