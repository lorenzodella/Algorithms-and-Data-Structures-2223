@echo off
setlocal enabledelayedexpansion

REM Determina il file sorgente C da compilare (default: main.c)
set "SRC=main.c"
if not "%~1"=="" (
    if exist "%~1" (
        set "SRC=%~1"
    ) else if exist "%~1.c" (
        set "SRC=%~1.c"
    ) else (
        echo [ERRORE] File sorgente non trovato: %~1
        exit /b 1
    )
)

REM Crea la cartella out se non esiste
if not exist out mkdir out

REM Compila il programma scelto
echo Compilazione di %SRC% in corso...
gcc -O3 -Dgetchar_unlocked=_getchar_nolock "%SRC%" -o main.exe
if errorlevel 1 (
    echo Errore di compilazione di %SRC%.
    exit /b 1
)

set /a total=0
set /a passed=0

echo.
echo Esecuzione dei test con %SRC%...
echo.

REM Itera su tutti i test da open_1 a open_112 (o tutti i file test\open_*.txt)
for %%f in (test\open_*.txt) do (
    set "fname=%%~nxf"
    REM Filtra escludendo i file che contengono .output.txt nel nome
    if "!fname!"=="!fname:.output.txt=!" (
        call :run_single_test "%%f" "%%~nf"
    )
)

echo.
echo ========================
echo Risultati del Test
echo ========================
echo Totali: !total!
echo Passati: !passed!
set /a failed=total-passed
echo Falliti: !failed!
echo ========================

pause
exit /b 0

:run_single_test
set "in_file=%~1"
set "test_name=%~2"
set "exp_file=test\%test_name%.output.txt"
set "out_file=out\%test_name%.out.txt"

REM Esegue il test con reindirizzamento standard input e output
main.exe < "%in_file%" > "%out_file%"

REM Confronta l'output ottenuto con quello atteso
if exist "%exp_file%" (
    REM Usa git diff ignorando le differenze di fine linea (CRLF vs LF)
    git diff --no-index --quiet --ignore-cr-at-eol "%exp_file%" "%out_file%" 2>nul
    if errorlevel 1 (
        echo [FAIL] %test_name%
    ) else (
        echo [PASS] %test_name%
        set /a passed+=1
    )
) else (
    echo [SKIP] %test_name% (manca file output atteso^)
)
set /a total+=1
exit /b 0
