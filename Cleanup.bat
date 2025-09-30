@echo off

echo Deleting saved folder
rmdir /S /Q Saved

echo Deleting intermediate folder
rmdir /S /Q Intermediate/ProjectFiles

echo Deleting editor folders
rmdir /S /Q .idea
rmdir /S /Q .vs

echo Deleting solution
del /F /Q .vsconfig
del /F /Q *.sln
del /F /Q *.DotSettings.user

echo Cleanup complete
PAUSE