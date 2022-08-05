# Atlivu - アトライブ

AviUtl 関係の資源を再利用できないかテストしています。

<!-- AviUtl -> AVIUTL -> ATLIVU -> Atlivu -> アトライブ -> 後来歩 -->

## 事前準備

Visual Studio 2022 のランタイムライブラリが必要です。(x86・x64 両方とも)
https://docs.microsoft.com/ja-jp/cpp/windows/latest-supported-vc-redist?view=msvc-170

## 導入方法

1. 以下のファイルとフォルダを任意のフォルダに配置します。
* 任意のフォルダ
	* Atlivu.exe
	* aui.exe
	* auo.exe
	* Plugins
		* lwinput.aui
		* x264guiEx.auo

Plugins フォルダは AviUtl の Plugins フォルダをそのまま持ってくれば OK。

## 使用方法

1. Atlivu.exe を起動します。

## テストの手順

1. メニューの「ファイル」→「入力プラグインを選択」で aui ファイルを選択します。(今のところ lwinput.aui のみでテスト)
1. メニューの「ファイル」→「入力プラグインの設定」で入力の設定を行います。※設定ウィンドウが背後に出ます。
1. メニューの「ファイル」→「出力プラグインを選択」で auo ファイルを選択します。(今のところ x264guiEx.auo のみでテスト)
1. メニューの「ファイル」→「出力プラグインの設定」で出力の設定を行います。※設定ウィンドウが背後に出ます。
1. メニューの「ファイル」→「メディアファイルを開く」で動画ファイルを選択します。(入力プラグインで読み込めるもの)
1. トラックーバーを動かしてシークができるかテストします。
1. 「再生」ボタンを押して再生できるかテストします。(正常動作すれば音声も出ます)
1. メニューの「ファイル」→「メディアファイルを保存」で動画ファイルを保存できるかテストします。

## 入力の流れ

Atlivu.exe (64bit UNICODE) -> aui.exe (32bit UNICODE) -> AviUtl 入力プラグイン (32bit マルチバイト)<br>
プロセス間通信にウィンドウメッセージと名前付きパイプを使用。<br>

### 初期化

1. 名前付きパイプ "\\\\.\\pipe\\aui%d" を作成する。"%d" はホスト側のスレッド ID。
2. ホスト側のウィンドウハンドルを引数にして aui.exe を起動する。（例 : aui.exe 0x12345678)

### 入力プラグインのロード

1. aui.exe のスレッドに Input::CommandID::LoadPlugin をポストする。
2. TCHAR[TextMaxSize] 型のファイル名をパイプに書き込む。
3. int32_t 型の結果をパイプから読み込む。

### 入力プラグインのアンロード

1. aui.exe のスレッドに Input::CommandID::UnloadPlugin をポストする。
2. int32_t 型の結果をパイプから読み込む。

### 入力プラグインのコンフィグ

1. aui.exe のスレッドに Input::CommandID::Config をポストする。
2. int32_t 型の結果をパイプから読み込む。

### メディアファイルのオープン

1. aui.exe のスレッドに Input::CommandID::OpenMedia をポストする。
2. TCHAR[TextMaxSize] 型のファイル名をパイプに書き込む。
3. int32_t 型のメディアハンドルをパイプから読み込む。

### メディアファイルのクローズ

1. aui.exe のスレッドに Input::CommandID::CloseMedia をポストする。
2. int32_t 型のメディアハンドルをパイプに書きこむ。
3. int32_t 型の結果をパイプから読み込む。

### メディア情報の取得

1. aui.exe のスレッドに Input::CommandID::GetMediaInfo をポストする。
2. int32_t 型のメディアハンドルをパイプに書きこむ。
3. MediaInfo 型のメディア情報をパイプから読み込む。

### 映像の取得

1. aui.exe のスレッドに Input::CommandID::ReadVideo をポストする。
2. int32_t 型のメディアハンドルをパイプに書きこむ。
3. int32_t 型のフレーム番号をパイプに書きこむ。
4. int32_t 型のバッファサイズをパイプから読み込む。
5. BYTE[バッファサイズ] 型の映像バッファをパイプから読み込む。

### 音声の取得

1. aui.exe のスレッドに Input::CommandID::ReadAudio をポストする。
2. int32_t 型のメディアハンドルをパイプに書きこむ。
3. int32_t 型の開始サンプルをパイプに書きこむ。
4. int32_t 型のサンプル数をパイプに書きこむ。
5. int32_t 型の実際に読み込むサンプル数をパイプから読み込む。
6. 実際に読み込むサンプル数からバッファサイズを算出する。
7. BYTE[バッファサイズ] 型の音声バッファをパイプから読み込む。

