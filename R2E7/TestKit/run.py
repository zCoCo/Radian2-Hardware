import sys, subprocess

command = ['pio','run']
start_monitor = ['pio','device','monitor']

if len(sys.argv) > 1:
    command.append('-e')
    command.append(sys.argv[1])
if len(sys.argv) > 2:
    command.append('-t')
    command.append(sys.argv[2])

print command
print subprocess.check_output(command)
# if len(sys.argv) > 2 and sys.argv[2] == "upload":
#     print subprocess.check_output(start_monitor)
