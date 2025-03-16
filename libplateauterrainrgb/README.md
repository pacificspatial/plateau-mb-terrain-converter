# libplateauterrainrgb

本ライブラリは[PLATEAU](https://www.mlit.go.jp/plateau/)が提供するCityGML形式の地形データ、および[基盤地図情報標高](https://www.gsi.go.jp/kiban/)ファイルを[Mapbox Terrain-RGB](https://docs.mapbox.com/ja/data/tilesets/reference/mapbox-terrain-rgb-v1/)に変換する機能を提供するライブラリです。
具体的な使用例については[tools](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/tools)フォルダ内のソースコードを参照してください。Pythonでの使用法については[python_example](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/python_example)フォルダ内のソースコードを参照してください。
以下に、本ライブラリが提供するAPIの説明を示します。

## createPlateauTileset
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
 CityGML形式のPLATEAU地形モデル（TIN）からMapbox-Terrain RGB形式のタイルセットを作成します。
出力ディレクトリにすでにタイルが存在している場合は`bOverwrite`引数によって以下のように追記されます。
- true
  - すでに存在しているタイルに対して入力ファイルがカバーする範囲を上書きします。
- false
  - すでに存在しているタイルのNoDataの部分に対して入力ファイルがカバーする範囲を追記します。

ここでNoDataは出力タイル画像のうちアルファチャンネルの値が0のピクセルを指します。

例として、左の既存のタイルに右のピクセルを追加するとき、`bOverwrite`引数によって以下のように画像が作成されます。
![orig image](../img/orig_tile.png)

| bOverwriteの値 |                 結果                 |
| :------------: | :----------------------------------: |
|     false      | ![false](../img/overwrite-false.png) |
|      true      |  ![true](../img/overwrite-true.png)  |

#### 引数
- strInputTerrainCityGML : 入力ファイル名（gml）
- strOutputTileDirectory : 出力ディレクトリ
- nMinZoomLevel : 最小ズームレベル
- nMaxZoomLevel : 最大ズームレベル
- bOverwrite : 上書きモード
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数
#### 戻り値
- true : 変換成功
- false : 変換失敗

## createGsiTileset
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
基盤地図情報標高データ（.xml）ファイルからMapbox-Terrain RGB形式のタイルセットを作成します。それ以外の動作は`createPlateauTileset()`関数と同様です。
標高データ中の「データなし」（-9999.0）の点はNoDataとして取り扱われます。

_Note:_
本関数の動作は基盤地図情報標高10mメッシュ-地形図の等高線（10B）で確認しています。それ以外の標高データに関しては一部未検証です。

#### 引数
- strInputGsiGml : 入力ファイル名（gml）
- strOutputTileDirectory : 出力ディレクトリ
- nMinZoomLevel : 最小ズームレベル
- nMaxZoomLevel : 最大ズームレベル
- bOverwrite : 上書きモード
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数
#### 戻り値
- true : 変換成功
- false : 変換失敗

## mergeTilesets
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

#### 引数
- vstrInputDirs : 入力ディレクトリ群
- strOutDir : 出力ディレクトリ
- bOverwrite : 上書きモード
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数

## fill_zero
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

#### 引数
- strTileDir : 入力ディレクトリ
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数

## terrain2gtif
```c++
namespace pmtc
{
    bool PMTC_DLL terrain2gtif(
        const std::string &strInputTerrainCityGML,
        const std::string &strOutputGTif,
        double dResolutionLon,
        double dResolutionLat,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr
    );
}
```
 CityGML形式のPLATEAU地形モデル（TIN）をGeoTIFFに変換します。出力GeoTIFFのNoDataの値には-9999を割り当てます。

#### 引数
- strInputTerrainCityGML : 入力ファイル名（gml）
- strOutputGTif : 出力ファイル名(tif)
- dResolutionLon : 経度の地上解像度（°）
- dResolutionLat : 緯度の地上解像度（°）
- fnMessageFeedback : メッセージコールバック関数
- fnProgressFeedback : 進捗コールバック関数
#### 戻り値
- true : 変換成功
- false : 変換失敗
