# M5StackImageWatch
![image](https://github.com/mongonta0716/M5StackImageWatch/blob/master/jpg/image1.jpg)
![image](https://github.com/mongonta0716/M5StackImageWatch/blob/master/jpg/image2.jpg)
![image](https://github.com/mongonta0716/M5StackImageWatch/blob/master/jpg/image3.jpg)

 PNGファイルから作成したデータを背景にして利用できるアナログ時計です。

 Analog clock for M5StickC whose background can be changed with PNG file data

## ●環境(Environment)
- Arduino IDE 1.8.10
- Arduino-esp32 1.0.4
- M5Stack Library 0.2.9
- M5StackUpdater 

## ●設定とコンパイル(Settings and Compile)
 ImageWatch.inoのInitialSettingsを書き換えてコンパイルしてください。

 Set the ImageWatchC.ino InitialSettings area according to your environment and compile.
### 必須項目(Required Settings)
起動時にWiFi接続してNTPから時刻を取得する必要があるため下記の設定は必須です。

This app synchronizes time with NTP via WiFi connection at startup. Therefore, be sure to set up WiFi and NTP.
- WiFi SSID
- WiFi Password
- URL of NTP Server

## ●使い方(Usage)
 起動時にWiFiに接続してNTPと時刻を同期します。BtnA,BtnB,BtnCを押すと背景画像を切り替えることができます。

 This app synchronizes time with NTP via WiFi connection at startup. Press BtnA, BtnB, BtnC to switch the background image.

## ●画像の入れ替え方法(How to change image)
 320x240のPNGファイルを自分で作成し、コンパイルすると背景画像を入れ替えることができます。

1. image.png.hを作成する。
2. image.pngをimage.png.hに変換
3. ソースファイルを編集し置き換える。
4. コンパイル

 If you create a 320x240 PNG file yourself and compile it, you can replace the background image.

1. Create image.png
2. Convert from image.png to image.png.h
3. Replace and Edit image.png.h
4. Compile

### 画像変換 `png2code.py`
[M5Stack_Test_tools](https://github.com/mongonta0716/M5Stack_Test_tools)のpng2code.pyを使って、変換してください。

Refer to [M5Stack_Test_tools](https://github.com/mongonta0716/M5Stack_Test_tools) and convert it using `png2code.py`.

# Licence
[MIT](https://github.com/mongonta0716/M5StackImageWatch/blob/master/LICENSE)

# Author
[Takao Akaki](https://twitter.com/mongonta555)
