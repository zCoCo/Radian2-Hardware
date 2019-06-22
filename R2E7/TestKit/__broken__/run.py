import sys, subprocess

command = ['pio','run']

if len(sys.argv) > 1:
    command.append('-e')
    command.append(sys.argv[1])
if len(sys.argv) > 2:
    command.append('-t')
    command.append(sys.argv[2])

print command
print subprocess.check_output(command)
