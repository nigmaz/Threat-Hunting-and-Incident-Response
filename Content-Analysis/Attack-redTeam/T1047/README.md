# Windows Management Instrumentation
- **REPORT:**
    * https://drive.google.com/file/d/1MkfdCFkW1shGq2QoT9e84aizpv4IGGow/view?usp=sharing
    * https://drive.google.com/file/d/1D3WodzFaGRrH4L4WNUwGXwrWahuvWK0E/view
- **REFERENCES:**
    * https://learn.microsoft.com/en-us/windows/win32/wmisdk/connecting-to-wmi-remotely-starting-with-vista
    * https://www.youtube.com/watch?v=ZwLbubzmjj4&t=385s
    * https://www.youtube.com/watch?v=tLyTLQ01508
    * https://www.youtube.com/watch?v=EN7AZgE_2uc
    * Rule:
         + https://detection.fyi/sigmahq/sigma/windows/process_creation/proc_creation_win_wmic_uninstall_application/?query=t1047
         + https://redcanary.com/threat-detection-report/techniques/windows-management-instrumentation/
         + https://attack.mitre.org/techniques/T1047/

## [0] CLIENTS - (Windows 10)
- Nmap check port 135 - DCOM
```powershell 
PS C:\Users\trant> nmap 192.168.137.145 -p 135
Starting Nmap 7.94 ( https://nmap.org ) at 2023-08-07 12:49 SE Asia Standard Time
Nmap scan report for 192.168.137.145
Host is up (0.0010s latency).

PORT    STATE    SERVICE
135/tcp filtered msrpc
MAC Address: 00:0C:29:A6:DF:DF (VMware)

Nmap done: 1 IP address (1 host up) scanned in 13.43 seconds
PS C:\Users\trant>
```

- `1.` Set NetworkCategory: Private <[Bật mạng thành private](https://vitinhquan7.info/cach-thay-doi-mang-cong-cong-thanh-mang-rieng/)> .
```powershell
Get-NetConnectionProfile
```

- `2.` Setup Service connect
    * Allow PowerShell remote
    ```powershell
    Enable-PSRemoting -Force
    ```
    * Start winrm in clients
    ```powershell
    winrm quickconfig
    ```
- `3.` Set AllowUnencrypted: true
```powershell
winrm get winrm/config/client
```
```powershell
Set-Item WSMan:\localhost\Client\AllowUnencrypted -Value $true
```
```powershell
Set-Item WSMan:\localhost\Service\AllowUnencrypted -Value $true
```
- `4.` WMI Clients allow Server is legit
```powershell
Set-Item WSMan:\localhost\Client\TrustedHosts -Value "192.168.137.145" -Concatenate -Force
```

## [1] SERVER - (Windows Server 2019)
- `1.` Run WMIC in windows server 2019
```powershell
Set-Service -Name winrm -StartupType Automatic
Start-Service winrm
```

- `2.` Add permisions user (GUI)
    * `a.` Component Services/Computers/My Computer/Properties/COM Security
    * `a.` Add user Debug\Administrator to 2 option `Edit Limits`
    * `b.` Windows + R > wmimgmt.msc /WMI Control (Local)/Properties/Security/Security
    * `b.` Add user Debug\Administrator full permisions

- `3.` Allow firewall (GUI)
    * File and Printer Sharing (Echo Request - ICMPv4-In) *
    * File and Printer Sharing (Echo Request - ICMPv6-In) *
    * Virtual Machine Monitoring (RPC)
    * Windows Defender Firewall Remote Management (RPC)
    * Windows Defender Firewall Remote Management (RPC-EPMAP)
    * Windows Management Instrumentation (ASync-In) *
    * Windows Management Instrumentation (DCOM-In) *
    * Windows Management Instrumentation (WMI-In) *
```powershell
Get-Service -Name RpcSs
```
```powershell
Start-Service -Name RpcSs
```
```powershell
Set-NetFirewallProfile -Profile Domain,Public,Private -Enabled False
```
```powershell
Set-NetFirewallRule -Name WINRM-HTTP-In-TCP-PUBLIC -RemoteAddress Any
```


- `4.` Allow PowerShell remote
```powershell
Enable-PSRemoting -Force
```

- `5.` AllowUnencrypted: true
```powershell
winrm get winrm/config/service
```
```powershell
Set-Item WSMan:\localhost\Service\AllowUnencrypted -Value $true
```

## [2] POWERSHELL SCRIPT:
- Local test
```powershell
wmic /node:"127.0.0.1" process call create "calc.exe"
```
- Remote test
```powershell
Invoke-WmiMethod -Class Win32_Process -Name Create -ArgumentList 'calc.exe' -ComputerName <ip-server> -Credential 'debug\Administrator' 
```




