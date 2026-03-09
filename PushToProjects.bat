ECHO OFF
CLS
SET CURRENTFOLDER=%cd%
SET PROJECTSFOLDER=D:\Seafile\_Projects-s\

ECHO %CURRENTFOLDER%
ECHO %PROJECTSFOLDER%

rd "%PROJECTSFOLDER%\_classes" /s /q
rd "%PROJECTSFOLDER%\DewController\Drivers\indi-dewheaterandfan" /s /q
rd "%PROJECTSFOLDER%\RegulusElectronicFocuser\Drivers\indi-regulusfocuser" /s /q
rd "%PROJECTSFOLDER%\FlatsPannel\Drivers\indi-flatspannel" /s /q
rd "%PROJECTSFOLDER%\ROR-Observatory-V2\Drivers\indi-rormag-ip" /s /q
rd "%PROJECTSFOLDER%\ROR-Observatory-V2\Drivers\indi-rorweather-ip" /s /q

robocopy "%CURRENTFOLDER%\my-indi-drivers\_classes" "%PROJECTSFOLDER%\_classes" /e
robocopy "%CURRENTFOLDER%\my-indi-drivers\indi-dewheaterandfan" "%PROJECTSFOLDER%\DewController\Drivers\indi-dewheaterandfan" /e
robocopy "%CURRENTFOLDER%\my-indi-drivers\indi-flatspannel" "%PROJECTSFOLDER%\FlatsPannel\Drivers\indi-flatspannel" /e
robocopy "%CURRENTFOLDER%\my-indi-drivers\indi-regulusfocuser" "%PROJECTSFOLDER%\RegulusElectronicFocuser\Drivers\indi-regulusfocuser" /e
robocopy "%CURRENTFOLDER%\my-indi-drivers\indi-rormag-ip" "%PROJECTSFOLDER%\ROR-Observatory-V2\Drivers\indi-rormag-ip" /e
robocopy "%CURRENTFOLDER%\my-indi-drivers\indi-rorweather-ip" "%PROJECTSFOLDER%\ROR-Observatory-V2\Drivers\indi-rorweather-ip" /e

SET /P M=- Delete driver sources from current folder? Type y to delete or n to skip, then press ENTER: 
IF "%M%" NEQ "y" GOTO ENDPROG
rd "%CURRENTFOLDER%\my-indi-drivers\_classes" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-dewheaterandfan" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-flatspannel" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-regulusfocuser" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-rormag-ip" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-rorweather-ip" /s /q
ECHO Current location sources purged.

:ENDPROG
ECHO Sources were pushed to target.
pause


