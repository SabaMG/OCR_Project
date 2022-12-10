#Import required library
from tkinter import *
from tkinter import font

from PIL import Image, ImageDraw, ImageFont
import os

def make_dir(number):
    for i in range(number):
        os.mkdir(str(i + 1))

#def gen_img(width, height, message, size, font):


width = 28
height = 28

message = "Lucas 1 2 3 4 5 6 7 8 9"

#Create a list of font using the font-family constructor
root = Tk()
fonts=list(font.families())
fonts.sort()
print(fonts)

for font_name in fonts:
    the_font = font.Font(family=font_name, size=20)
    img = Image.new('RGB', (width, height), color='white')
    imgDraw = ImageDraw.Draw(img)
    xText = (width - 20) / 2
    yText = (height - 20) / 2
    imgDraw.text((xText, yText), message, font=the_font, fill=(0, 0, 0))
    img.save(message + 'png')
