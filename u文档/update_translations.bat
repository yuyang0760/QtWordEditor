@echo off
setlocal

REM 设置Qt路径（请根据实际安装路径修改）
set QT_PATH=E:\Qt\6.8.2\mingw_64

REM 进入项目根目录
cd /d "%~dp0"

REM 创建translations目录（如果不存在）
if not exist "translations" mkdir translations

REM 使用lupdate生成或更新.ts文件
echo 正在更新翻译文件...
"%QT_PATH%\bin\lupdate.exe" . -ts translations\QtWordEditor_zh_CN.ts -no-obsolete
"%QT_PATH%\bin\lupdate.exe" . -ts translations\QtWordEditor_en_US.ts -no-obsolete

echo 翻译文件已更新完成！
echo 请使用Qt Linguist打开.ts文件进行翻译编辑。
pause