## 出力の流れ

Atlivu.exe (64bit UNICODE) -> auo.exe (32bit UNICODE) -> AviUtl 出力プラグイン (32bit マルチバイト)<br>
プロセス間通信にウィンドウメッセージと名前付きパイプを使用。<br>

### 初期化

1. 名前付きパイプ "\\\\.\\pipe\\auo%d" を作成する。"%d" はホスト側のスレッド ID。
2. ホスト側のウィンドウハンドルを引数にして auo.exe を起動する。（例 : auo.exe 0x12345678)

### 出力プラグインのロード

1. auo.exe のスレッドに Output::CommandID::LoadPlugin をポストする。
2. TCHAR[TextMaxSize] 型のファイル名をパイプに書き込む。
3. int32_t 型の結果をパイプから読み込む。

### 出力プラグインのアンロード

1. auo.exe のスレッドに Output::CommandID::UnloadPlugin をポストする。
2. int32_t 型の結果をパイプから読み込む。

### 出力プラグインのコンフィグ

1. auo.exe のスレッドに Output::CommandID::Config をポストする。
2. int32_t 型の結果をパイプから読み込む。

### ファイルの保存 (エンコード)

1. auo.exe のスレッドに Output::CommandID::SaveFile をポストする。
2. TCHAR[TextMaxSize] 型のファイル名をパイプに書き込む。
3. MediaInfo 型のメディア情報をパイプに書き込む。
4. auo.exe 側からパイプデータが送られてくるのでそれを読み込む。

### パイプデータの終了

1. Output::CommandID::End を読み込む。
2. パイプデータの読み込みを終了する。

### 保存処理の中止

1. Output::CommandID::IsAbort を読み込む。
2. int32_t 型のデータをパイプに書きこむ。保存を中止するなら 0 以外を書きこむ。

### 残り時間の表示

1. Output::CommandID::RestTimeDisp を読み込む。
2. int32_t 型の現在フレームをパイプから読み込む。
3. int32_t 型のフレーム総数をパイプから読み込む。

### プレビューの更新

1. Output::CommandID::UpdatePreview を読み込む。
2. 必要ならプレビューを更新する。

### 映像バッファを渡す

1. Output::CommandID::GetVideo を読み込む。
2. int32_t 型のフレーム番号をパイプから読み込む。
3. int32_t 型のフォーマットをパイプから読み込む。
4. int32_t 型のバッファサイズをパイプに書き込む。
5. BYTE[バッファサイズ] 型の映像バッファをパイプに書き込む。

### 音声バッファを渡す

1. Output::CommandID::GetAudio を読み込む。
2. int32_t 型の開始サンプルをパイプから読み込む。
3. int32_t 型のサンプル数をパイプから読み込む。
4. int32_t 型のバッファサイズをパイプに書き込む。
5. BYTE[バッファサイズ] 型の音声バッファをパイプに書き込む。

## シーク

* 映像を取得してビットマップを描画。
* 音声波形を取得して折れ線グラフを描画。

## 再生

* シーク処理に加えて音声を再生。WaveOut を使用。

## 未解決

* プラグインの設定ウィンドウが後ろに行ってしまう。
* エンコード時、音声の長さがおかしいときがある。

## 予定

* 32bit 版と 64bit 版の入出力インターフェイスを定義する。
	* 当面は 32bit 版インターフェイスで aui.exe と auo.exe を使用する。
	* 入出力プラグイン製作者の方が 64bit 版インターフェイスに対応してくれたらそれを直接使用する。<br>
	これにより入出力プラグイン側のメモリ空間が 64bit に拡張され、エンコードの安定性が増す。<br>
	さらにプロセス間通信のオーバーヘッドがなくなって処理速度が多少向上する。(AviUtl と同等にまで戻る)<br>
* サブプロセスの exe ファイルはユーザーが誤って実行しないように拡張子を変えたほうがいいかもしれない。
* タイムラインの UI 制作をやってみたい。

## 更新履歴

* 2.0.0 - 2022/08/06 入出力を整理
* 1.0.1 - 2022/07/28 ロケールを設定するように修正
* 1.0.0 - 2022/07/16 入力プラグインと出力プラグインが使えるかテスト

## 作成者情報
 
* 作成者 - 蛇色 (へびいろ)
* GitHub - https://github.com/hebiiro
* Twitter - https://twitter.com/io_hebiiro

## 免責事項

この作成物および同梱物を使用したことによって生じたすべての障害・損害・不具合等に関しては、私と私の関係者および私の所属するいかなる団体・組織とも、一切の責任を負いません。各自の責任においてご使用ください。
