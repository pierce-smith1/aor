@echo off

for %%I in (%CD%\assets\img\items\*.png) do (
    convert %%I -fill "#000000" -colorize 100 %CD%\assets\img\items\sil\%%~nxI
)

echo ^<!DOCTYPE RCC^>^<RCC version="1.0"^>^<qresource^> > images.qrc
for %%I in (%CD%\assets\img\items\*.png) do (
    echo ^<file^>assets/img/items/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\items\sil\*.png) do (
    echo ^<file^>assets/img/items/sil/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\icons\*.png) do (
    echo ^<file^>assets/img/icons/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\lk\*.png) do (
    echo ^<file^>assets/img/lk/%%~nxI^</file^> >> images.qrc
)
for %%I in (%CD%\assets\img\style\*.png) do (
    echo ^<file^>assets/img/style/%%~nxI^</file^> >> images.qrc
)

echo ^</qresource^>^</RCC^> >> images.qrc

qmake
nmake release

mkdir release\deploy
copy release\desktop-yokin.exe release\deploy
cd release\deploy
windeployqt desktop-yokin.exe
cd ..\..
