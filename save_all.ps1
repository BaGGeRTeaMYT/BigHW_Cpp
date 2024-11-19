git add *;
$commit_message = Read-Host "Enter commit message" 
git commit -m $commit_message;
git push;
Set-Location "C:\Program Files (x86)\Steam\steamapps\common\dota 2 beta\game\bin\win64";
./dota2.exe