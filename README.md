# Qt
This project is about my thesis for university graduation.

# ソフトウィア紹介
人の目の代わりに工場の製造ラインを助けよう、組み込みシステムを用いた産業用画像検出システムを設計するきっかけでした。
Qtフレームワークに基づき、C/C++を使って、ソフトウィアを作りました。理由は、組み込みシステムはハードウェアに深い関係がある、ネットワークインターフェース層としてデータ処理、制御に応答速度が凄く要求されます。lwIPスタックを用いた有線ネット通信でカメラからの画像をもらったデータから毎秒30フレームのJPEGデータストリームを抽出することは一番難しいですが、UI設計、シリアルポート通信などを実現し、OpenCVでエッジ検出などの画像処理手法で、パーツの欠陥と長さを検出しました。最終的に目視検査よりも高い精度に達成しました。

# 全体的な紹介
卒業テーマとして組み込みシステムを用いた産業用画像検出システムの設計です。
老齢化社会とともに、人の目では工場でパーツの外観欠陥を検出することに時間がかかり、24時間労働も不可能です。
目の代わりに工場の製造ラインを助けようと思ったのが、そのシステムを設計するきっかけでした。
2016年7月から2017年5月までのほぼ一年間をかかって電子回路、組み込みシステム、ネットワーク通信、ソフトウェア、画像処理などの知識を学んで
ホスト、マスター、欠陥検出三つの部分に作っていきました。
ハードウェアについて、専門家の手法を参考し、回路設計しました。その中で、一番複雑な部分は、176ピンのプロセッサにはんだごてとヒートガンを使ってプリント基板で表面実装することです。
リアルタイムシステムを入れて、プロセス管理を行った上、資源を割り付けます。
マスタは普通のPCですが、有線ネット通信でカメラからの画像を伝送するためにパッケージについて制御します。
OpenCVに基づき、エッジ検出などの画像処理アルゴリズムで、パーツの欠陥と長さを検出しました。

