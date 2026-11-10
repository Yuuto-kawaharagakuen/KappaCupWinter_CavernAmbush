# CAVERN AMBUSH

笠井のカッパ杯ウィンター（GFFコンテスト）応募作品。
砂に潜るバウンティハンターが洞窟の賊のアジトを制圧する、ステルス3Dシューティング。

## 参考資料
- 企画書: `08_笠井友斗_企画書_CAVERN_AMBUSH.pptx`
- タスク表（Notion）: https://app.notion.com/p/3fbdbc3807ce80f29a89ca25d3b94d1c
  - α版: 〜11/16（ステージ1で通しで遊べる。BGM・SE・エフェクトもα版に入れる）、β版: 11/17〜12/27（盾持ち兵・ステージ2・ボス部屋・隊長・ボス＋素材・UI・演出・グレネード周りの音とエフェクト）、提出準備: 12/28〜12/30

## 目標（成績）
- Aランクを狙う
  - B: 動き回って撃って倒す／挙動の違う敵が3種類以上（同じ挙動は1種類とみなす）／アウトゲーム（タイトル・リザルト・クリア画面）があり、ゲームループする
  - A: Bに加えて、アウトゲームに動きをつける（画像を貼るだけにしない）＋プレイヤーの動作にエフェクトと音をつける

## ゲーム仕様の要点
- 浮上中: 一人称（画面には銃のみ）。短射程の砂弾を撃つ
- 潜行中: 三人称。撃てないが敵の弾も当たらない。地中でリロード。敵に近づくと感知される
  - 床（砂）は基本的にどこでも潜れる。オブジェクト（岩・木箱など）の上は潜れない
- プレイヤーの操作は 移動・視点 に加えて「潜行／浮上」「リーン」「リロード」「射撃」のみ。空は飛べない
  - リロードは潜行中だけボタンで行える（浮上中は不可）。1発ずつ補充され、浮上すると中断（入った分は残る）
  - ボタン: 潜行／浮上 B、射撃 RB2、スコープをのぞく LB2（押している間。ズーム1.5倍・視点と移動が遅くなる）、リロード X、リーン LB1／RB1
- ステージ1 → ステージ2（隊長がカギを落とす）→ ボス部屋（ジェットパックで浮遊するボス）
  - 隊長: 雑魚兵の基本の動きに加えて、グレネードと物陰からのリーン撃ちを使う（雑魚兵と別種類として数えるため）
  - 雑魚兵はグレネードを使わない
  - ボス: 基本ずっと空中に浮いてライフルで撃つ。たまにグレネードを連続で投げる。プレイヤーはそれまでと同じくオブジェクトに隠れ、潜行も使いながら下から撃つ
- 敵: 雑魚兵 / 盾持ち兵（正面からの弾を無効化）/ 隊長 / ボス（賊のボス。賞金首ではない）
  - 敵は4種類とも入れる（3種類以上がBランクの条件）
- HPが0でゲームオーバー、ボス撃破でクリア

## 作業ルール
- 新規ファイルは UTF-8（BOM付き）・改行CRLF で作る
- コードに注釈コメントは基本書かない
- 操作はコントローラーのみ（g_pad）。キーボード・マウスは使わない
- コードを書いたら、何を実装したか・作った関数と役割を簡潔に説明する
- コードを書いたら Debug|x64 でビルドしてエラーがないことを確認してから渡す
- プレイ確認と素材探しは笠井が行う。アニメーションの実装は任されることがある
- Notion のタスクは、笠井の「OK」が出てからステータスを「完了」にする
- Notion の子タスクは 1日3コマ（午前 9:00〜12:00／午後 13:00〜17:00／夜 19:00〜22:00）で時刻付きの期日を入れる。作業2日ごとに何も入れない予備日を1日置く（10/25 から3日おき）。タスクが予備日をまたがないように組む
- Notion の親タスク（サブアイテムを持つタスク）の期日は「最後のサブアイテムの終了日 + 1日」にする
- Notion のタスク名は「（主語：）何を・どうする」＋「〜の実装／〜の機能追加／〜の制作／〜の組み込み」で書き、ページ内に「やること」「完了の目安」を書く
- コミット: タスクにOKが出たとき、または指示があったとき（日本語のメッセージ）
- Push: 指示があったとき、または一日の作業が終わるとき
- 一日の作業が終わったら、下の「作業ログ」にその日やったことを追記し、不要になったメモリを掃除してから Push する

