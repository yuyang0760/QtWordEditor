
# Convert all files to UTF-8 with CRLF line endings
# Version 3 - Include project root files

$ErrorActionPreference = "Continue"
$rootPath = "d:\vscodeproject\QtWordEditor"
$includePath = Join-Path $rootPath "include"
$srcPath = Join-Path $rootPath "src"

Write-Host "=== Converting all files to UTF-8 with CRLF line endings ==="
Write-Host "Root path: $rootPath"
Write-Host ""

# Get all relevant files
$files = @()
# C/C++ source files
$files += Get-ChildItem -Path $includePath -Filter "*.h" -Recurse -File
$files += Get-ChildItem -Path $srcPath -Filter "*.cpp" -Recurse -File
# Project files in root
$rootFiles = @("CMakeLists.txt", "CMakePresets.json", "*.md", "*.bat")
foreach ($pattern in $rootFiles) {
    $files += Get-ChildItem -Path $rootPath -Filter $pattern -File -ErrorAction SilentlyContinue
}

Write-Host "Found $($files.Count) total files to process"
Write-Host ""

$processedCount = 0
$errorCount = 0

foreach ($file in $files) {
    try {
        # Try multiple times in case file is locked
        $attempts = 0
        $maxAttempts = 3
        $success = $false
        
        while ($attempts -lt $maxAttempts -and -not $success) {
            try {
                # Read the file content with UTF-8 encoding
                $content = [System.IO.File]::ReadAllText($file.FullName, [System.Text.Encoding]::UTF8)
                
                # Normalize line endings - handle all cases
                $content = $content -replace "`r`n", "`n"
                $content = $content -replace "`r", "`n"
                $content = $content -replace "`n", "`r`n"
                
                # Write back with UTF-8 without BOM
                $utf8NoBom = New-Object System.Text.UTF8Encoding $false
                [System.IO.File]::WriteAllText($file.FullName, $content, $utf8NoBom)
                
                $success = $true
                $processedCount++
                Write-Host "[OK] $($file.FullName)"
            }
            catch {
                $attempts++
                if ($attempts -lt $maxAttempts) {
                    Start-Sleep -Milliseconds 100
                }
            }
        }
        
        if (-not $success) {
            $errorCount++
            Write-Host "[ERROR] Failed to process: $($file.FullName)" -ForegroundColor Red
        }
    }
    catch {
        $errorCount++
        Write-Host "[ERROR] $($file.FullName): $_" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== Summary ==="
Write-Host "Processed: $processedCount"
Write-Host "Errors: $errorCount"
Write-Host "Total: $($files.Count)"
Write-Host ""

if ($errorCount -eq 0) {
    Write-Host "All files successfully converted to UTF-8 with CRLF line endings!" -ForegroundColor Green
} else {
    Write-Host "Some files could not be processed. Please close any open files and try again." -ForegroundColor Yellow
}

