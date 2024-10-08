# Check User
:log warning "Check All Users";
:foreach user in=[/ip hotspot user print as-value] do={
    :local userName ($user->"name");
    :local limitUptime [/ip hotspot user get $userName limit-uptime];
    :local uptime [/ip hotspot user get $userName uptime];
    :local interval "00:00:00";  # Default interval to avoid unset variable issues
    :local profile [/ip hotspot user get $userName profile];
    :local schedName [/system schedule find name=$userName];


    # Skip default-trial user and admin profile
    :if ($userName != "default-trial" && $profile != "admin") do={  

        # Set schedule for unused user
        :if ($uptime = "00:00:00") do={
            :if ([:len $schedName] = 0) do={
                :set interval "23:59:59";
                :log warning "Scheduled removal for user: $userName with interval $interval";
                /system schedule add name=$userName interval=$interval on-event="/ip hotspot active remove [find user=$userName]\r\n/ip hotspot user remove [find name=$userName]\r\n/system schedule remove [find name=$userName]";
            }
        } 

        # Delete timed-out user
        :if ($limitUptime <= $uptime) do={
            :log warning "Deleting timed-out user: $userName";
            /ip hotspot user remove [find name=$userName];
            /system schedule remove [find name=$userName];
        }
    } else={
        :log warning "Skipping user: $userName";
    }
}