## 環境
- エンジン: k2Engine（C++ / DirectX12）。`k2Engine/` と `k2EngineLow/` はエンジン本体
- ゲームのコード: `Game/`。新しいファイルは `Game.vcxproj` と `.filters` に追加する
- モデルなどの素材: `Game/Assets/`（モデルは `modelData/`、.tkm / .tks / .tka）
- ビルド: `C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe Game\Game.sln /p:Configuration=Debug /p:Platform=x64`
  - エンジン由来の警告 LNK4099 は無視してよい
- Git: PATHに無いので Fork 同梱の git を使う
  `%LOCALAPPDATA%\Fork\gitInstance\<version>\cmd\git.exe`

## ステージ・モデルの書き出し（3ds Max 2026 のバッチ）
- `3dsmaxbatch.exe tools\3dsMax\MakeStage1.ms` → `MaxData\Stage1.max` を生成（形・縮尺はスクリプト冒頭の定数）
- `3dsmaxbatch.exe tools\3dsMax\ExportTkm.ms -mxsString file:Stage1.max -mxsString names:Floor_Stage1,Wall_Stage1,Gate_Stage1` → tkm を書き出し、各 tkm に `tkmOptimizer.exe <in> <out>` をかける
- `3dsmaxbatch.exe tools\3dsMax\ExportTkl.ms -mxsString file:Stage1.max -mxsString level:Stage1` → `Game\Assets\level3D\Stage1.tkl`
- テクスチャは `texconv.exe -vflip -f BC7_UNORM -srgb -y -o Game\Assets\modelData <画像>` で DDS にする（k2 のツールは `C:\my\02_Git\01_k2Engine\GameTemplate\tools\3dsMaxScripts\`）
- 書き出し後にできる `MaxData\export_report.txt` と `*_backup.max` は消す
- プレイヤーの銃: `Downloads\m4a4.fbx` を `tools\3dsMax\MakeM4.ms` で `M4.max` にして `names:M4A4 flat:true` で書き出す（原点はスコープの接眼部、テクスチャは `MaxData\textures\m4\`）
- 潜行中の砂の盛り上がり: `MakeSandMound.ms`
- 門の横の穴: `MakePropTextures.ps1` で `T_Hole` を作ってから `MakeHole.ms` で `Hole.max` にして `names:StageHole` で書き出す
- マップに置く小物（`Prop_*`）: `Downloads` の4つの FBX（砂袋の壁は ProOptimizer で頂点を12%に削減）を `MakeProps.ms` で `Props.max` にして `ExportTkm.ms` で書き出す（原点は底面の中心、テクスチャは `MaxData\textures\props\` から `-w 1024 -h 1024` を足して DDS にする。木箱・木樽は FBX にテクスチャが無いので `tools\MakePropTextures.ps1` で自作し、`MakeProps.ms` より先に実行する）。配置は `MakeStage1.ms` の `propClusters`（Point の名前が tkm の名前になり、当たり判定も付く）
- 敵（SWAT）: 元データは `Documents\Swat`（CGTrader、Mixamo の骨）。`MakeSwat.ms` で `Swat.max` を作る（銃を右手に合体・頂点を18%に削減・待機/歩き/撃つ/倒れる/見回す/構えのキーを打つ）。テクスチャは `MaxData\textures\swat\` から 1K の DDS にする（敵は数が多いので軽さ優先）
  - `ExportTkm.ms -mxsString file:Swat.max -mxsString names:SwatSoldier -mxsString flat:true`（頂点の多いモデルは flat:true にする。false だと読み込み時の法線スムージングで Debug がほぼ止まる）
  - `ExportTka.ms -mxsString file:Swat.max -mxsString root:SWAT -mxsString clips:Swat_Idle:0:59,Swat_Walk:100:131,Swat_Shoot:200:209,Swat_Death:300:347,Swat_Look:400:489,Swat_Aim:500:529` → `Game\Assets\animData\`（構え・撃つは IK で両手を銃に合わせている）
  - ProOptimizer などモディファイアのボタン操作は、バッチでも `select` → `max modify mode` → `modPanel.setCurrentObject` してからなら動く
- UI の画像は `tools\MakeUISprites.ps1` で作る（`Game\Assets\sprite\UI_*.DDS`）

## エンジン利用時の注意
- エンジンのソースは Shift-JIS。基本的に編集しない（必要なときは相談してから）
  - 変更済み: `k2EngineLow.cpp` の左上のFPS表示は `K2_SHOW_FPS` を定義したときだけ出す
  - 変更済み: `geometry/BSP.cpp` の `WalkTree` の空ループと、ノードへの `leafArray` のコピーを削除（Debug での tkm 法線スムージングが頂点数の2乗で遅くなっていたため。結果は同じ）
- 当たり判定のワイヤーフレームは Debug ビルドでのみ表示される
- CharacterController の座標は足元（カプセルの下端）
- `FontRender::SetText()` は渡した文字列を書式として `swprintf_s` し直す。`%` を表示したいときは `%%` にして渡す（そのままだと実行時エラー）

## 作業ログ

要点だけ書く。仕様やルールは上の各節に反映し、ここに重複させない。「次回」と「未決定」は最新の日にだけ残す。

### 2026-10-20
- 実装: `Player`（移動・一人称カメラ・重力）、`BackGround`（BoxCollider の仮ステージ）
- 環境整備: git / ビルド確認 / CLAUDE.md

### 2026-10-21
- Notion のタスク表を整理（名前・内容・追加タスク・予備日・段々のタイムラインビュー）。コード変更なし

### 2026-10-23
- 実装: `Stage`（LevelRender で Stage1.tkl を読み込み、PlayerStart・Hole_ をフック）。`BackGround` は削除
- 3ds Max をバッチで動かすステージ制作の流れを用意（`tools/3dsMax/`）

### 2026-10-26
- 実装: 潜行／浮上の切り替え（B）、潜行中の三人称カメラとの切り替え（切り替え中は移動不可・三人称は上下視点なし）、射撃（RB2、`SandBullet`）

### 2026-10-27
- 実装: 弾数（10発）と潜行中だけのリロード（X、1発ずつ補充）、`PlayerUI`（照準・弾数表示）、一人称の仮の銃モデル（`Gun.tkm`）
- ステージ1を本番用の洞窟に作り直し（一本道・床は砂・壁と天井は岩、ambientCG のテクスチャ）。tkl もスクリプトで書き出すようにした
- Notion: Aランク向けにタスク追加（隊長の攻撃・アウトゲーム演出・エフェクト・SE）、ミニマップは保留、α版のタスクを3日前倒し

### 2026-10-29〜11-04
- プレイヤー: HP・被弾・無敵時間、HP0で暗転してゲームオーバー画面へ（`Fade`・`GameOver`、A でリトライ）
- HUD の作り直し（`PlayerUI`）: HP バー、弾の残数マーク、照準、ヒットマーク、ダメージ数字、潜行中の表示、リロードのリング、被弾時の赤い縁。画像は `MakeUISprites.ps1` で生成
- 潜行中の砂の盛り上がり（`SandMound`）
- 敵の土台: `EnemyBase`（HP・被弾・倒れて沈む）、`EnemyBullet`、当たり判定の共通処理 `HitUtil`
- 雑魚兵 `Soldier`: 巡回・発見（！）・攻撃・警戒（？）。潜行中のプレイヤーに正面から近づかれると警戒し、5秒で攻撃
- 雑魚兵の見た目を SWAT モデルに（`MakeSwat.ms`・`ExportTka.ms`、待機・歩き・撃つ）
- エンジン: 左上の FPS 表示を `K2_SHOW_FPS` 定義時だけにした
- `CREDITS.md` を追加

### 2026-11-05
- プレイヤーの銃を M4A4 に変更。LB2 でスコープをのぞく（ズーム・視点と移動の減速・スコープ画面）、腰撃ちは銃口から照準先へ撃つ
- 雑魚兵: 構え（IK で両手を銃に合わせる）・倒れる・見回すアニメーション、障害物に引っかかったら横に避ける、狙う高さと弾の出る位置を銃口に
- コード整理: `MathUtil`・`ObjectNames` に共通処理と名前をまとめ、古い仮モデル（Gun / TempSoldier / Blaster）を削除
- 購入素材の元データを `.gitignore` に入れ、履歴を書き換えて公開リポジトリから除いた

### 2026-11-06〜11-09
- 開始時の向きを右に90°、スコープの倍率を下げた（コードは 1.25 倍）
- 雑魚兵: 攻撃中に一定時間ごとに横や前後へ位置を変える（`UpdateAttack`・`ChooseAttackMove`・`IsReachable`）。物陰に隠れる動きとアニメーションは試したが外した
- 三人称カメラの壁めり込み防止（球のスイープでカメラを手前に寄せる `ResolveCameraCollision`）
- エンジン: `BSP.cpp` の空ループ削除で Debug の起動を高速化（相談の上）
- ステージ1を0.7倍に縮小（`G_SCALE` 1.071）。ゲート・兵士の位置も合わせた
- 小物の取り込み `MakeProps.ms`（木箱・樽・ドラム缶・携行缶・UAZ）と配置 `propClusters`。木箱と樽のテクスチャは `MakePropTextures.ps1` で自作
- 遅れていたタスクを実装: 敵の全滅で門の横の穴から進める（穴のモデル `StageHole` も作成）、クリアタイム計測（`Game::GetClearTime`）、仮のクリア画面 `StageClear`、画面上部のメッセージ（`PlayerUI::ShowMessage`）
- 雑魚兵を9人→6人に

### 2026-11-10
- 砂袋の壁（直線・弧・L字）を取り込み（頂点を12%に削減、1.15倍）。入口に2段積みのバリケード、ほかに陣地を5か所
- 木箱・樽・ドラム缶を18個に減らし、携行缶は車の近くだけに。小物は全部で40個
- 敵の撃破エフェクト（砂粒が飛び散る）は作ったが、不要とのことで削除
- 内容ごとに5回に分けてコミットし Push。古い履歴の `backup-before-rewrite` ブランチを削除
- 次回: 11/10〜11/11 の「クリア／ゲームオーバー／リトライの画面切り替え」「ゲーム全体の流れ（現在のステージ・リトライ地点）の管理」（11/12 は予備日）
- 未決定: Notion のステータス（砂の盛り上がり・ステージ1の配置・障害物モデル・全滅で進める・クリアタイムは笠井のOK待ち）、「敵の撃破エフェクトの制作」タスクの扱い、「潜れない場所の判定」（保留中）、スコープ倍率の仕様（上の節は1.5倍、コードは1.25倍）、「α版の通しプレイ確認」が予備日の 11/15 にかかっている件
- 保留: CREDITS.md に今回の4素材（木箱と樽・ドラム缶と携行缶・UAZ・砂袋）の配布ページを追記（リンク待ち）、企画書のファイル名と中身にフルネーム・学校名が入っている件（笠井の判断待ち）
