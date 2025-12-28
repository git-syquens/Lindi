@echo off
call e:\Dev\Espressif\frameworks\esp-idf-v5.5\export.bat
cd /d e:\Dev\Lindi
idf.py -p COM7 build flash monitor
