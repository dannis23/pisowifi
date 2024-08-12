#Check User
:log warning "Check All User";
:foreach user in=[/ip hotspot user print as-value] do={
    :local userName ($user->"name");
    :local limitUptime [/ip hotspot user get $userName limit-uptime];
    :local uptime [/ip hotspot user get $userName uptime];
    :local profile [/ip hotspot user get $userName profile];
    :local interval "00:00:00";  # Default interval to avoid unset variable issues

    # Skip default-trial user
    :if ($userName != "default-trial") do={  
        #Delete unused user
        :if ($uptime = "00:00:00") do={
            :if ([:len [/system schedule find name=$userName]] = 0) do={
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
                    :log warning "Scheduled removal for user: $userName<<<$profile>>> with interval $interval"
                    /system schedule add name=$userName interval=$interval on-event="/ip hotspot active remove [find user=$userName]\r\n/ip hotspot user remove [find name=$userName]\r\n/system schedule remove [find name=$userName]";
                }
            }
        } 

        #Delete time out user
        :if ($limitUptime <= $uptime) do={
            :log warning "delete time out name: $userName"
            /ip hotspot user remove [find name=$userName]
            /system schedule remove [find name=$userName]
        }
    }
}