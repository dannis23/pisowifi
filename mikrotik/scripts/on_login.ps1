:local userName $user
:local limitUptime [/ip hotspot user get [find name=$userName] limit-uptime]
:local uptime [/ip hotspot user get [find name=$userName] uptime]
:local interval "00:00:00"
:local profile ""
:local addSched "0"
:local macAddress [/ip hotspot active get [find user=$userName] mac-address]
:local address [/ip hotspot active get [find user=$userName] address]
:local schedName [/system schedule find name=$userName]

:log warning "limit-uptime: $limitUptime"
:log warning "uptime: $uptime"
:log warning "schedule-name: $schedName"

:if ($uptime = "00:00:00") do={
    :if ([:len $schedName] = 0) do={
        :set addSched "1"
    } else={
        :local schedInterval [/system schedule get $userName interval]
        :if ($schedInterval = "23:59:59") do={
            :set addSched "1"
        } 
    }

    :log warning "Add schedule: $addSched"

    :if ($addSched = "1") do={
        :log warning "Login Check User"

        :if ([:totime $limitUptime] <= [:totime "00:12:00"]) do={
            :log warning "Setting schedule for 30 minutes"
            :set interval "00:30:00"
            :set profile "hotspot"
        } else={
            :if ([:totime $limitUptime] <= [:totime "23:59:59"]) do={
                :log warning "Setting schedule for 1 day"
                :set interval "1d 00:00:00"
                :set profile "hotspot2"
            } else={
                :log warning "Setting schedule for 2 days"
                :set interval "2d 00:00:00"
                :set profile "hotspot3"
            }
        }

        :if ($interval != "00:00:00") do={
            :log warning "Scheduled removal for user: $userName<<<$profile>>> with interval $interval"
            /system schedule remove [find name=$userName]
            /system schedule add name=$userName interval=$interval on-event="/ip hotspot active remove [find user=$userName]\r\n/ip hotspot user remove [find name=$userName]\r\n/system schedule remove [find name=$userName]"
            /ip hotspot user set $userName address=$address
            /ip hotspot user set $userName profile=$profile
        }
    } else={
        :log warning "skipping schedule creation or re-scheduling"
    }
} else={
    :log warning "User has already logged in before, skipping schedule creation."
}
