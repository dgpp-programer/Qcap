
#!/bin/bash


output1=$(/home/dpdk/dpdk-stable-21.11.2/usertools/dpdk-devbind.py --status | awk '
  /Network devices using DPDK-compatible driver/ {
    getline
    while ($0 !~ /^$/) {
      if ($0 ~ /^([^ ]+)/) {
        device = $1
      }
      if ($0 ~ /unused=([^ ]+)/) {
        unused = substr($0, index($0, "unused=") + 7)
      }
      if (device && unused) {
        print device, unused
      }
      getline
    }
  }
')

output2=$(/home/dpdk/dpdk-stable-21.11.2/usertools/dpdk-devbind.py --status | awk '
  /Other Network devices/ {
    getline
    while ($0 !~ /^$/) {
      if ($0 ~ /^([^ ]+)/) {
        device = $1
      }
      if ($0 ~ /unused=([^ ]+)/) {
        split(substr($0, index($0, "unused=") + 7), arr, ",")
        unused = arr[1]
      }
      if (device && unused) {
        print device, unused
      }
      getline
    }
  }
')

IFS=$'\n'  # 设置换行符为字段分隔符

for line in $output1; do
  device=$(echo "$line" | awk '{print $1}')
  unused=$(echo "$line" | awk '{print $2}')
  /home/dpdk/dpdk-stable-21.11.2/usertools/dpdk-devbind.py --bind="$unused" "$device"
done

for line in $output2; do
  device=$(echo "$line" | awk '{print $1}')
  unused=$(echo "$line" | awk '{print $2}')
  /home/dpdk/dpdk-stable-21.11.2/usertools/dpdk-devbind.py --bind="$unused" "$device"
done

rmmod igb_uio