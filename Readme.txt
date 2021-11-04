■開発者情報
氏　名：相原 康希
連絡先：190283@jc-21.jp

■作品概要
ホームランダービーを行うゲームです。

■起動方法
exec/HomeRunDerby.exe を起動してください。

■操作方法
・ストライクゾーン（画面中央の白い長方形）を左クリックするとバッターがスイングします。
・ピッチャーの投げるボールにタイミングとカーソルを合わせてスイングすると打つことができます

■アピールポイント（プログラムで力を入れたこと）
・物理演算（project/HomeRunDerby/PlayScene/Ball.cpp）
・バッターのアニメーション(project/HomeRunDerby/PlayScene/Batter.cpp)
・ソースコードを変更せずに難易度調整等を可能にする(project/HomeRunDerby/Assets/Data)

■今後の改善点
・namespaceで纏められている部分をクラス化しアクセスを制限する。
・intを使用している部分をshortにするなどして使用メモリを削減する。
・エフェクトやサウンドを追加することでより面白さを体験できるものにする。
・動作内容についてなどの不必要なコメントを削減し可読性を向上する。

■開発期間
8カ月（ただし、最初の6カ月はDirectXの学習及びゲームエンジンの制作）

■開発環境
・VisualStudio2019
・WindowsAPI
・DirectX11
・Maya
・ペイント3D
・AdobeXD

■使用言語
・c++
・HLSL

■その他
下記2ファイルは自作ではなく、 "https://polyhaven.com/a/rocks_ground_08" を利用させて頂きました。
・rocks_ground_08_diff_2k.jpg
・rocks_ground_08_nor_gl_2k.jpg
