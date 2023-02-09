#!/usr/bin/expect

spawn bluetoothctl
expect "Agent registered"
send "power on\r"
expect "Changing power on succeeded"
send "discoverable on\r"
expect "Changing discoverable on succeeded"
send "pairable on\r"
expect "Changing pairable on succeeded"
send "default-agent\r"
expect "Default agent request successful"

while {1} {
    expect {
        "Request confirmation" {
        send "yes\r"
        }
        eof {
            break
        }
    }
}