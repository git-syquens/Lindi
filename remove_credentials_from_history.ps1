# Script to remove WiFi credentials from git history
# WARNING: This rewrites git history - backup recommended

Write-Host "This script will rewrite git history to remove WiFi credentials." -ForegroundColor Yellow
Write-Host "This is a DESTRUCTIVE operation that rewrites history." -ForegroundColor Red
Write-Host ""
Write-Host "Before proceeding:"
Write-Host "1. Make sure you have a backup of your repository"
Write-Host "2. Close all applications using this repository"
Write-Host "3. All collaborators will need to re-clone after force push"
Write-Host ""
Write-Host "The following credentials will be removed from ALL commits:"
Write-Host '  WIFI_SSID: "interxs-ws300n"' -ForegroundColor DarkGray
Write-Host '  WIFI_PASS: "cdeccdecefgefggagfecgagfeccgccgc"' -ForegroundColor DarkGray
Write-Host ""

$confirm = Read-Host "Type 'YES' to continue"
if ($confirm -ne "YES") {
    Write-Host "Operation cancelled." -ForegroundColor Green
    exit 0
}

Write-Host "`nRemoving WiFi credentials from git history..." -ForegroundColor Cyan

# Use environment variable to suppress filter-branch warning
$env:FILTER_BRANCH_SQUELCH_WARNING = "1"

# Use a simpler approach with --tree-filter and inline sed
git filter-branch --force --tree-filter `
  'if [ -f main/main.c ]; then sed -i "s/#define WIFI_SSID      \"interxs-ws300n\"/#define WIFI_SSID      \"YOUR_WIFI_SSID\"/g; s/#define WIFI_PASS      \"cdeccdecefgefggagfecgagfeccgccgc\"/#define WIFI_PASS      \"YOUR_WIFI_PASSWORD\"/g" main/main.c; fi' `
  --tag-name-filter cat -- --all

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nHistory rewrite complete!" -ForegroundColor Green
    Write-Host "`nCleaning up backup references..." -ForegroundColor Cyan
    
    # Clean up
    Remove-Item -Recurse -Force .git/refs/original/ -ErrorAction SilentlyContinue
    git reflog expire --expire=now --all
    git gc --prune=now --aggressive
    
    Write-Host "`nNext steps:" -ForegroundColor Yellow
    Write-Host "1. Verify the changes with: git log --all --oneline -- main/main.c"
    Write-Host "2. Push to remote with: git push origin --force --all"
    Write-Host "3. Also push tags with: git push origin --force --tags"
    Write-Host ""
    Write-Host "IMPORTANT: All collaborators must re-clone the repository!" -ForegroundColor Red
} else {
    Write-Host "`nError occurred during history rewrite." -ForegroundColor Red
    Write-Host "Check for .git/refs/original/ to restore if needed"
}
