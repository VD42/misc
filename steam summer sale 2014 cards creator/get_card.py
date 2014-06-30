# -*- coding: utf-8 -*- 

import tempfile
import urllib
import urllib2
import os
from PIL import Image, ImageDraw, ImageFont

def get_games():
    return get_games_main()

def get_games_flash():
    page = urllib.urlopen('http://store.steampowered.com/?cc=us')
    content = page.read()
    content = content[content.find('id="summersale_flashsales"'):]
    content = content[:content.find('<div style="clear: left;"></div>')]
    urls = []
    while content.find('<a class="summersale_dailydeal') > -1:
        content = content[content.find('<a class="summersale_dailydeal'):]
        content = content[content.find('href="') + 6:]
        url = content[:content.find('"')]
        urls.append({'url': url})
    return urls
    
def get_games_choose():
    page = urllib.urlopen('http://store.steampowered.com/?cc=us')
    content = page.read()
    content = content[content.find('<div class="communitychoice_content">'):]
    content = content[:content.find('<div style="clear: left;"></div>')]
    urls = []
    while content.find('<a class="summersale_dailydeal') > -1:
        content = content[content.find('<a class="summersale_dailydeal'):]
        content = content[content.find('href="') + 6:]
        url = content[:content.find('"')]
        urls.append({'url': url})
    return urls
    
def get_games_main():
    page = urllib.urlopen('http://store.steampowered.com/?cc=us')
    content = page.read()
    content = content[content.find('<div class="summersale_dailydeals">'):]
    content = content[:content.find('<div class="encore_board_ctn">')]
    urls = []
    while content.find('<a class="summersale_dailydeal') > -1:
        content = content[content.find('<a class="summersale_dailydeal'):]
        content = content[content.find('href="') + 6:]
        url = content[:content.find('"')]
        urls.append({'url': url})
    return urls

path = tempfile.mkdtemp('sscc4')
print('Getting list...')
games = get_games()
mode = 4
if len(games) > 4:
    mode = 9
if mode == 4:
    main_image = Image.new('RGBA', (585, 273), (255, 255, 255, 255))
elif mode == 9:
    # main_image = Image.new('RGBA', (878, 410), (255, 255, 255, 255))
   main_image = Image.new('RGBA', (1171, 1369), (255, 255, 255, 255))
