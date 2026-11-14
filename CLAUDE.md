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
  - 敵に見つかるかどうかは、切り替えの途中は切り替える前の状態として扱う（浮上しきるまでは見つからない。潜りきるまでは見つかる。`Player::IsUnderground`）
  - 切り替え中（プレイヤーが動けない間）は、敵（`EnemyBase::Update`）・敵の弾（`EnemyBullet::Update`）・クリアタイムを止める（`Player::IsWorldPaused`）。砂しぶき・砂ぼこりなどのエフェクトは止めない
  - 弾は、潜るボタンを押して動けなくなった瞬間から、浮上しきるまで当たらない（潜っている間と切り替え中は無敵。`Player::Damage`・`HitTest`）
  - 床（砂）は基本的にどこでも潜れる。オブジェクト（岩・木箱など）の上は潜れない
- プレイヤーの操作は 移動・視点 に加えて「潜行／浮上」「リーン」「リロード」「射撃」のみ。空は飛べない
  - リロードは潜行中だけボタンで行える（浮上中は不可）。1発ずつ補充され、浮上すると中断（入った分は残る）
  - ボタン: 潜行／浮上 B、射撃 RB2、スコープをのぞく LB2（押している間。ズーム1.5倍・視点と移動が遅くなる）、リロード X、リーン LB1／RB1
