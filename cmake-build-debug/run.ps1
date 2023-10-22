$exe = "MetaheuristicsRepo.exe"
$configDir = "../MultiObjOpt/OptimizersConfigurations/GeneticAlgorithm/TTP_SO"
$ttpDir = "../MultiObjOpt/ProblemsDefinitions/TTP"
$outputDir = "../output/GA"

$cfgFiles = Get-ChildItem -Path $configDir -Filter *.cfg
$ttpFiles = Get-ChildItem -Path $ttpDir -Filter *.ttp

foreach ($cfgFile in $cfgFiles) {
    foreach ($ttpFile in $ttpFiles) {
        $outputSubDir = Join-Path $outputDir ("$($cfgFile.BaseName)_$($ttpFile.BaseName)")

        if (!(Test-Path -Path $outputSubDir -PathType Container)) {
            New-Item -Path $outputSubDir -ItemType Directory | Out-Null
        }

        Write-Host "Running $exe with arguments:"
        Write-Host "Config file: $($cfgFile.FullName)"
        Write-Host "TTP file: $($ttpFile.FullName)"
        Write-Host "Output directory: $outputSubDir"
        Write-Host "Number: 10"

        & $exe $cfgFile.FullName "TTP_CityOrderKnapBin_SO" $ttpFile.FullName $outputSubDir 10
    }
}