text_list = '<ul>\n'
map = '<map name="[ИЗМЕНИТЕ ИМЯ]">\n'
for i in xrange(len(games)):
    print('Getting game ' + games[i]['url'] + '...')
    if games[i]['url'].find('/app/') > -1:
        id = games[i]['url'][games[i]['url'].find('/app/') + 5:-1]
        games[i]['id'] = id
        games[i]['sub'] = False
    elif games[i]['url'].find('/sub/') > -1:
        id = games[i]['url'][games[i]['url'].find('/sub/') + 5:-1]
        games[i]['id'] = id
        games[i]['sub'] = True
    else:
        continue
    request = urllib2.Request(games[i]['url'] + '?cc=ru')
    request.add_header('Cookie', 'birthtime=623314801')
    page = urllib2.urlopen(request)
    content = page.read()
    if not games[i]['sub']:
        if content.find('<div class="apphub_AppName">') > -1:
            content = content[content.find('<div class="apphub_AppName">') + 28:]
            title = content[:content.find('</div>')]
            games[i]['title'] = title.strip()
    else:
        if content.find('<div class="game_name">') > -1:
            content = content[content.find('<div class="game_name">'):]
            content = content[content.find('<div class="blockbg">') + 21:]
            title = content[:content.find('</div>')]
            games[i]['title'] = title.strip()
    if content.find('<div class="discount_block game_purchase_discount">') > -1:
        content = content[content.find('<div class="discount_block game_purchase_discount">'):]
        content = content[content.find('<div class="discount_pct">') + 26:]
        sale = content[:content.find('</div>')]
        content = content[content.find('<div class="discount_final_price" itemprop="price">') + 51:]
        price = content[:content.find(' ')]
        games[i]['sale'] = sale
        games[i]['price'] = price
    if not ('title' in games[i]):
        request = urllib2.Request(games[i]['url'] + '?cc=us')
        request.add_header('Cookie', 'birthtime=623314801')
        page = urllib2.urlopen(request)
        content = page.read()
        if not games[i]['sub']:
            if content.find('<div class="apphub_AppName">') > -1:
                content = content[content.find('<div class="apphub_AppName">') + 28:]
                title = content[:content.find('</div>')]
                games[i]['title'] = title.strip()
        else:
            if content.find('<div class="game_name">') > -1:
                content = content[content.find('<div class="game_name">'):]
                content = content[content.find('<div class="blockbg">') + 21:]
                title = content[:content.find('</div>')]
                games[i]['title'] = title.strip()
    print('Downloading game picture...')
    if games[i]['sub']:
        picture_page = urllib.urlopen('http://cdn.highwinds.steamstatic.com/steam/subs/' + games[i]['id'] + '/header_292x136.jpg')
    else:
        picture_page = urllib.urlopen('http://cdn.highwinds.steamstatic.com/steam/apps/' + games[i]['id'] + '/header_292x136.jpg')
    picture_file = open(os.path.join(path, games[i]['id'] + '.jpg'), 'wb')
    picture_file.write(picture_page.read())
    picture_file.close()
    image = Image.open(os.path.join(path, games[i]['id'] + '.jpg')).resize((292, 136), Image.ANTIALIAS)
    price_font = ImageFont.truetype('verdana.ttf', 20, encoding='unic')
    if 'price' in games[i]:
        price_text = unicode(games[i]['price'] + ' руб. (' + games[i]['sale'] + ')', 'utf-8')
    else:
        price_text = unicode('недоступна в России', 'utf-8')
    price_size = price_font.getsize(price_text)
    price_image = Image.new('RGBA', (price_size[0] + 10, price_size[1] + 9), (0, 0, 0, 216))
    price_draw = ImageDraw.Draw(price_image)
    price_draw.text((5, 3), price_text, font=price_font, fill='#fec900')
    image.paste(price_image, (image.size[0] - price_image.size[0], image.size[1] - price_image.size[1]), price_image)
    if mode == 4:
        if i == 0:
            main_image.paste(image, (0, 0))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="0,0,292,136" />\n'
        elif i == 1:
            main_image.paste(image, (293, 0))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="293,0,585,136" />\n'
        elif i == 2:
            main_image.paste(image, (0, 137))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="0,137,292,273" />\n'
        elif i == 3:
            main_image.paste(image, (293, 137))
    elif mode == 9:
      row = int(i / 4) 
      column = i % 4
      main_image.paste(image, (293 * column, 137 * row))
    """
        if i == 0:
            main_image.paste(image, (0, 0))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="0,0,292,136" />\n'
        elif i == 1:
            main_image.paste(image, (293, 0))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="293,0,585,136" />\n'
        elif i == 2:
            main_image.paste(image, (586, 0))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="586,0,878,136" />\n'
        elif i == 3:
            main_image.paste(image, (0, 137))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="0,137,292,273" />\n'
        elif i == 4:
            main_image.paste(image, (293, 137))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="293,137,585,273" />\n'
        elif i == 5:
            main_image.paste(image, (586, 137))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="586,137,878,273" />\n'
        elif i == 6:
            main_image.paste(image, (0, 274))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="0,274,292,566" />\n'
        elif i == 7:
            main_image.paste(image, (293, 274))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="293,274,585,566" />\n'
        elif i == 8:
            main_image.paste(image, (586, 274))
            map += '<area alt="' + games[i]['title'] + '" href="' + games[i]['url'] + '" shape="rect" coords="586,274,878,566" />\n'
    """
    if 'price' in games[i]:
        text_list += '<li><a href="' + games[i]['url'] + '">' + games[i]['title'] + '</a> — ' + games[i]['price'] + ' руб. (' + games[i]['sale'] + ');</li>\n'
    else:
        text_list += '<li><a href="' + games[i]['url'] + '">' + games[i]['title'] + '</a> — недоступна в России;</li>\n'
text_list += '</ul>'
map += '</map>\n'
main_image.save('output.png', 'PNG')
open('output.txt', 'w').write(text_list)
open('map.txt', 'w').write(map)
        
    

