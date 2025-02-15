
# PLATEAU Mapbox Terrain Converter

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

#### 3-2-1. CMakeコマンド

```
$ mkdir build (Windowsの場合はmd build)
$ cd build
$ cmake ..
$ cmake --build . --config Release
$ cmake --install .
```

