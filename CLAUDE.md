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
- 新規ファイルは UTF-8（BOM付き）・改行CRLF で作る（シェーダー `.fx` だけは BOM を付けない。BOM があるとシェーダーのコンパイルが通らない）
- 笠井への返答・説明は必ず日本語で書く
- ゲームを起動したり画面を撮ったりしない（笠井がPCを使っている。プレイ確認は笠井が行い、スクリーンショットをくれる）
- 進捗報告の pptx（`08_*_進捗報告*.pptx`）はリポジトリに入れない。購入素材の元データも入れない（再配布しない）
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
- `3dsmaxbatch.exe tools\3dsMax\ExportTkm.ms -mxsString file:Stage1.max -mxsString names:Floor_Stage1,Wall_Stage1,Gate_Stage1,Desert_Stage1` → tkm を書き出し、各 tkm に `tkmOptimizer.exe <in> <out>` をかける
- `3dsmaxbatch.exe tools\3dsMax\ExportTkl.ms -mxsString file:Stage1.max -mxsString level:Stage1` → `Game\Assets\level3D\Stage1.tkl`
- `MakeStage1.ms` の主な定数: `g_playerStart`（開始位置）、`g_diveTunnels`（潜ったときだけ通れる抜け道）、`lightData`（`Light_Daylight`／`Light_Lamp` の Point → ゲームでポイントライト）、`EntranceBarrier`（透明な壁）。座標は Max の生の値 × `G_SCALE` = ワールド、ゲームでは (x, z, -y)
- 床と外の砂漠のどちらかを変えたら、継ぎ目がそろうように両方を書き出す
- テクスチャは `texconv.exe -vflip -f BC7_UNORM -srgb -y -o Game\Assets\modelData <画像>` で DDS にする（k2 のツールは `C:\my\02_Git\01_k2Engine\GameTemplate\tools\3dsMaxScripts\`）
- 書き出し後にできる `MaxData\export_report.txt` と `*_backup.max` は消す
- プレイヤーの銃: `Downloads\m4a4.fbx` を `tools\3dsMax\MakeM4.ms` で `M4.max` にして `names:M4A4 flat:true` で書き出す（原点はスコープの接眼部、テクスチャは `MaxData\textures\m4\`）
- 潜行中の砂の盛り上がり: `MakeSandMound.ms`
- 潜る／浮上するときの砂しぶきの粒: `MakeSandGrain.ms` で `SandGrain.max` → `names:SandGrain flat:true` で書き出す（テクスチャは床の砂と同じ）
- 門の横の穴: `MakePropTextures.ps1` で `T_Hole` を作ってから `MakeHole.ms` で `Hole.max` にして `names:StageHole` で書き出す
- マップに置く小物（`Prop_*`）: `Downloads` の4つの FBX（砂袋の壁は ProOptimizer で頂点を12%に削減）を `MakeProps.ms` で `Props.max` にして `ExportTkm.ms` で書き出す（原点は底面の中心、テクスチャは `MaxData\textures\props\` から `-w 1024 -h 1024` を足して DDS にする。木箱・木樽は FBX にテクスチャが無いので `tools\MakePropTextures.ps1` で自作し、`MakeProps.ms` より先に実行する）。配置は `MakeStage1.ms` の `propClusters`（Point の名前が tkm の名前になり、当たり判定も付く）
- 敵（SWAT）: 元データは `Documents\Swat`（CGTrader、Mixamo の骨）。`MakeSwat.ms` で `Swat.max` を作る（銃を右手に合体・頂点を18%に削減・待機/歩き/撃つ/倒れる/見回す/構えのキーを打つ）。テクスチャは `MaxData\textures\swat\` から 1K の DDS にする（敵は数が多いので軽さ優先）
  - `ExportTkm.ms -mxsString file:Swat.max -mxsString names:SwatSoldier -mxsString flat:true`（頂点の多いモデルは flat:true にする。false だと読み込み時の法線スムージングで Debug がほぼ止まる）
  - `ExportTka.ms -mxsString file:Swat.max -mxsString root:SWAT -mxsString clips:Swat_Idle:0:59,Swat_Walk:100:131,Swat_Shoot:200:209,Swat_Death:300:347,Swat_Look:400:489,Swat_Aim:500:529` → `Game\Assets\animData\`（構え・撃つは IK で両手を銃に合わせている）
  - ProOptimizer などモディファイアのボタン操作は、バッチでも `select` → `max modify mode` → `modPanel.setCurrentObject` してからなら動く
- 陣地のランタン: `MakePropTextures.ps1`（金属・ガラス・ロープのテクスチャ）→ `MakeLantern.ms` で `Lantern.max` → `names:Prop_Lantern,Prop_LanternGlass,Prop_LanternRope` で書き出す。原点はガラスの中心（ロープは下端、長さ100）。ゲームでは `Light_Lamp` の位置に置き、天井までロープを伸ばす（`Stage::AddLantern`）。ガラスは `Emissive.fx` で光らせる
- UI の画像は `tools\MakeUISprites.ps1` で作る（`Game\Assets\sprite\UI_*.DDS`）

## エンジン利用時の注意
- エンジンのソースは Shift-JIS。基本的に編集しない（必要なときは相談してから）
  - 変更済み: `k2EngineLow.cpp` の左上のFPS表示は `K2_SHOW_FPS` を定義したときだけ出す
  - 変更済み: `geometry/BSP.cpp` の `WalkTree` の空ループと、ノードへの `leafArray` のコピーを削除（Debug での tkm 法線スムージングが頂点数の2乗で遅くなっていたため。結果は同じ）
- 当たり判定のワイヤーフレームは Debug ビルドでのみ表示される
- CharacterController の座標は足元（カプセルの下端）
- `ModelRender` の視錐台カリングは AABB の8頂点を `|w|` で割って判定するので、カメラのすぐ横に広がる大きなモデル（砂漠など）は映っていても消える。その場合は `IRenderer` を継承して `Model` を持ち、毎フレーム `g_renderingEngine->AddRenderObject` する
- ポイントライトは `PointLight`（`Init` → 色・範囲・累乗を設定）。カメラ空間の座標を使うので毎フレーム `Update()` を呼ぶ
- 外の日差しはゲーム側の `Game\Assets\shader\DeferredLighting.fx`（Shift-JIS・BOMなし）で入口から外だけに付ける。ディレクションライト0が太陽（影あり）、ディレクションライト3は光ではなくパラメータ置き場（direction = 入口の x・ぼかす長さ・有効フラグ、color = 外の環境光）。設定は `Stage::InitLighting`。トーンマップとブルームはエンジン側でオフ
- エンジンのディファード以外（`model.fx` の半透明・フォワード）はこの切り替えを知らないので、使うときは注意
- `ModelRender.cpp` は `Draw(rc, 1)` と書かれているが、LevelRender で同じ名前の小物をインスタンシングでまとめても全部見えている（笠井が確認）。砂しぶきの `SandSplash` は粒ごとに ModelRender を持っている
- エフェクトは Effekseer のファイルが無いので、今はモデルをコードで動かして作る
- 潜行中のカプセルは、エンジンの CharacterController が「下向きにゆるく傾いた天井」を壁と見なさずにすり抜けるので、`Player::IsDiveHeadBlocked` で体の中心の上（高さ60まで、半径10の球）をスイープして止め、当たったら X か Z だけの移動に切り替える（`LimitDiveMove`）。抜け道（`MakeStage1.ms` の `G_TUNNEL_HALF_WIDTH` 95・`G_TUNNEL_HEIGHT` 135）は横をまっすぐ立てた箱形にして、ゆるい天井の斜面を作らない。壁モデルの厚み（Shell 40）の分だけ天井は低くなり、床からの隙間は約101（潜行中のカプセル80は通れ、浮上中の170は通れない）。広げすぎると洞窟の壁ごと削れて抜け道に見えなくなる
- 保険として、真上に天井が無い位置（＝マップの外）に出たら直前の位置に戻す（`Player::HasCeilingAbove`・`m_safePosition`）
- フォント（太字）には ▶ ！（）が無い。使う前にグリフがあるか確かめる
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
- （ここから下は同じ日の後半。まだコミットしていない）
- 画面の流れ: `GameFlow`（現在のステージ・リトライ地点・カギ・合計タイム・リトライ回数。`FindGO` か `NewGO` で常駐）、仮の `Title`（A でスタート）、`GameOver` をメニュー化（リトライ／タイトルへ、十字キーとスティック）、`StageClear` → `GameClear` に改名（合計タイム・リトライ回数を表示し、A でタイトルへ）。`Fade::Get()` を追加。`main.cpp`（Shift-JIS）は `Title` から始まる
- 開始直後に撃たれる件: 開始位置を洞窟の入口側の通路 `g_playerStart = [-3950,-1250]` に移し、最初の見張り（`Game.cpp` の SOLDIERS[0]）は入口の通路の出口を向いて立つ。潜ったときだけ通れる低い抜け道 `g_diveTunnels` を4本追加（地図は `MaxData\stage_report.txt` に出る。このファイルはコミットしない）
- 潜行中のプレイヤーは低いカプセル（`m_diveCharaCon`、高さ合計80）に切り替える。天井が低くて浮上できないときは「天井が低くて浮上できない」と出す（`Player::HasHeadroom`・`SetDiveBody`）
- 開始位置の裏を洞窟の入口に: 入口の外は砂漠（`Desert_Stage1`）と昼の空（`skyCubeMapWild`、カメラに追従、`Stage::InitSky`）。入口には透明な壁（tkl の Point `EntranceBarrier`、`Stage::InitBarrier`）。遠くの岩山は不要とのことで削除
- ライティング: 洞窟らしく暗くした（太陽を0、環境光 0.16）。tkl の Point `Light_Daylight`（入口の光）と `Light_Lamp`（13個、陣地の上）からポイントライトを作る（`Stage::AddPointLight`）。ランプのモデルはまだ無い
- 床と砂漠の UV はワールド座標の真上からの投影にして継ぎ目をそろえた（`finishGroundMesh`、300 で1枚）
- 笠井の確認: ゲームループ（仮）・入口と透明な壁・洞窟のライティングは OK。床と砂漠の境目の線はまだ見えた。抜け道で潜行中にマップの外に出た
- 境目の対策: 砂漠の専用シェーダー（OutdoorLit.fx）をやめ、砂漠も普通のディファードで描く（`m_outdoorModel` を毎フレーム `AddRenderObject`）。日差しは `DeferredLighting.fx` で入口から外だけに足すので、床と砂漠が同じ光の計算になる。太陽は洞窟の奥側の上から当て、入口の外に岩の影が落ちるようにした
- 抜け道の不具合: 天井の低いゆるい斜面をエンジンの当たり判定が壁と見なさず、すり抜けていた。頭の判定（`Player::IsDiveHeadBlocked`）だけでは直らなかったので、抜け道の横を垂直に作り直してステージを書き出し直した（ゆるい斜面は415枚→2枚、残りは洞窟の中の岩の柱）。一度広げすぎて壁ごと削れたので、幅95・高さ135に戻した。真上に天井が無い位置に出たら戻す保険も入れた
- 陣地のランタンを追加（`MakeLantern.ms`、`Stage::AddLantern`、`Emissive.fx`）。13個のランプの位置に天井からロープで吊るす
- 笠井の OK が出たので内容ごとにコミットして Push。Notion の画面切り替え・ゲーム全体の流れのタスクを完了にした
- 砂しぶき（`SandSplash`、Player が持つ）: 潜る瞬間・浮上する瞬間に砂粒（`SandGrain.tkm`）を足元から飛ばす。重力で落ちて床に着き、最後に縮んで消える（`Play`・`Spawn`・`Update`・`Draw`）。Notion のタスクは「進行中」

### 2026-11-11
- 砂しぶき: 潜るときは OK。浮上のときは「周りの砂がボコボコするくらい」にしたいとのことで、高く吹き上げるのをやめ、周りに小さな砂の山（`SandMound.tkm` を小さくしたもの）を時間差で7つ盛り上げて沈め、そこから砂粒を少し跳ねさせるようにした（`SpawnBump`・`SpawnBumpGrains`・`UpdateBump`）
- 笠井の OK が出たのでコミットし、Notion の「潜行時の砂しぶきエフェクトの制作」を完了にした。同じ木箱・樽が複数あっても全部見えていることも確認してもらった
- 次回: 「マズルフラッシュ・被弾エフェクトの制作」（期日 11/13）
- 未決定: Notion のステータス（砂の盛り上がり・ステージ1の配置・障害物モデル・全滅で進める・クリアタイムは笠井のOK待ち）、「敵の撃破エフェクトの制作」タスクの扱い、「潜れない場所の判定」（保留中）、スコープ倍率の仕様（上の節は1.5倍、コードは1.25倍）、「α版の通しプレイ確認」が予備日の 11/15 にかかっている件、ランタンを自作の仮モデルのままにするか素材に差し替えるか
- 保留: CREDITS.md に今回の4素材（木箱と樽・ドラム缶と携行缶・UAZ・砂袋）の配布ページを追記（リンク待ち）、企画書のファイル名と中身にフルネーム・学校名が入っている件（笠井の判断待ち）
