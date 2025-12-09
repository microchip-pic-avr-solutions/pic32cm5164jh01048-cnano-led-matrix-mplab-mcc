from __future__ import print_function
try:
	import sys
	from builtins import bytes, int
	import argparse
	import struct 
	from serial import Serial
	from intelhex import IntelHex
except ImportError:
    sys.exit("""ImportError: You are probably missing some modules.
To add needed modules, run 'python -m pip install -U future pyserial intelhex'""")

import numpy as np


MATRIX_XSIZE = 8
MATRIX_YSIZE = 32
PANEL_XSIZE = 48
PANEL_YSIZE = 32
PANEL_LED_NO = PANEL_XSIZE * PANEL_YSIZE

MATRIX_LED_NO = MATRIX_XSIZE * MATRIX_YSIZE   
PANEL_MATRIX_NO = (PANEL_XSIZE / MATRIX_XSIZE) * (PANEL_YSIZE / MATRIX_YSIZE)   

DISPLAY_PAGE_SIZE = PANEL_XSIZE * PANEL_YSIZE * 3  

FRAME_BUFFERS_OFFSET = 0x8000

class MatrixLastEntry:
    TOP_LEFT = 0
    TOP_RIGHT = 1
    BOTTOM_LEFT = 2
    BOTTOM_RIGHT = 3

class MatrixOrientation:
    HORIZONTAL = 0
    VERTICAL = 1

class MatrixType:
    SERPENTINE = 0
    NORMAL = 1

class PanelChain:
    def __init__(self, index, m_type):
        self.index = index
        self.m_type = m_type

MATRIX_TYPE = MatrixType.SERPENTINE
MATRIX_ORIENTATION = MatrixOrientation.VERTICAL

myPanel = [
    PanelChain(5, MatrixLastEntry.TOP_LEFT),
    PanelChain(4, MatrixLastEntry.BOTTOM_RIGHT),
    PanelChain(3, MatrixLastEntry.TOP_LEFT),
    PanelChain(2, MatrixLastEntry.BOTTOM_RIGHT),
    PanelChain(1, MatrixLastEntry.TOP_LEFT),
    PanelChain(0, MatrixLastEntry.BOTTOM_RIGHT)
]

from PIL import Image
##from snake import snakeInit, snakeStart
##from menu  import mainInit, mainWelcome, gameEnd, antsMain, whiteEdge, pongMain, mchp

import numpy as np
import os

class ws2812_color_t:
    def __init__(self, green, red, blue):
        self.green = green
        self.red = red
        self.blue = blue

class color_type_t:
    BLACK = 0
    RED = 1
    GREEN = 2
    BLUE = 3
    ORANGE = 4
    PINK = 5
    PURPLE = 6
    YELLOW = 7
    WHITE = 8
    TBD = 9

myColors = [
    0, 0, 0,
    0, 0x7F, 0,
    0x7F, 0, 0,
    0, 0, 0x7F,
    0x3F, 0x7F, 0,
    0x28, 0x7F, 0x48,
    0x00, 0x3F, 0x3F,
    0x7F, 0x7F, 0x00,
    0xFF, 0xFF, 0xFF,
    0x7F, 0x00, 0x7F
]

def get_panel_xy_offset(x, y):
    address = 0x00
    panel_position = (int)(x / MATRIX_XSIZE) 
    line_position = x % MATRIX_XSIZE

    if myPanel[panel_position].m_type == MatrixLastEntry.TOP_LEFT:
        address += MATRIX_LED_NO
        address -= y * MATRIX_XSIZE
        if y % 2 == 1:
            address -= MATRIX_XSIZE - line_position
        else:
            address -= line_position + 1
    elif myPanel[panel_position].m_type == MatrixLastEntry.TOP_RIGHT:
        address += y * MATRIX_XSIZE
        if y % 2 == 0:
            address += line_position
        else:
            address += MATRIX_XSIZE - 1 - line_position
    elif myPanel[panel_position].m_type == MatrixLastEntry.BOTTOM_LEFT:
        address += MATRIX_LED_NO
        address -= y * MATRIX_XSIZE
        if y % 2 == 1:
            address -= line_position + 1
        else:
            address -= MATRIX_XSIZE - line_position
    else:  # BOTTOM_RIGHT
        address += y * MATRIX_XSIZE
        if y % 2 == 0:
            address += MATRIX_XSIZE - 1 - line_position
        else:
            address += line_position
    
    address += myPanel[panel_position].index * MATRIX_LED_NO
    return 3 * address


def bmp_to_ws2812_hex(file_path, output_file):
    image = Image.open(file_path).convert('RGB')
    height, width = image.size
    pixels = image.load()
 
    hex_data = np.zeros(DISPLAY_PAGE_SIZE)
 
    for x in range(height):
        for y in range(width):
            r, g, b = pixels[x, y]
            offset = get_panel_xy_offset(x,y)
            # WS2812 expects GRB format
            hex_data[offset]=g
            hex_data[offset+1]=r
            hex_data[offset+2]=b
    
    hex_data = np.uint8(hex_data)

    # Print as hex bytes
    for i in range(0, len(hex_data), 24):  # 8 LEDs per line (24 bytes)
        line = ' '.join(f'0x{byte:02X},' for byte in hex_data[i:i+24])
        output_file.write(line)
        print(line)


output_file = open('output.txt', 'w')

# Example usage for a static image
bmp_to_ws2812_hex('Scripts\images_48x32\scroll_start.bmp', output_file)


output_file.close()
