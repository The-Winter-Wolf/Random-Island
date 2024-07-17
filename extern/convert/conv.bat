@echo off

set path=%~d0%~p0

cd %path%..\assets

echo ////////////// Create all the directories we need //////////////
if not exist %cd%\compressed\lossy\DDS (md %cd%\compressed\lossy\DDS) 
if not exist %cd%\compressed\lossless\DDS (md %cd%\compressed\lossless\DDS)

set "lossy_path=%cd%\compressed\lossy\"
set "lossless_path=%cd%\compressed\lossless\"
set "lossyDDS_path=%cd%\compressed\lossy\DDS\"
set "losslessDDS_path=%cd%\compressed\lossless\DDS\"

echo ////////////// Lossless compression using optipng //////////////
for /R %%G in (*.png) do (
	if %%~dpG neq %lossless_path% (
		if %%~dpG neq %lossy_path% (
			echo "compress %%~nxG"
			%path%optipng -o1 %%~fG -out %%~dpnG-lossless.png
			move "%%~dpnG-lossless.png" "%lossless_path%"		
)))

echo ////////////// Lossy compression using pngquant //////////////
for /R %%G in (*.png) do ( 
	if %%~dpG neq %lossless_path% (
		if %%~dpG neq %lossy_path% (
			echo "compress %%~nxG"
			%path%pngquant --speed 1 --quality=65-80 %%~fG --ext -lossy.png
			move "%%~dpnG-lossy.png" "%lossy_path%"
)))

cd compressed

echo ////////////// Conversion textures to DDS format //////////////

for /R %%G in (*.png) do ( 
	echo "converse %%~nxG"
	%path%PVRTexToolCLI -i %%~fG -f RGBG8888 -o %%~fG.dds
	move "%%~fG.dds" "%%~dpG\DDS"
)

cd..\..\convert