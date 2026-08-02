"# 設計詳細: PhysicsEngine

## 概要
このプロジェクトは、基本的な物理シミュレーションエンジン（PointMassモデル）と、そのOpenGLによる視覚化を目的としています。

## 物理エンジン設計
### 1. 数学基盤 (Vector3)
- `Vector3` 構造体は、3次元空間でのベクトル演算を提供します。
- 実装内容: 加算、減算、スカラー乗算、ドット積、長さの計算。

### 2. 物理オブジェクト (PointMass)
- `PointMass` クラスは、質量を持つ点としての物理的属性を保持します。
- 属性: 位置 (`position`), 速度 (`velocity`), 質量 (`mass`)。
- メソッド: 属性のゲッター/セッターを提供します。

### 3. シミュレーション世界 (PhysicsWorld)
- `PhysicsWorld` クラスは、複数の `PointMass` オブジェクトを管理し、物理シミュレーションを進行させます。
- 動作原理:
    - 重力 (`gravity`) ベクトルを保持。
    - `addPointMass`: 点質量を世界に追加。
    - `step(double timeStep)`: 指定された時間ステップごとに、すべての点質量の速度と位置を更新します。
        - 速度更新: $v = v + g \cdot dt$
        - 位置更新: $p = p + v \cdot dt$
    - 簡易的な欧ラー法（Forward Euler）に近い更新式を採用しています。

## 描画システム (OpenGL)
- **GLFW/GLAD**: ウィンドウ管理およびOpenGLコンテキストの初期化。
- **Shader**:
    - Vertex Shader: モデル、ビュー、プロジェクション行列を適用。
    - Fragment Shader: 単一の色の描画。
- **Geometry**:
    - 地面 (Ground): 平面を模した矩形。
    - ボール (Ball): 点質量の位置を表現する立方体。
- **Collision Detection**:
    - `main.cpp` 内で簡易的な地面との衝突判定を実装。
    - 衝突時、速度の反転（跳ね返り）と位置の補正を行い、地面への沈み込みを防ぎます。

## 技術仕様
- 言語: C++17
- グラフィックスAPI: OpenGL 3.3+
- ライブラリ: GLFW, GLAD, GLM
- ビルドシステム: CMake, vcpkg"