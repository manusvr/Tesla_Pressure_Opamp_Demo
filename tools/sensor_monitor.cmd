@echo off
rem Live sensor monitor: prints one fresh line of 29 sensor values per second from COM4.
rem Usage: sensor_monitor.cmd [COMport]   (default COM4). Stop with Ctrl+C.
set PORT=%1
if "%PORT%"=="" set PORT=COM4
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$port = New-Object System.IO.Ports.SerialPort '%PORT%',115200,'None',8,'One';" ^
  "$port.ReadTimeout = 2000; $port.DtrEnable = $true;" ^
  "try { $port.Open() } catch { Write-Host ('Cannot open %PORT% - is PuTTY or another monitor using it? (' + $_.Exception.Message + ')'); exit 1 };" ^
  "try { while ($true) {" ^
  "  $port.DiscardInBuffer(); $null = $port.ReadLine();" ^
  "  $line = $port.ReadLine().Trim();" ^
  "  Write-Host ('[{0:HH:mm:ss}] {1}' -f (Get-Date), $line);" ^
  "  Start-Sleep -Milliseconds 1000 } }" ^
  "catch [TimeoutException] { Write-Host 'No data received (timeout).' }" ^
  "finally { $port.Close() }"
