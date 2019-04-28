robocopy.exe "..\..\..\bin\Bodyguard\Deploy" "..\..\..\deploy\Bodyguard" "Bodyguard.dll" "Bodyguard.pdb"
if %errorlevel% geq 8 exit /b %errorlevel%
robocopy.exe "..\..\..\resources\Bodyguard" "..\..\..\deploy\Bodyguard" "config.json" "PluginInfo.json" "readme.txt"
if %errorlevel% geq 8 exit /b %errorlevel%
exit /b 0