- ステージ1 → ステージ2（隊長がカギを落とす）→ ボス部屋（ジェットパックで浮遊するボス）
  - ステージ1とステージ2は、門の横から入る潜ったときだけ通れる抜け道でつながっていて、兵士を倒さなくても行き来できる（ステージ2からも同じ道で戻れる）。抜け道は途中で3つに分かれ、正解の1本だけがステージ2に続く（ほかは行き止まり）
  - ボス部屋に入るには、ステージ1・2の兵士を全員倒し、さらに隊長のカギを持っている必要がある
  - ステージ2ができるまでは、正解の抜け道の奥（`StageExit`）に着くとクリア画面に行く（仮）
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
- `MakeStage1.ms` の主な定数: `g_playerStart`（開始位置）、`g_diveTunnels`（潜ったときだけ通れる抜け道。後ろの4本が門の横から北へ入ってステージ2へ向かう分かれ道）、`g_stageExit`（正解の抜け道の奥。tkl の Point `StageExit`）、`lightData`（`Light_Daylight`／`Light_Lamp` の Point → ゲームでポイントライト。ランプは入口から門の横の抜け道の入口まで、進む道に沿って約600〜700おきに10個並べ、光で奥へ誘導する。ポイントライトは影を落とさないので、ランプの真下には小物を置かない（`stage_report.txt` に各ランプから一番近い小物までの距離が出る。200以上離す））、`EntranceBarrier`（透明な壁）。座標は Max の生の値 × `G_SCALE` = ワールド、ゲームでは (x, z, -y)
- 床と外の砂漠のどちらかを変えたら、継ぎ目がそろうように両方を書き出す
- テクスチャは `texconv.exe -vflip -f BC7_UNORM -srgb -y -o Game\Assets\modelData <画像>` で DDS にする（k2 のツールは `C:\my\02_Git\01_k2Engine\GameTemplate\tools\3dsMaxScripts\`）
- 書き出し後にできる `MaxData\export_report.txt` と `*_backup.max` は消す
- プレイヤーの銃: `Downloads\m4a4.fbx` を `tools\3dsMax\MakeM4.ms` で `M4.max` にして `names:M4A4 flat:true` で書き出す（原点はスコープの接眼部、テクスチャは `MaxData\textures\m4\`）
- 潜行中の砂の盛り上がり: `MakeSandMound.ms`（平たい楕円の盛り上がり＋前と横に押しのけた砂のふち＋ノイズのひび。後ろに長い。平らなつばの部分は削って、盛り上がっているところだけ残す（`keepHeight`）。`names:SandMound` で書き出す）。MAXScript では `-(x)^2` が「(-x) の2乗」になるので、マイナスは `-1.0 * x * x` と書く
- 潜る／浮上する切り替え（`Player::UpdateCamera`）は 1.6 秒（`CAMERA_BLEND_TIME`。0.8 秒は速すぎ、2.4 秒は遅すぎた）。潜るときは銃を浮上のときの逆の動きで下げて消す（進み具合 30% まで、`GUN_LOWER_RATE`。銃は沈む目線 `m_eyeHeight` に付いて動く）。始めから徐々に暗転し、進み具合 45〜55% は真っ暗、そこから明るくなる（`Player::GetSwitchDarkness`・`PlayerUI::UpdateSwitchFade`）。真っ暗の真ん中（時間順の進み具合 `GetSwitchProgress` の `SWITCH_CUT` 0.5）で一人称と三人称を切る。潜るときは一人称のまま目線が床まで沈み、切ったあと三人称のカメラが近くから後ろへ引き、盛り上がりがポンと出る（`EaseOutBack`）。浮上するときは盛り上がりが膨らんで揺れ、切った瞬間に砂が弾け、一人称の目線が床から跳ね上がる。切り替えの前後は小さく揺らす。暗転なしで一方向になめらかに移すだけの動きや、砂の画像で画面を覆う演出も試したが、今の形（暗転＋沈む／跳ね上がる動き）に落ち着いた
- 潜る／浮上するときの砂しぶきの粒: `MakeSandGrain.ms` で `SandGrain.max` → `names:SandGrain flat:true` で書き出す（テクスチャは床の砂と同じ）
- 浮上するときに舞う砂ぼこり: `tools\MakeEffectTextures.ps1` が `FX_SandDust.png`（ふちがぼけた丸、アルファ付き）も作る → `MakeSandDust.ms` で `SandDust.max`（20×20 の板1枚、+Z 向き）→ `names:SandDust flat:true` で書き出す。ゲームでは `SandDust.fx`（半透明、色と透明度を定数バッファで粒ごとに渡す）で描き、毎フレームカメラに向ける
- 銃口の光: `tools\MakeEffectTextures.ps1`（星形と炎のテクスチャ、`MaxData\textures\effects\`）→ `MakeMuzzleFlash.ms` で `MuzzleFlash.max` → `names:MuzzleFlash flat:true` で書き出す。原点は銃口で、+Z（Max の -Y）が撃つ向き。正面の星形の板1枚と十字に交わる炎の板2枚
- 敵の弾: `tools\MakeAmmoTextures.ps1`（真鍮と銅のテクスチャ、`MaxData\textures\ammo\`）→ `MakeEnemyBullet.ms` で `Downloads\Ammos.fbx` の中くらいの弾（`Cylinder.001` と `Cone`）を `EnemyBullet.max` にして `names:EnemyBullet` で書き出す。長さ20、原点は弾の中心で、+Z（Max の -Y）が弾頭
- 黒い穴のモデル（`MakeHole.ms`・`StageHole.tkm`）は今は使っていない
- マップに置く小物（`Prop_*`）: `Downloads` の4つの FBX（砂袋の壁は ProOptimizer で頂点を12%に削減）を `MakeProps.ms` で `Props.max` にして `ExportTkm.ms` で書き出す（原点は底面の中心、テクスチャは `MaxData\textures\props\` から `-w 1024 -h 1024` を足して DDS にする。木箱・木樽は FBX にテクスチャが無いので `tools\MakePropTextures.ps1` で自作し、`MakeProps.ms` より先に実行する）。配置は `MakeStage1.ms` の `propClusters`（Point の名前が tkm の名前になり、当たり判定も付く）。木箱・樽・ドラム缶・携行缶は大きさをそろえず、Point の scale で 0.8〜1.25 倍にばらつかせる（`G_PROP_SCALE_MIN/MAX`、乱数の種は固定）
- 敵（SWAT）: 元データは `Documents\Swat`（CGTrader、Mixamo の骨）。`MakeSwat.ms` で `Swat.max` を作る（銃を右手に合体・頂点を18%に削減・待機/歩き/撃つ/倒れる/見回す/構えのキーを打つ）。テクスチャは `MaxData\textures\swat\` から 1K の DDS にする（敵は数が多いので軽さ優先）
  - `ExportTkm.ms -mxsString file:Swat.max -mxsString names:SwatSoldier -mxsString flat:true`（頂点の多いモデルは flat:true にする。false だと読み込み時の法線スムージングで Debug がほぼ止まる）
  - `ExportTka.ms -mxsString file:Swat.max -mxsString root:SWAT -mxsString clips:Swat_Idle:0:59,Swat_Walk:100:131,Swat_Shoot:200:209,Swat_Death:300:347,Swat_Look:400:489,Swat_Aim:500:529` → `Game\Assets\animData\`（構え・撃つは IK で両手を銃に合わせている）
  - ProOptimizer などモディファイアのボタン操作は、バッチでも `select` → `max modify mode` → `modPanel.setCurrentObject` してからなら動く
- 陣地のランタン: `MakePropTextures.ps1`（金属・ガラス・ロープのテクスチャ）→ `MakeLantern.ms` で `Lantern.max` → `names:Prop_Lantern,Prop_LanternGlass,Prop_LanternRope` で書き出す。原点はガラスの中心（ロープは下端、長さ100）。ゲームでは `Light_Lamp` の位置に置き、天井までロープを伸ばす（`Stage::AddLantern`）。ガラスは `Emissive.fx` で光らせる
- UI の画像は `tools\MakeUISprites.ps1` で作る（`Game\Assets\sprite\UI_*.DDS`）。ボタンの絵 `UI_ButtonB/X/LT/RT` も自作（Xbox の配色）
- HUD は画面の上側にまとめる（HP は左上、弾数は右上、潜行中の表示は上の真ん中）。操作ガイド（`PlayerUI::UpdateGuide`、弾数の下に上から並べる）: 今できる操作だけを「文字＋ボタンの絵」で出す。地上は 射撃 RT（弾0で薄く）・スコープ LT・潜る B、潜行中は リロード X（満タンなら出さない、リロード中は薄く）・浮上する B。切り替え中と死亡時は消す。文字は右寄せで、幅は文字数×`GUIDE_CHAR_WIDTH` で見積もっている

## エンジン利用時の注意
- エンジンのソースは Shift-JIS。基本的に編集しない（必要なときは相談してから）
  - 変更済み: `k2EngineLow.cpp` の左上のFPS表示は `K2_SHOW_FPS` を定義したときだけ出す
  - 変更済み: 被写界深度（遠くをぼかす）を使うため、`PostEffect.h`・`RenderingEngine.h` に `EnableDof`／`DisableDof` を追加し、`postEffect/Dof.cpp` のボケ画像を合成するスプライトの大きさを 1280×720 固定から `FRAME_BUFFER_W/H` に直した（そのままだと画面の中央の四角にしかかからない）。ゲームでは `Stage::InitLighting` で有効にし、`Stage` を消すときに無効にする（ゲームを終了するときは `g_renderingEngine` が先に `nullptr` になってからゲームオブジェクトが消えるので、デストラクタでエンジンを触るときは `nullptr` か確かめる）。ぼけ始め・最大の距離と濃さはゲーム側の `Game\Assets\shader\postEffect\dof.fx`（Shift-JIS・BOMなし）の `DOF_START`・`DOF_END`・`DOF_MAX_RATE`
  - 変更済み: `geometry/BSP.cpp` の `WalkTree` の空ループと、ノードへの `leafArray` のコピーを削除（Debug での tkm 法線スムージングが頂点数の2乗で遅くなっていたため。結果は同じ）
- 当たり判定のワイヤーフレームは Debug ビルドでのみ表示される
- CharacterController の座標は足元（カプセルの下端）
- `ModelRender` の視錐台カリングは AABB の8頂点を `|w|` で割って判定するので、カメラのすぐ横に広がる大きなモデル（砂漠など）は映っていても消える。その場合は `IRenderer` を継承して `Model` を持ち、毎フレーム `g_renderingEngine->AddRenderObject` する
- ポイントライトは `PointLight`（`Init` → 色・範囲・累乗を設定）。カメラ空間の座標を使うので毎フレーム `Update()` を呼ぶ
- 外の日差しはゲーム側の `Game\Assets\shader\DeferredLighting.fx`（Shift-JIS・BOMなし）で入口から外だけに付ける。ディレクションライト0が太陽（影あり）、ディレクションライト3は光ではなくパラメータ置き場（direction = 入口の x・ぼかす長さ・有効フラグ、color = 外の環境光）。設定は `Stage::InitLighting`。トーンマップとブルームはエンジン側でオフ
- エンジンのディファード以外（`model.fx` の半透明・フォワード）はこの切り替えを知らないので、使うときは注意
- `ModelRender.cpp` は `Draw(rc, 1)` と書かれているが、LevelRender で同じ名前の小物をインスタンシングでまとめても全部見えている（笠井が確認）。砂しぶきの `SandSplash` は粒ごとに ModelRender を持っている
- エフェクトは Effekseer のファイルが無いので、今はモデルをコードで動かして作る。光るものは `Emissive.fx` をフォワードで描き、`m_alphaBlendMode = AlphaBlendMode_Add`・`m_isDepthWrite = false`・`m_cullMode = D3D12_CULL_MODE_NONE` にすると黒い部分が消えて光だけが足される
- 撃ったときと着弾のエフェクトは `EffectManager`（Game が作る、`EffectManager::Get()`）にまとめている
- 砂袋の壁（`Prop_Sandbag*`）は LevelRender に任せず `Stage::AddSandbag` で読む。見た目はそのまま、当たり判定は形に沿って並べた縦の箱（`SANDBAG_SHAPES`、Max のローカル座標の線分＋厚み・高さ）にしている。メッシュの当たり判定だと、丸い袋の斜面をキャラが登ってしまい、細かい三角形との判定でとても重くなるため
- 潜行中のカプセルは、エンジンの CharacterController が「下向きにゆるく傾いた天井」を壁と見なさずにすり抜けるので、`Player::IsDiveHeadBlocked` で体の中心の上（高さ60まで、半径10の球）をスイープして止め、当たったら X か Z だけの移動に切り替える（`LimitDiveMove`）。抜け道（`MakeStage1.ms` の `G_TUNNEL_HALF_WIDTH` 95・`G_TUNNEL_HEIGHT` 135）は横をまっすぐ立てた箱形にして、ゆるい天井の斜面を作らない。壁モデルの厚み（Shell 40）の分だけ天井は低くなり、床からの隙間は約101（潜行中のカプセル80は通れ、浮上中の170は通れない）。広げすぎると洞窟の壁ごと削れて抜け道に見えなくなる
- 保険として、真上に天井が無い位置（＝マップの外）に出たら直前の位置に戻す（`Player::HasCeilingAbove`・`m_safePosition`）
- 音は `Game\Assets\sound\<SoundId の名前>.wav`（16bit PCM）。`SoundList.h` の `SoundId` と名前の表に足すだけで、最初に鳴らすときにまとめて登録される。ファイルが無い音は鳴らないだけで落ちない。`Play`（2D）／`PlayAt`（カメラからの距離で音量を下げる）／`PlayLoop`・`StopLoop`（ループ。鳴り終わると消える普通の音と違い、自分で止めて消す）
- 兵士の足音 `SoldierStep` は1歩分の短い音で、歩きアニメーション（1周 1.067 秒で2歩）に合わせて 0.533 秒×アニメ速度ごとに `PlayAt` で鳴らす（`Soldier::UpdateFootstep`、範囲 1600）。何歩も続く録音なら1歩だけ切り出して置く
- 音の取り込みは `tools\ConvertSound.ps1 -Source <mp3かwav> -Name <SoundIdの名前>`（mp3 は WinRT の MediaTranscoder で wav にし、24bit などは fmt と data だけの 16bit に直す。ffmpeg は無い）。効果音ラボはスクリプトからのダウンロードを 403 で弾くので、ダウンロードは笠井がブラウザで行う
- LB2・RB2（アナログのトリガー）は、エンジンが「0 でなければ押している」と判定するので、押し始めと離すときに値がちらついて `IsTrigger` が何回も立つ。1回押しを数えたいときは、少しの間離れていないと「離した」と見なさない処理を挟む（`Player::UpdateShotTrigger`）
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
- マズルフラッシュと着弾のエフェクト（`EffectManager`）: 撃つと銃口に星形＋炎の光（加算合成、0.06秒）と、まわりを照らす一瞬のポイントライトが出る（プレイヤーは小さめ、雑魚兵は大きめ）。砂弾が壁や床に当たると砂粒が跳ね、敵に当たるともっと多く跳ねる。敵の弾が壁に当たると小さな砂粒が少し跳ねる（`PlayMuzzleFlash`・`PlayBulletImpact`・`PlayEnemyBulletImpact`、砂粒は `SandSplash::PlayImpact`）。Notion のタスクは「進行中」
- 壁・床・小物・兵士が全部同じ色に見える件: テクスチャは砂（平均 R179 G151 B114）と岩（R92 G79 B73）で違うが、ランプの光が強いオレンジ（7, 4.2, 1.8）で範囲も広く、全部を同じ黄土色に染めていた。ランプを白っぽい暖色（5.2, 4.2, 3.0）・範囲700・累乗2.0に、洞窟の環境光を少し青みのある中性色（0.11, 0.115, 0.13）にした（`Stage.cpp` の `LAMP_*`・`CAVE_AMBIENT`）
- マズルフラッシュ・着弾・ライティングの色とも笠井の OK。コミットして Notion の「マズルフラッシュ・被弾エフェクトの制作」を完了にした

### 2026-11-12〜11-14
- 敵の弾を `Ammos.fbx` の中くらいの弾のモデルに（`MakeEnemyBullet.ms`、飛ぶ向きに弾頭を向ける）
- 効果音（`SoundList.h`・`tools\ConvertSound.ps1`）: 射撃・リロード（1発ごと）・構える（浮上したときだけ）・弾切れ・潜る・浮上・満タン・スコープ・敵の銃声・敵が倒れる・発見・メニューのカーソルと決定・兵士の足音（歩きに合わせる、素材待ち）。弾切れの音はトリガーのちらつきで連打になっていたので直した（`UpdateShotTrigger`）。音のファイルは再配布禁止なので `.gitignore` に入れた
- ステージ2への道: 全滅で開く黒い穴をやめ、門の横から入る潜ったときだけ通れる抜け道を3つに分け、正解の奥 `StageExit` に着くとクリア（仮）
- 砂袋の当たり判定を縦の箱に（上に乗れて重くなっていた）、残弾数の表示サイズをそろえた
- 外部レビューを受けて: 浮上時と潜行中の移動で砂ぼこり（`SandDust.fx`・`SandSplash::SpawnDust`）、ランタンを奥への道に沿って10個並べ直し、真下に物を置かない
- 潜る・浮上するの見せ方を何度も試した（画面を砂で覆う→外した、一方向のなめらかな移動→分かりにくい）。今は 1.6 秒で、暗転＋目線が沈む／跳ね上がる＋盛り上がりがポンと出る／膨らんで弾ける。潜るときは銃を下げて消す。切り替え中は敵・敵の弾・クリアタイムを止め、弾は当たらない（見つかるかは切り替え前の状態）
- 盛り上がりのモデルを作り直し（平たい形、ふちを削る）、移動中は後ろに小さな盛り上がりの跡
- 小物（木箱・樽・ドラム缶・携行缶）の大きさを 0.8〜1.25 倍にばらつかせた
- 被写界深度（遠くをぼかす）を有効に（エンジンの変更は上の節。今は 1200〜2500 で強さ 0.3）。終了時にデストラクタで落ちていたのを直した
- UI: 状態に合わせた操作ガイド（ボタンの絵を自作）、HP・弾数・ガイドを画面の上側へ、「潜行中 射撃不可」を大きく。Select（スペース）で被ダメージのデバッグ処理を削除
- 動画の編集（指定区間の切り取り・倍速）は Media Foundation の自作ツールで対応した（スクラッチに置いてあり、リポジトリには入れていない）
- 次回: 兵士の足音の素材（`SoldierStep`、笠井が探す）。浮上・潜行の演出と操作ガイドの見た目は笠井のプレイ確認待ち。α版の締め切り（11/16）に向けて通しプレイの確認
- 未決定: Notion のステータス（砂の盛り上がり・ステージ1の配置・障害物モデル・全滅で進める・クリアタイムは笠井のOK待ち。全滅で進める仕様は抜け道に変わった）、「敵の撃破エフェクトの制作」タスクの扱い、「潜れない場所の判定」（保留中）、スコープ倍率の仕様（上の節は1.5倍、コードは1.25倍）、ランタンを自作の仮モデルのままにするか素材に差し替えるか、使わなくなった黒い穴のモデル（`MakeHole.ms`・`StageHole.tkm`）を消すか、効果音を提出用データにどう入れるか
- 外部レビュー（2026-11-14 に笠井が受けた）: 砂から浮上するときに、砂の粒子が舞うエフェクトがあるとより良い。Nova Shader（Unity 用のエフェクト向けシェーダー）を勧められたが、k2Engine では使えないので、同じ考え方（ふわっと漂う細かい砂ぼこり）を自前で作る。ほかに、光るもの（ランタン）を奥へ向かって置いていけば、UI で「奥へ進め」と書かなくてもプレイヤーは光の方へ進む／ランタンの真下に物があると陰影に違和感が出るので避ける、とのこと
- 保留: CREDITS.md に素材の配布ページを追記（木箱と樽・ドラム缶と携行缶・UAZ・砂袋・Ammos.fbx・効果音ラボ・AudioStock の発砲音。リンク待ち）、企画書のファイル名と中身にフルネーム・学校名が入っている件（笠井の判断待ち）
