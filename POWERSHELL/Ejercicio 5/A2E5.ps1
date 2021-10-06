<#
    .SYNOPSIS
    Genera un archivo en formato zip para cada directorio pasado, comprimiendo los archivos de log antiguos de cada carpeta de logs y los almacena en otra carpeta.
    .DESCRIPTION
    El proceso recibirá como parámetro 1 archivo de configuración que contendrá:
        • En la primera línea la carpeta de destino de los archivos comprimidos. 
        • A partir de la segunda línea, las rutas donde se encuentran las carpetas de logs a analizar.
    Ejemplo del archivo de configuración:
        ./Datos/Salida/
        ./Servicio1/carpeta_con_logs1/
        ./Servicio2/carpeta_con_logs2/
        ./Servicio3/carpeta_con_logs3/
#>

[CmdletBinding()]
Param (
    [Parameter(Mandatory=$true)]        
    [ValidateScript({
        if(-Not ($_ | Test-Path)){
            throw "El archivo no existe."
        }
        if(-Not ($_ | Test-Path -PathType Leaf)){
            throw "La ruta debe indicar un archivo, no una carpeta."
        }
        return $true;
    })]
    [System.IO.FileInfo]$conf
)

function ValidarPath {
    param (
        [String]$path
    )

    if(-Not ($path | Test-Path)){
        throw "El archivo no existe. (( $path ))"
    }
    if( $path | Test-Path -PathType Leaf ){
        throw "La ruta debe indicar una carpeta, no un archivo"
    }
    Write-Host "Ruta Valida";
}

[string[]]$lineas_conf = Get-Content $conf;

if ($lineas_conf.Length -lt 2){
    throw "El archivo de configuración debe tener al menos dos parametros"
}

$lineas_conf | ForEach-Object -Process {ValidarPath($_)};


$salida = $lineas_conf[0];
$entradas = $lineas_conf[1..$lineas_conf.Length];

#Calcular fecha de ejecucion
$fecha_ejecucion = Get-Date -Format "yyyyMMdd_HHmmss";

foreach ($entrada in $entradas) {
    
    $path_zip = "$((get-item $salida).FullName)logs_$((get-item $entrada ).Parent.Name)_$fecha_ejecucion";

    Write-Host "Se procesara $path_zip";

    #Archivos del directorio que cumplen con las condiciones solicitadas.
    # Verifica que la fecha de modificacion de los archivos no sea en el dia en que se corre el script
    $archivos = @(Get-ChildItem -Path "$((get-item $entrada).FullName)/*" -Include "*.txt","*.log","*.info" | Where-Object { -not ($_.LastWriteTime.Date -eq [datetime]::Today )})
    
    if ($archivos.Length -eq 0 ){
        #Si no hay archivos salgo al siguiente item para que el compreson no de error.
        continue;
    }
    
    $compress = @{
        Path = $archivos
        CompressionLevel = "Fastest"
        DestinationPath = $path_zip
      }
    Compress-Archive @compress 

    # Comentado momentaneamente para testear
    #Remove-Item $archivos;
}

