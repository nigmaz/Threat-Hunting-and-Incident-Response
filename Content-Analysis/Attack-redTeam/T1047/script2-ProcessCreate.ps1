$password = "nigmaz76$" | ConvertTo-SecureString -asPlainText -Force
$username = "debug\administrator"
$credential = New-Object System.Management.Automation.PSCredential($username, $password)
$computerName = "192.168.131.133"

Invoke-WmiMethod -Class Win32_Process -Name Create -ArgumentList 'calc.exe' -ComputerName $computerName -Credential $credential
