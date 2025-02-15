# PLATEAU Mapbox Terrain Converter実行ファイル

このフォルダのソースコードは[libplateauterrainrgb](https://github.com/pacificspatial/plateau-mb-terrain-converter/tree/main/libplateauterrainrgb)ライブラリを利用して変換を行う実行ファイルのソースコードです。
以下に、各実行ファイルのコマンドラインに付いて説明します。

## convertTerrainToMapboxRGB

#### コマンドライン
```
convertTerrainToMapboxRGB Usage:
convertTerrainToMapboxRGB [(options)] [input file path(CityGML)] [output directory]
options :
    --min_zoom [zoom no] : minimum zoom level (default = 6)
    --max_zoom [zoom no] : maximum zoom level (default = 15)
    --overwrite : overwrite existing tiles
    --logfile [filename] : specify output log file
```
PLATEAU DEMファイル（CityGML）ファイルからMapbox-RGB形式のタイルセットを生成します。

#### 引数
- 入力ファイルへのパス
  - 入力ファイルはPLATEAU DEMのCityGMLファイル（TIN形式）である必要があります。
- 出力ディレクトリ
  - Mapbox-RGB形式のタイルセットを出力するディレクトリを指定します。出力ディレクトリにすでにタイルが存在している場合は追記されます。追記の動作は下記の`--overwrite`オプションによります。

#### オプション
- --min_zoom
  最小ズームレベルを指定します。省略すると最小ズームレベルは６となります。
- --max_zoom
  最大ズームレベルを指定します。省略すると最大ズームレベルは１５となります。
- --overwrite
  出力ディレクトリにタイルセットがすでに存在している場合にこのオプションを指定すると、すでに存在しているタイルに対して入力ファイルがカバーする範囲を上書きします。このオプションを指定しない場合はすでに存在しているタイルのNoDataの部分に対して入力ファイルがカバーする範囲を追記します。
- --logfile [ファイル名]
  指定したファイルにログを出力します。すでに存在しているファイルを指定した場合は末尾に追記します。

ここでNoDataは出力タイル画像のうちアルファチャンネルの値が0のピクセルを指します。

## convertGsiDemToMapboxRGB

#### コマンドライン
```
convertGsiDemToMapboxRGB Usage:
convertGsiDemToMapboxRGB [(options)] [input file path(.xml)] [output directory]
options :
    --min_zoom [zoom no] : minimum zoom level (default = 6)
    --max_zoom [zoom no] : maximum zoom level (default = 15)
    --overwrite : overwrite existing tiles
    --logfile [filename] : specify output log file
```
基盤地図情報標高ファイル（GML）ファイルからMapbox-RGB形式のタイルセットを生成します。

#### 引数
- 入力ファイルへのパス
  - 入力ファイルは基盤地図情報GML形式である必要があります。
  - 本プログラムの動作は基盤地図情報標高10mメッシュ-地形図の等高線（10B）で確認しています。それ以外の標高データに関しては一部未検証です。

- 出力ディレクトリ
  - Mapbox-RGB形式のタイルセットを出力するディレクトリを指定します。出力ディレクトリにすでにタイルが存在している場合は追記されます。追記の動作は下記の`--overwrite`オプションによります。

#### オプション
- --min_zoom
  最小ズームレベルを指定します。省略すると最小ズームレベルは６となります。
- --max_zoom
  最大ズームレベルを指定します。省略すると最大ズームレベルは１５となります。
- --overwrite
  出力ディレクトリにタイルセットがすでに存在している場合にこのオプションを指定すると、すでに存在しているタイルに対して入力ファイルがカバーする範囲を上書きします。このオプションを指定しない場合はすでに存在しているタイルのNoDataの部分に対して入力ファイルがカバーする範囲を追記します。
- --logfile [ファイル名]
  指定したファイルにログを出力します。すでに存在しているファイルを指定した場合は末尾に追記します。

## merge_tilesets

#### コマンドライン
```
merge_tilesets Usage:
merge_tilesets [(options)] [input directory [input directory] ...]
options :
    --outdir [directory] : output directory
    --overwrite : overwrite existing tiles
```

引数で指定された複数のディレクトリにあるタイルセットをマージして指定したディレクトリに出力します。

#### 引数
- 入力ディレクトリ（複数指定可）

#### オプション
- --outdir (required)
  出力ディレクトリ

## fill_zero_tileset

#### コマンドライン
```
fill_zero_tileset Usage:
fill_zero_tileset [input directory] [(option)output directory]
```
引数で指定されたディレクトリ内のタイル画像に対して、NoData（アルファチャンネルが0のピクセル）を0m（RGBA=[1,134,160,255]）に置換します。

#### 引数
- 対象ディレクトリ
- 出力ディレクトリ（オプション）
  出力ディレクトリを省略すると対象のディレクトリの画像を上書きします。
