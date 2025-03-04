# Pythonサンプルコード

本フォルダはlibplateauterrainrgbライブラリをPythonから利用するためのサンプルコードを格納しています。

### コールバック関数について

SWIGの仕様の関係でコールバック関数については以下のように実装する必要があります。
まず、`PMTCFeedback`クラスのサブクラスを作成して②つのコールバック関数を実装します。以下は実装例です。

```python
class myFeedback(PMTCFeedback):

    def __init__(self):
        PMTCFeedback.__init__(self)

    def messageFeedback(self, eStatus, strMessage):
        if eStatus == MESSAGE_ERROR:
            print("ERROR : " + strMessage, file=sys.stderr)
        else:
            print(strMessage)

    def progressFeedback(self, nProgress):
        sys.stdout.write(str(nProgress) + '\r')
        sys.stdout.flush()
```
このクラスのオブジェクトを作成し、各関数のfeedback引数へ渡してください。

## API説明

以下に、各関数について説明します。

### CreatePlateauTileset

```python
def CreatePlateauTileset(
  strInputTerrainCityGML : str, 
  strOutputTileDirectory : str, 
  nMinZoomLevel : int, 
  nMaxZoomLevel : int, 
  bOverwrite : bool, 
  pFeedback : myFeedback) -> bool:
```

PLATEAU DEMのCityGMLファイル（TIN形式）からMapbox-RGB形式のタイルセットを作成します。
出力ディレクトリにすでにタイルが存在している場合は`bOverwrite`引数によって以下のように追記されます。
- true
  - すでに存在しているタイルに対して入力ファイルがカバーする範囲を上書きします。
- false
  - すでに存在しているタイルのNoDataの部分に対して入力ファイルがカバーする範囲を追記します。

ここでNoDataは出力タイル画像のうちアルファチャンネルの値が0のピクセルを指します。

#### 引数
- strInputTerrainCityGML : 入力ファイル名（gml）
- strOutputTileDirectory : 出力ディレクトリ
- nMinZoomLevel : 最小ズームレベル
- nMaxZoomLevel : 最大ズームレベル
- bOverwrite : 上書きモード
- pFeedback : コールバックオブジェクト（上記参照）
#### 戻り値
- true : 変換成功
- false : 変換失敗

### CreateGsiTileset
```python
def CreateGsiTileset(
  strInputGsiGml : str, 
  strOutputTileDirectory : str, 
  nMinZoomLevel : int, 
  nMaxZoomLevel : int, 
  bOverwrite : bool, 
  pFeedback : myFeedback) -> bool:
```
基盤地図情報標高データ（.xml）ファイルからMapbox-RGB形式のタイルセットを作成します。それ以外の動作は`createPlateauTileset()`関数と同様です。
標高データ中の「データなし」（-9999.0）の点はNoDataとして取り扱われます。

_Note:_
本関数の動作は基盤地図情報標高10mメッシュ-地形図の等高線（10B）で確認しています。それ以外の標高データに関しては一部未検証です。

#### 引数
- strInputGsiGml : 入力ファイル名（gml）
- strOutputTileDirectory : 出力ディレクトリ
- nMinZoomLevel : 最小ズームレベル
- nMaxZoomLevel : 最大ズームレベル
- bOverwrite : 上書きモード
- pFeedback : コールバックオブジェクト（上記参照）
#### 戻り値
- true : 変換成功
- false : 変換失敗

## MergeTilesets
```python
def MergeTilesets(
  vstrInputDirs : List[str], 
  strOutDir : str, 
  bOverwrite : bool, 
  pFeedback : myFeedback) -> None:
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
- pFeedback : コールバックオブジェクト（上記参照）

## Fill_zero
```python
def Fill_zero(
  strTileDir : str, 
  pFeedback : myFeedback) -> None:
```
指定したディレクトリ以下にあるタイル画像のNoDataのピクセルを0m（RGBA=[1,134,160,255]）に置換します。置換は上書きで行われます。

#### 引数
- strTileDir : 入力ディレクトリ
- pFeedback : コールバックオブジェクト（上記参照）

