import os
from shutil import copyfile

def run():
    print " > Moving Pinouts"
    try:
        # Target Directory for File Move:
        targetBase = os.path.expanduser('~')
        targetBase = os.path.join(targetBase,'.platformio', 'packages', 'framework-arduinoavr', 'variants')
        # Loop through All *.h Files in "pins" Directory and Move to Named
        # Directory in targetBase:
        for root, dirs, files in os.walk("pins"):
            for file in files:
                if file.endswith(".h"):
                    print " >>. Moving: " + str(file[:-2])
                    d = os.path.join(targetBase, str(file[:-2]))
                    if not os.path.exists(d):
                        os.makedirs(d)
                    copyfile(
                        os.path.join(root, str(file)),
                        os.path.normpath(os.path.join(d,"pins_arduino.h"))
                    )
    except Exception as e:
        print " > # Failed to Move Pinout Files: "
        print str(e)
    else:
        print " >. Pinouts Moved."
