param(
    [string]$RemoteUrl = "",
    [string]$Branch = "main"
)

if ([string]::IsNullOrWhiteSpace($RemoteUrl)) {
    $RemoteUrl = Read-Host "Enter your GitHub repository URL (example: https://github.com/yourname/optilink-modem.git)"
}

if ([string]::IsNullOrWhiteSpace($RemoteUrl)) {
    Write-Error "A GitHub repository URL is required."
    exit 1
}

Write-Host "Switching branch to $Branch..."
git branch -M $Branch

Write-Host "Configuring remote origin..."
try {
    git remote remove origin 2>$null
} catch {}
git remote add origin $RemoteUrl

Write-Host "Pushing to GitHub..."
git push -u origin $Branch
