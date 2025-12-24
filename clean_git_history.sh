#!/bin/bash
# This script rewrites git history to remove WiFi credentials

echo "Removing WiFi credentials from git history..."

export FILTER_BRANCH_SQUELCH_WARNING=1

git filter-branch --force --tree-filter '
if [ -f main/main.c ]; then 
    sed -i "s/WIFI_SSID[[:space:]]*\"interxs-ws300n\"/WIFI_SSID      \"YOUR_WIFI_SSID\"/g" main/main.c
    sed -i "s/WIFI_PASS[[:space:]]*\"cdeccdecefgefggagfecgagfeccgccgc\"/WIFI_PASS      \"YOUR_WIFI_PASSWORD\"/g" main/main.c
fi
' --tag-name-filter cat -- --all

echo "Cleaning up..."
rm -rf .git/refs/original/
git reflog expire --expire=now --all
git gc --prune=now --aggressive

echo "Done! Push with: git push origin --force --all"
