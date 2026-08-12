# C++ 実装ガイドライン & コーディングルール (MyPhysicalEngine)

このドキュメントは、本物理エンジンプロジェクト (MyPhysicalEngine) において C++ コードを実装・リファクタリングする際の品質・設計ルールを定めたものです。

---

## 1. モダン C++ & RAII (C++17)

- **メモリ管理と所有権**:
  - 生の `new` / `delete` の使用は厳禁。生ポインタによる動的確保は行わず、`std::unique_ptr` や `std::shared_ptr` などのスマートポインタを原則使用すること。
  - 所有権の移動には `std::move` を明示的に使用すること。
  - ポインタの所有権を持たない参照用渡しには、`const T&` または生の `T*`（非所有の参照用）を使用すること。
- **RAII パターン**:
  - OpenGL リソース（VAO, VBO, EBO, シェーダープログラム）やファイル/パイプハンドル（`FILE*` 等）は、必ずデストラクタで確実に破棄・解放される RAII クラスでカプセル化すること。
- **キーワードの徹底**:
  - メンバ関数変更のない関数には `const` 修飾子を必ず付与すること。
  - 派生クラスのオーバーライド関数には `override` を明示すること。
  - 例外を投げない関数には `noexcept` を適切に使用すること。

---

## 2. 命名規則 & ディレクトリ構造

- **識別子の命名規則**:
  - **クラス / 構造体 / Enum**: `PascalCase` (例: `PhysicsWorld`, `PointMass`, `SphereMesh`)
  - **関数 / メソッド**: `camelCase` (例: `addPointMass()`, `getPosition()`, `step()`)
  - **メンバ変数**: `camelCase` (例: `position`, `velocity`, `invMass`)
  - **定数 / マクロ**: `UPPER_SNAKE_CASE` (例: `MAX_SUB_STEPS`)
- **インクルードパス**:
  - インクルードパスには Windows スタイルのバックスラッシュ (`\`) は使用せず、**必ずスラッシュ (`/`) に統一** すること（クロスプラットフォーム互換性のため）。
  - 例: `#include "PhysicsEngine/Physics/PhysicsWorld.hpp"`
- **ヘッダーガード**:
  - ヘッダーファイルの先頭には `#pragma once` を記載すること。
- **ネームスペース**:
  - すべての物理エンジンコードは `PhysicsEngine` ネームスペース内（必要に応じて `PhysicsEngine::Math`, `PhysicsEngine::Physics`, `PhysicsEngine::Geometry`, `PhysicsEngine::Utils` などサブネームスペース）に配置すること。

---

## 3. 物理エンジンのパフォーマンス & アルゴリズム規約

- **高速物理ループ内のメモリ動的割当の禁止**:
  - シミュレーションステップ (`PhysicsWorld::step()` 等の毎フレーム実行されるループ内) での `new` や `std::vector::push_back` による動的メモリ確保を避けること。
  - 必要なバッファは初期化時またはワールド構築時にあらかじめリザーブ (`reserve()`) または固定領域確保すること。
- **数値積分法とエネルギー安定性**:
  - オイラー法（Forward Euler）による積分はエネルギーが過大膨張するため避け、数値的安定性の高い **Symplectic Euler (Semi-implicit Euler)** または **Position Based Dynamics (PBD)** アルゴリズムに従うこと。
- **インライン化と値渡し**:
  - `Vector3` などの軽量な数学構造体は値渡し（Value pass）または `const Vector3&` で効率的に受け渡すこと。

---

## 4. エラーハンドリング & ディバッギング

- **ビルド警告ゼロの維持**:
  - すべてのコードは MSVC / GCC / Clang において警告ゼロでビルドできる状態を維持すること。非安全関数警告等が出た場合は適切に対処すること。
- **OpenGL エラーチェック**:
  - グラフィックス関連の呼び出しではデバッグビルド時にエラーチェックを行い、未定義動作を未然に防止すること。
