# PhysicsEngine - 物理エンジン開発プロジェクト

このプロジェクトは、PBD (Position Based Dynamics) をベースとした物理エンジンを開発するためのプロジェクトです。

## 開発環境の前提条件
ビルドを行うには、以下の環境が構築されている必要があります。

- **Visual Studio 2022** (C++ によるデスクトップ開発ワークロードを含む)
- **CMake** (3.20 以上を推奨)
- **vcpkg** (ライブラリ管理システム)
    - インストール済みパスの例: `<VCPKG_PATH>`

## ライブラリのセットアップ
このプロジェクトでは `vcpkg` を使用してライブラリを管理します。以下のライブラリがインストールされているか確認してください。

```powershell
<VCPKG_PATH>/vcpkg.exe list glfw3 glm glad
```

もしインストールされていない場合は、以下のコマンドを実行してください（パスはご自身の環境に合わせて書き換えてください）。

```powershell
<VCPKG_PATH>/vcpkg.exe install glfw3:x64-windows glm:x64-windows glad:x64-windows
```

## ビルド手順
ビルドを行う際は、`vcpkg` のツールチェーンファイルを正しく指定する必要があります。

1. **ビルド用ディレクトリの作成とクリーンアップ**
   プロジェクトのルートディレクトリに移動し、`build` フォルダを作成（または中身を削除）します。

   ```powershell
   cd <PROJECT_ROOT>
   Remove-Item -Recurse -Force build
   mkdir build
   cd build
   ```

2. **CMake の構成 (Configure)**
   以下のコマンドを実行して、絶対パスを用いてプロジェクトを構成します。
   ※ `<VCPKG_PATH>` は環境に合わせて書き換えてください。

   ```powershell
   cmake .. -DCMAKE_TOOLCHAIN_FILE=<VCPKG_PATH>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_PREFIX_PATH=<VCPKG_PATH>/installed/x64-windows
   ```

3. **ビルドの実行**
   構成が成功したら、ビルドを実行します。

   ```powershell
   cmake --build .
   ```

## 実行方法
ビルドが正常に完了すると、`build` ディレクトリ内に `PhysicsSim.exe` が生成されます。

```powershell
.\PhysicsSim.exe
```

## aider 実行方法
下記コマンドによりaiderを起動する

```powershell
aider --yes-always --no-show-model-warnings --model ollama/gemma4:12b
```
