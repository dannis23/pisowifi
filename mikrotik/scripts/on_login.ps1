:local userName $user;
:local limitUptime [/ip hotspot user get [find name=$userName] limit-uptime]
:local uptime [/ip hotspot user get [find name=$userName] uptime]
:local profile [/ip hotspot user get $userName profile]
:local interval "00:00:00";  # Default interval to avoid unset variable issues
:local macAddress [/ip hotspot active get [find user=$userName] mac-address]
:local address [/ip hotspot active get [find user=$userName] address]

:if ($uptime = "00:00:00") do={
    :if ([:len [/system schedule find name=$userName]] = 1) do={
        :log warning "Login Check User";

        :if ($profile = "hotspot") do={
            :log warning "Setting schedule for 30 minutes"
            :set interval "00:30:00"
        } else={
            :if ($profile = "hotspot2" || $profile = "hotspot3") do={
                :log warning "Setting schedule for 1 day"
                :set interval "1d 00:00:00"
            } else={
                :if ($profile = "hotspot4") do={
                    :log warning "Setting schedule for 2 days"
                    :set interval "2d 00:00:00"
                } else={
                    :log warning "Profile not matched, no schedule set for user: $userName"
                }
            }
        }

        :if ($interval != "00:00:00") do={
            :log warning "Scheduled removal for user: $userName<<<$profile>>> with interval $interval";
            /system schedule remove [find name=$userName];
            /system schedule add name=$userName interval=$interval on-event="/ip hotspot active remove [find user=$userName]\r\n/ip hotspot user remove [find name=$userName]\r\n/system schedule remove [find name=$userName]";
            /ip hotspot user set $userName address=$address;
        }
    } else={
        /system script run check_user;
    }
}