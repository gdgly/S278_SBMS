set MATLAB=D:\Program\Matlab2015

cd .

if "%1"=="" (D:\Program\MATLAB~1\bin\win64\gmake -f Q1.mk all) else (D:\Program\MATLAB~1\bin\win64\gmake -f Q1.mk %1)
@if errorlevel 1 goto error_exit

exit /B 0

:error_exit
echo The make command returned an error of %errorlevel%
An_error_occurred_during_the_call_to_make
