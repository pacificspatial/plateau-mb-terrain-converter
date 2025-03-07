
# PLATEAU Mapbox Terrain Converter

![banner](img/banner.png)

## 1．概要

PLATEAU Mapbox Terrain Converterは[PLATEAU](https://www.mlit.go.jp/plateau/)が提供するCityGML形式の地形データを[Mapbox Terrain-RGB](https://docs.mapbox.com/ja/data/tilesets/reference/mapbox-terrain-rgb-v1/)に変換する機能を提供するライブラリ、及びそれを用いた実行ファイルです。

## 2．フォルダ構成

- [libplateauterrainrgb](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/libplateauterrainrgb) : 本ライブラリのソースコード
- [tools](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/tools) : 本ライブラリを使用する実行ファイルのサンプルコード
- [swig](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/swig) : Pythonバインディング向けSWIGインターフェース
- [python_example](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/python_example) : Pythonサンプルコード

それぞれの利用方法に関しては各フォルダの説明を参照してください。

## 3．ビルド

### 3-1．必要なソフトウェアと依存ライブラリ

本ライブラリをビルドするには以下のソフトウェアが必要です。
- C++コンパイラ（gcc、Visual C++ etc.）
  - C++17（ISO/IEC 14882:2017）に対応しているもの
- [CMake](https://cmake.org/)
- [SWIG](https://www.swig.org/)

また本ライブラリは以下のライブラリに依存します。
- [GDAL](https://gdal.org/)
  - 3.4.1以上
- [libpng](http://www.libpng.org/pub/png/libpng.html)
  - 1.2以上
- [SQLite](https://www.sqlite.org/)
  - 3.37以上
- [libxml2](https://github.com/GNOME/libxml2)
  - 2.9以上

Mac OS Xの場合はこれらのソフトウェア／ライブラリのインストールを行うために[Homebrew](https://brew.sh/ja/)をインストールする必要があります。

### 3-2．ビルド手順

以下のコマンドでビルド、インストールを行います。

```
$ mkdir build (Windowsの場合はmd build)
$ cd build
$ cmake ..
$ cmake --build . --config Release
$ cmake --install .
```

## 4．機能概要

### 4-1．本ライブラリの機能

本ライブラリは以下の機能を提供します。

-  CityGML形式のPLATEAU地形モデル（TIN）からMapbox-Terrain RGB形式のタイル生成
- 基盤地図情報標高データからMapbox-Terrain RGB形式のタイル生成
- 複数のMapbox-Terrain RGBタイルセットのマージ
  - 複数のプロセスで生成されたタイルセットを1つにまとめる際に利用します。
- タイルセットのNoDataピクセルを0mのピクセルに置換

ライブラリが提供するAPIの詳細については[libplateauterrainrgb](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/libplateauterrainrgb)フォルダのページを参照してください。

[tools](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/tools)フォルダには本ライブラリを利用して変換を行う実行ファイルのソースコードがあります。コマンドラインの説明は上記フォルダ内に記載されています。ソースコードは本ライブラリを利用するためのサンプルコードとして参照してください。

本ライブラリは[SWIG](https://www.swig.org/)を利用してPythonバインディングを作成します。Pythonでの利用方法に関しては[python_example](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/python_example)のソースコードを参照してください。

### 4-2．配信URL

本ツールおよびライブラリを使用して作成した日本全国の地形データを以下のURLにて配信しています。ズームレベルは6～15に対応しています。

`https://api.plateauview.mlit.go.jp/tiles/plateau-terrainrgb/{z}/{x}/{y}.png`

作成手順は以下の通りです。

1. CityGML形式のPLATEAU地形モデル（TIN）が整備されている都市に対して、都市ごとに変換を行い、Mapbox-Terrain RGBタイルセットを作成
2. 1で作成したすべてのMapbox-Terrain RGBタイルセットのマージしたを行う
3. CityGML形式のPLATEAU地形モデル（TIN）が無い箇所を基盤地図情報標高データ（10mメッシュ）で埋めて、日本全国のMapbox-Terrain RGBタイルを作成
4. 海域等に残ったNoDataのピクセルを0mのピクセルで置換

### 4-3．デモサイト

上記の日本全国の地形データを表示するデモサイトを公開しています。

[http://plateau-terrainrgb-demo.s3-website-ap-northeast-1.amazonaws.com/#11.78/35.22809/138.7405/0/75](http://plateau-terrainrgb-demo.s3-website-ap-northeast-1.amazonaws.com/#11.78/35.22809/138.7405/0/75)

## 5．動作環境

本ライブラリおよびソフトウェアは以下の環境での動作を確認しています。
- OS:
  - Windows 10
  - MAC OS X
  - Ubuntu 22.04
- CPU、メモリ、ストレージ等:
  CPUに関しては特に制限はありません。
  メモリに関しては入力ファイルのサイズによって必要な容量が決定します。
  ストレージに関しては入力ファイルの容量に加えて出力されるタイルセットの容量が必要になりますが、これは入力ファイルがカバーしている範囲に依存します。なお、ネットワーク接続は必要ありません。

## 6．ライセンス

- 本ソフトウェアは、MITライセンスのもとで提供されるオープンソースソフトウエアです。
- ソースコードおよび関連ドキュメントの著作権は国土交通省および開発者に帰属します。
- 本ソフトウェアの開発はPacific Spatial Solutions株式会社（[https://pacificspatial.com/](https://pacificspatial.com/)）が行っています。

## 7．注意事項

- 本リポジトリおよびソフトウェアは Project PLATEAU の参考資料として提供しているものです。動作の保証は行っておりません。
- 本リポジトリおよび本ソフトウェアの利用により生じた損失及び損害等について、開発者および国土交通省はいかなる責任も負わないものとします。
- 本リポジトリの内容は予告なく変更・削除する場合があります。
