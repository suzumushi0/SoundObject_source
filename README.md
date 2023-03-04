# SoundObject source code distribution.

## Licence.

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a> at no charge.

## How to build SoundObject.

**(1) You will need the following tools.**

Visual Studio 2022.

VST SDK.

https://steinbergmedia.github.io/vst3_dev_portal/pages/Getting+Started/How+to+setup+my+system.html

**(2) Create a VST3 project template using VST3 SDK Project Generator with the following parameters.**

Name: SoundObject

Type: Audio Effect

Use VST GUI: Check

C++ Class Name: SoundObject

Bundle ID: NA

Filename Prefix: SO

Output Directory: as you like

CMake Generator: Visual Studio 17 2022

**(3) Setup files.**

copy `source/*, resource/*, and CMakeList.txt` from GitHub.

add `.h and .cpp files` to the Visual Studio project.

update vst3sdk_SOURCE_DIR absolute path in `CMakeList.txt`.

update absolute path in `resource\SOeditor.uidesc`.

update relative paths for png file in `resource\SoundObject.uidesc` to absolute paths.

**(4) Setup property.**

Select C++ latest from the property.

**(5) Build.**

## SoundObject のビルド方法

**(1) 以下のツールが必要です．**

Visual Studio 2022.

VST SDK.

https://steinbergmedia.github.io/vst3_dev_portal/pages/Getting+Started/How+to+setup+my+system.html

**(2) VST3 SDK Project Generator により以下のパラメータで VST3 プロジェクトテンプレートを生成する．**

Name: SoundObject

Type: Audio Effect

Use VST GUI: チェック

C++ Class Name: SoundObject

Bundle ID: NA

Filename Prefix: SO

Output Directory: お好きにどうぞ

CMake Generator: Visual Studio 17 2022

**(3) ファイルのセットアップ．**

GitHub から `source/*, resource/*, CMakeList.txt` をコピー．

Visual Studio のプロジェクトに `.h と.cpp ファイル`を加える．

`CMakeList.txt` の中の絶対パス vst3sdk_SOURCE_DIR を更新．

`resource\SOeditor.uidesc` の中の絶対パスを更新．

`resource\SoundObject.uidesc` の中の png ファイルへの相対パスを絶対パスに更新．

**(4) プロパティの設定．**

プロパティから最新版の C++ を選択．

**(5) ビルド．**

---
<img width="100" src="https://user-images.githubusercontent.com/67182469/130337395-b8ab38cd-e66e-4056-b441-49d33337410e.png">
VST is a registered trademark of Steinberg Media Technologies GmbH.
