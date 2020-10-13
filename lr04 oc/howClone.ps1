param
(
[string]$path
)
#Get-Item -Path $path

get-childitem $path | foreach-object {
$getobject=$_;
get-childitem $path | 
foreach-object {
if ($_.length -eq $getobject.length -and $getobject.name -ne $_.name) 
{$getobject.name}}} |sort  | get-unique