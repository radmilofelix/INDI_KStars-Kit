ECHO OFF
CLS
SET CURRENTFOLDER=%cd%
SET PROJECTSFOLDER=D:\Seafile\_Projects-s\

ECHO %CURRENTFOLDER%
ECHO %PROJECTSFOLDER%

rd "%CURRENTFOLDER%\my-indi-drivers\_classes" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-dewheaterandfan" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-flatspannel" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-regulusfocuser" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-rormag-ip" /s /q
rd "%CURRENTFOLDER%\my-indi-drivers\indi-rorweather-ip" /s /q

robocopy "%PROJECTSFOLDER%\_classes" "%CURRENTFOLDER%\my-indi-drivers\_classes" /e
robocopy "%PROJECTSFOLDER%\DewController\Drivers\indi-dewheaterandfan" "%CURRENTFOLDER%\my-indi-drivers\indi-dewheaterandfan" /e
robocopy "%PROJECTSFOLDER%\RegulusElectronicFocuser\Drivers\indi-regulusfocuser" "%CURRENTFOLDER%\my-indi-drivers\indi-regulusfocuser" /e
robocopy "%PROJECTSFOLDER%\FlatsPannel\Drivers\indi-flatspannel" "%CURRENTFOLDER%\my-indi-drivers\indi-flatspannel" /e
robocopy "%PROJECTSFOLDER%\ROR-Observatory-V2\Drivers\indi-rormag-ip" "%CURRENTFOLDER%\my-indi-drivers\indi-rormag-ip" /e
robocopy "%PROJECTSFOLDER%\ROR-Observatory-V2\Drivers\indi-rorweather-ip" "%CURRENTFOLDER%\my-indi-drivers\indi-rorweather-ip" /e

:ENDPROG
ECHO Sources were pulled from target.
pause

