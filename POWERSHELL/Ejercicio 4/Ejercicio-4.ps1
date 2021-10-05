
# Actividad Práctica de Laboratorio Nro. 2 - Ejercicio 4
# Fierro, Agustin Gabriel- 42.427.695
# Albanesi, Matias - 39.770.388
# Rodriguez, Ezequiel Nicolás - 40.135.570
# Jimenez Vitale, Matias - 34.799.834
# Cambiasso, Tomas - 41.471.465

<#
.SYNOPSIS
Este script sirve para recopilar la información de las distintas sucursales con las ventas totales de cada producto.
.DESCRIPTION
Este script genera un resumen en un archivo JSON llamado "salida.json"
.EXAMPLE
.\Ejercicio-4.ps1 -d ".\archivos\" -o ".\dirSalida"
.\Ejercicio-4.ps1 -d ".\archivos\" -e "Moron" -o ".\dirSalida"
#>

[CmdletBinding()]
Param (
    [Parameter(Mandatory = $True, Position = 0, HelpMessage = "Directorio donde se encuentran los archivos CSV de las sucursales.")]
    [ValidateScript({ Test-Path $_ })]
    [ValidateNotNullOrEmpty()]
    [string]$directorio,
    [Parameter(HelpMessage = "Parámetro opcional que indicará la exclusión de alguna sucursal a la hora de generar el archivo unificado.")]
    [ValidateNotNullOrEmpty()]
    [string]$excluir,
    [Parameter(Mandatory = $True, HelpMessage = "Directorio donde se generará el resumen (salida.csv).")]
    [ValidateNotNullOrEmpty()]
    [string]$out
)

function Capitalize($text) {
    if ($text.Length -gt 1) {
        return $text.substring(0, 1).toupper() + $text.substring(1).tolower()
    }
    else {
        return $text.ToUpper()
    }
}

#Write-Host "-directorio" $directorio
#Write-Host "-excluir" $excluir
#Write-Host "-out" $out

if ($directorio -eq $out) {
    Write-Error -Message 'El directorio de salida NO puede ser el mismo donde se encuentran los CSV, para evitar que se mezclen archivos.'
    return;
}

$hash = @{}

$archivos = @(Get-ChildItem -Path $directorio -Filter *.csv -Recurse)

$archivos | ForEach-Object {
    $nombre_archivo = $_.Name.Split(".")[0]
    #Write-Host $nombre_archivo
    #Write-Host $_.FullName

    if ([String]::IsNullOrEmpty((Import-Csv $_.FullName))) {
        Write-Warning -Message "El archivo $nombre_archivo esta vacio."
        return;
    }

    if (![String]::IsNullOrEmpty(($excluir))) {
        $excluir_capitalize = Capitalize $excluir;
        $nombre_capitalize = Capitalize $nombre_archivo;
        if ($excluir_capitalize -eq $nombre_capitalize) {
            return;
        }
    }

    Import-Csv $_.FullName -Delimiter ',' -Encoding UTF7 -Header PRODUCTO, CANTIDAD | Select-Object -Skip 1 | Foreach-Object {
        #Write-Host $_.IP1
        #Write-Host $_.IP2

        $producto = Capitalize $_.PRODUCTO
        #Write-Host $producto

        if ($hash.Contains($producto)) {
            $hash[$producto] = ([int]$hash[$producto] + [int]$_.CANTIDAD)
        }
        else {
            $hash.Add($producto, [int]$_.CANTIDAD)
        }
    }
}

# foreach ($i in $hash.Keys) {
#     Write-Host $i
#     Write-Host $hash[$i]
# }

if (-not(Test-Path -Path "$out\salida.json" -PathType Leaf)) {
    try {
        $null = New-Item -ItemType File -Path "$out\salida.json" -Force -ErrorAction Stop
    }
    catch {
        throw $_.Exception.Message
    }
}

#Write-Host "$out\salida.json"

$newhash = [ordered]@{}

$hash.GetEnumerator() | Sort-Object Name | ForEach-Object {
    $newhash.Add($_.Name, $_.Value);
}

$newhash | ConvertTo-Json | Out-File -FilePath "$out\salida.json"

#Get-Content "$out\salida.json"

