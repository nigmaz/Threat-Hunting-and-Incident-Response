$password = "nigmaz76$" | ConvertTo-SecureString -asPlainText -Force
$username = "debug\administrator"
$credential = New-Object System.Management.Automation.PSCredential($username, $password)
$computerName = "192.168.137.145"

Invoke-Command -ComputerName $computerName -Credential $credential -ScriptBlock {
    wmic process call create "calc.exe"
}
