@echo off
setlocal

REM 设置Qt路径（请根据实际安装路径修改）
set QT_PATH=E:\Qt\6.8.2\mingw_64

REM 进入项目根目录
cd /d "%~dp0"

REM 编译.ts文件为.qm文件
echo 正在编译翻译文件...
"%QT_PATH%\bin\lrelease.exe" translations\QtWordEditor_zh_CN.ts -qm translations\QtWordEditor_zh_CN.qm
"%QT_PATH%\bin\lrelease.exe" translations\QtWordEditor_en_US.ts -qm translations\QtWordEditor_en_US.qm

echo 翻译文件编译完成！
pause