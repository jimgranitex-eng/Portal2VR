@echo off
set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
%MSBUILD% S:\p2s\l4d2vr.sln /p:Configuration=Release /p:Platform=x86 /m /v:minimal
%MSBUILD% S:\p2s\l4d2vr.sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal
