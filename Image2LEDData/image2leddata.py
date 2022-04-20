from PIL import Image

# 読み込む画像ファイル名
BMPIMG_FILE_NAME = "input.bmp"

# エクスポート先のファイル名
EXPORT_FILE_NAME = "export.txt"

# イメージを読み込む
img = Image.open(BMPIMG_FILE_NAME)
width, height = img.size

export_str = "" # LEDデータを格納する文字列

# 左上から縦に読み出す（縦=y軸, 横=x軸）
for x in range(width):
    for y in range(height):
      # LEDMatrixはくねくねしてるため、奇数列で反転する
        if x % 2 == 0:
            local_y = y
        else:
            local_y = height - 1 - y

        # 対象のピクセルの値を取得
        pix = img.getpixel((x,local_y))
        r = pix[0] # Red
        g = pix[1] # Green
        b = pix[2] # Blue

        # このピクセルの情報を文字列に追加
        export_str += hex(r)[2:].zfill(2)
        export_str += hex(g)[2:].zfill(2)
        export_str += hex(b)[2:].zfill(2)


# 標準出力
print(export_str)

# ファイル書き出し
with open(EXPORT_FILE_NAME, 'w', encoding='utf-8') as f:
    f.write(export_str)
