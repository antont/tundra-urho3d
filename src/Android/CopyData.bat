md assets\Data
xcopy ..\..\Bin\Data\*.* assets\Data /S /E /C /Y
xcopy ..\..\Bin\*.json assets /S /E /C /Y
rd /S /Q assets\Data\Shaders\HLSL
