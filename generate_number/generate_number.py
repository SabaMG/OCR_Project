#Import required library
from PIL import Image, ImageDraw, ImageFont
import os

def make_dir(number):
    for i in range(number):
        os.mkdir(str(i + 1))

def gen_img(width, height, message, size, angle, font, filename):
    img = Image.new('RGB', (width, height), color='white')
    imgDraw = ImageDraw.Draw(img)
    textWidth = imgDraw.textlength(message, font)
    new_box = imgDraw.textbbox((0, 0), message, font)
    new_w = new_box[2] - new_box[0]  # bottom-top
    new_h = new_box[3] - new_box[1]  # right-left]]]]
    xText = (width - new_w) / 2
    yText = (height - new_h) / 2
    if(message == "0"):
        message = ""
    imgDraw.text((xText, yText), message, font=font, fill=(0, 0, 0))
    img = img.rotate(angle, fillcolor='white')
    img.save(filename)


def init_dir(main_dir_name):
    os.mkdir(main_dir_name)
    for i in range(10):
        os.mkdir(main_dir_name + "/" + str(i))

width = 28
height = 28

path = '/home/lucassiauve/Documents/EPITA/Projet_S3_OCR/generate_number/fonts/'
fonts = os.listdir(path)

init_dir("data")

for i in range(10):
    for j in range(15, 30):
        for k in range (-10, 10):
            for font_name in fonts:
                name = font_name[:-4] + "_" + str(j) + "_" + str(k)
                path_f = path + font_name
                f = ImageFont.truetype(path_f, 20)
                gen_img(width, height, str(i), j, k, f, 'data/' + str(i) + '/' + name + '.jpg')

