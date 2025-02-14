
# PLATEAU Mapbox Terrain Converter

## 1．概要

PLATEAU Mapbox Terrain Converterは[PLATEAU](https://www.mlit.go.jp/plateau/)が提供するCityGML形式の地形データを[Mapbox Terrain-RGB](https://docs.mapbox.com/ja/data/tilesets/reference/mapbox-terrain-rgb-v1/)に変換する機能を提供するライブラリです。

## 2．フォルダ構成

- libplateauterrainrgb : 本ライブラリのソースコード
- tools : 本ライブラリを使用する実行ファイルのサンプルコード
- swig : Pythonバインディング向けSWIGインターフェース
- python_example : Pythonサンプルコード

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

## 4．API説明

具体的な使用例については[tools](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/tools)フォルダ内のソースコードを参照してください。Pythonでの使用法については[python_example](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/python_example)フォルダ内のソースコードを参照してください。

### createPlateauTileset
```c++
namespace pmtc
{
    bool PMTC_DLL createPlateauTileset(
        const std::string &strInputTerrainCityGML, 
        const std::string &strOutputTileDirectory, 
        const int nMinZoomLevel, 
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr
    );
}
```
PLATEAU DEMのCityGMLファイル（TIN形式）からMapbox-RGB形式のタイルセットを作成します。
出力ディレクトリにすでにタイルが存在している場合は`bOverwrite`引数によって以下のように追記されます。
- true
  - すでに存在しているタイルに対して入力ファイルがカバーする範囲を上書きします。
- false
  - すでに存在しているタイルのNoDataの部分に対して入力ファイルがカバーする範囲を追記します。

ここでNoDataは出力タイル画像のうちアルファチャンネルの値が0のピクセルを指します。

##### 引数
- strInputTerrainCityGML : 入力ファイル名（gml）
- strOutputTileDirectory : 出力ディレクトリ
- nMinZoomLevel : 最小ズームレベル
- nMaxZoomLevel : 最大ズームレベル
- bOverwrite : 上書きモード
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数
##### 戻り値
- true : 変換成功
- false : 変換失敗

### createGsiTileset
```c++
namespace pmtc
{
    bool PMTC_DLL createGsiTileset(
        const std::string &strInputGsiGml,
        const std::string &strOutputTileDirectory,
        const int nMinZoomLevel,
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr
    );
}
```
基盤地図情報標高データ（.xml）ファイルからMapbox-RGB形式のタイルセットを作成します。それ以外の動作は`createPlateauTileset()`関数と同様です。

_Note:_
本関数の動作は基盤地図情報標高10Bで確認しています。それ以外の標高データに関しては一部未検証です。

##### 引数
- strInputGsiGml : 入力ファイル名（gml）
- strOutputTileDirectory : 出力ディレクトリ
- nMinZoomLevel : 最小ズームレベル
- nMaxZoomLevel : 最大ズームレベル
- bOverwrite : 上書きモード
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数
##### 戻り値
- true : 変換成功
- false : 変換失敗

### mergeTilesets
```c++
namespace pmtc
{
    void PMTC_DLL mergeTilesets( 
        const std::vector<std::string> &vstrInputDirs,
        const std::string& strOutDir, 
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr 
    );
}
```
複数のタイルセットをマージします。重複するタイルが存在する場合は引数`bOverwrite`によって以下のようにマージされます。
- true
  - 引数`vstrInputDirs`で指定されたディレクトリの後ろにあるものが優先されます。
- false
  - 引数`vstrInputDirs`で指定されたディレクトリの前にあるものが優先されます。

##### 引数
- vstrInputDirs : 入力ディレクトリ群
- strOutDir : 出力ディレクトリ
- bOverwrite : 上書きモード
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数

### fill_zero
```c++
namespace pmtc
{
    void PMTC_DLL fill_zero(
        const std::string &strTileDir,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr 
    );
}
```
指定したディレクトリ以下にあるタイル画像のNoDataのピクセルを0m（RGBA=[1,134,160,255]）に置換します。置換は上書きで行われます。

##### 引数
- strTileDir : 入力ディレクトリ
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数
