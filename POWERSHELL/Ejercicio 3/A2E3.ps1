<#
.SYNOPSIS
       	Script que renombra archivos.

.DESCRIPTION
	Para mejorar el script que ya hizo, el influencer del ejercicio 2 decide agregar una nueva funcionalidad
	a su script: monitoreo de directorios en tiempo real.
	Implementar sobre el script del ejercicio anterior un mecanismo que permite renombrar los archivos a
	medida que se vayan copiando/moviendo/creando en el directorio de las fotos. Esto se debe realizar
	utilizando los eventos provistos por FileSystemWatcher.
.INPUTS 
        Posee los siguientes parametros
        -Directorio: Carpeta de archivos a procesar.
	-Dia: (Opcional) Es el nombre de un día para el cual no se quieren renombrar los archivos. Los valores posibles para este parámetro son los nombres de los días de la semana (sin tildes). El script tiene que funcionar correctamente sin importar si el nombre del día está en minúsculas o mayúsculas.
	-k: Detiene el proceso.

#>

# Script Parameters
param(
	[Parameter(Mandatory=$False)][String]$Directorio,
	[Parameter(Mandatory=$False)][String]$Dia,
	[Parameter(Mandatory=$False)][Switch]$k
)

$global:Hugin=$Directorio
$global:Munin=$Dia

$DB="Sábado Domingo Lunes Martes Miercoles Jueves Viernes"

function Confirm-Directorio{
	if(!(Test-Path $Directorio)){
		Write-Host "Directorio inválido" -ForegroundColor red
		exit 1
	}
	$global:Hugin=Resolve-Path -path $global:Hugin
}

function Confirm-Dia{
	if(!$Dia -or $Dia -eq ""){
		return
	}
	if(!($DB -Match $Dia)){
		Write-Host "Día invalido" -ForegroundColor red
		exit 1
	}
}

function End-Job {
	if(!(Test-Path $PSScriptRoot"/.A2E3log")){
		Write-Host "No se esta ejecutando el script" -ForegroundColor red
		exit 1
	}
	$id = Get-Content $PSScriptRoot"/.A2E3log" -First 1
	stop-job -Id $id
	remove-job -Id $id
	remove-item $PSScriptRoot"/.A2E3log" -Force
	exit 1
}


function schmoving($D1,$D2){
	Get-ChildItem $D1 -File | Foreach-Object {
		$time = "Almuerzo"
		$name = $_.Name
		$ext = $name.Substring($name.LastIndexOf(".")+1)
		$day = $name.Substring(6, 2);
		$month = $name.Substring(4, 2);
		$year = $name.Substring(0, 4);
		$hour = $name.Substring(9, 2);
		$minute = $name.Substring(11, 2);
		$second = $name.Substring(13, 2);
		$value= (($hour -as [int]) * 3600) + (($minute  -as [int]) * 60) + ($second  -as [int])
	
		
		if($name -Match "del"){
			return
		}
	
		if($value -lt 39600){
			$time = "Desayuno"
		}
		if($value -gt 68400){
			$time = "Cena"
		}
		$dayofweek= (get-date -year $year -month $month -day $day).DayOfWeek
		switch ( $dayofweek )
		{
		    'Sunday' { $dayofweek= 'domingo'    }
		    'Monday' { $dayofweek= 'lunes'    }
		    'Tuesday' { $dayofweek= 'martes'   }
		    'Wednesday' { $dayofweek= 'miercoles' }
		    'Thursday' { $dayofweek= 'jueves'  }
		    'Friday' { $dayofweek= 'viernes'    }
		    'Saturday' { $dayofweek= 'sabado'  }
		}
		if($D2 -ne "" -and $dayofweek.ToUpper() -eq $D2.ToUpper() ){
			return
		}
		$dayofweek= "$($day)-$($month)-$($year) $($time) del $($dayofweek)"
		if(Test-Path "$($D1)/$($dayofweek).$($ext)"){
			$i=1;
			While(Test-Path "$($D1)/$($dayofweek)($($i)).$($ext)"){
				$i++;
			}
			$dayofweek="$($dayofweek)($($i)).$($ext)"	
		}else{
			$dayofweek="$($dayofweek).$($ext)"	
		}
		Rename-Item -Path $_ -NewName $dayofweek	
	}
}

function OCD{

	$fsw = New-Object System.IO.FileSystemWatcher 
	$fsw.Path = Resolve-Path -path $global:Hugin
	$fsw.Filter = ""
	$fsw.IncludeSubDirectories = $True  
	$fsw.EnableRaisingEvents = $True
	
	schmoving $global:Hugin $global:Munin
	
	$pso = new-object psobject -property @{D1 = $global:Hugin; D2 = $global:Munin}
	$job = Register-ObjectEvent -InputObject $fsw -EventName Created -MessageData $pso -Action {
			Get-ChildItem $event.MessageData.D1 -File | Foreach-Object {
				$time = "Almuerzo"
				$name = $_.Name
				$ext = $name.Substring($name.LastIndexOf(".")+1)
				$day = $name.Substring(6, 2);
				$month = $name.Substring(4, 2);
				$year = $name.Substring(0, 4);
				$hour = $name.Substring(9, 2);
				$minute = $name.Substring(11, 2);
				$second = $name.Substring(13, 2);
				$value= (($hour -as [int]) * 3600) + (($minute  -as [int]) * 60) + ($second  -as [int])
			
				
				if($name -Match "del"){
					return
				}
			
				if($value -lt 39600){
					$time = "Desayuno"
				}
				if($value -gt 68400){
					$time = "Cena"
				}
				$dayofweek= (get-date -year $year -month $month -day $day).DayOfWeek
				switch ( $dayofweek )
				{
				    'Sunday' { $dayofweek= 'domingo'    }
				    'Monday' { $dayofweek= 'lunes'    }
				    'Tuesday' { $dayofweek= 'martes'   }
				    'Wednesday' { $dayofweek= 'miercoles' }
				    'Thursday' { $dayofweek= 'jueves'  }
				    'Friday' { $dayofweek= 'viernes'    }
				    'Saturday' { $dayofweek= 'sabado'  }
				}
				if($event.MessageData.D2 -ne "" -and $dayofweek -eq $event.MessageData.D2 ){
					return
				}
				$dayofweek= "$($day)-$($month)-$($year) $($time) del $($dayofweek)"
				if(Test-Path "$($event.MessageData.D1)/$($dayofweek).$($ext)"){
					$i=1;
					While(Test-Path "$($event.MessageData.D1)/$($dayofweek)($($i)).$($ext)"){
						$i++;
					}
					$dayofweek="$($dayofweek)($($i)).$($ext)"	
				}else{
					$dayofweek="$($dayofweek).$($ext)"	
				}
				Rename-Item -Path $_ -NewName $dayofweek	
			}
	}
	$job.Id | Out-file -FilePath $PSScriptRoot"/.A2E3log"
}

if(!$Directorio -and !$k){
	Write-Host "Ingrese Parámetros para ejecutar"
	exit 1
}elseif(!$Directorio -and !$Dia -and $k -eq $true){
	End-Job 
}elseif(($Directorio -or $Dia) -and $k){
	Write-Host "Demasiados parametros para ejecutar -k" -ForegroundColor red
	exit 1
}
if(Test-Path $PSScriptRoot"/.A2E3log"){
	Write-Host "El proceso ya esta en ejecución" -ForegroundColor red
	exit 1
}

Confirm-Directorio 
Confirm-Dia
OCD
