:local userName $user;
:local limitUptime [/ip hotspot user get [find name=$userName] limit-uptime];
:local uptime [/ip hotspot user get [find name=$userName] uptime]

:if ($limitUptime <= $uptime) do={
    :log warning "delete time out name: $userName"
    /ip hotspot user remove [find name=$userName]
    /system schedule remove [find name=$userName]
}