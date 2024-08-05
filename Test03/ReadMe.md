# CG2 評価課題3

<div style = "text-align: right;">
LE2A_08_ オリハラ_イッセイ
</div>

## 操作

* ### cameraについて 
ImGuiの "DebugCameraWindow" で変更するか，  
マウスで操作する場合は  
[ ctl ] + マウス右で回転  
[ ctl ] + マウス左で移動  
の操作ができる．

* ### ImGui の各Windowについて

 * ### FileList

   - TextureFilesを開くと <./reosurce> フォルダー内の png が表示され，  
それぞれのファイル名を左クリックすることでSpriteが表示される  
( ロード時間が長いとuvChecker が表示されることがあるが，バグではない )
  
   - ModelList を開くと<./reosurce> フォルダー内の .obj が表示され，
 それぞれのファイル名を左クリックすると ModelObject が生成される．  
 ( 読み込み時間が長いとしばらく表示されない時間がある可能性があるが，バグではない)
   - [  CreateSphere  ] ボタンで Sphereオブジェクトを 生成できる
  ( テクスチャーなし )

* ### MaterialManager
  - MaterialName に 作成したい マテリアルの名前を入力し，  
  [ Create ] ボタンを押すことで 生成される
  - 各マテリアルネームの TreeNode 内で それぞれのマテリアル を編集する (Sprite はこれに含まれない )

* ### DebugCamera
  - デバッグカメラの rotate,translate を確認できる

* ### #numbers 
  - 各オブジェクト を編集できる

* ### Light
  - direction やカラーを変更できる

## 加点要素 ＆ やったこと

* 球の描画
* LambertianReflectance
* HalfLambert
* UVTransform
* 複数モデルの描画
* UtahTeapot の描画
* StanfordBunny の描画
* MultiMeshの対応
* MultiMaterialの対応
* Suzanne の描画
* Lighting方式の変更  

* obj や png の読み込みのバックグラウンド化
* PhongReflectionModelの実装
* BlendMode の追加(Editorでは全てAlphaに設定)
