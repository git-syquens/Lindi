# Build and flash script for Lindi
& e:\Dev\Espressif\frameworks\esp-idf-v5.5\export.ps1
cd e:\Dev\Lindi
idf.py -p COM7 build flash monitor
