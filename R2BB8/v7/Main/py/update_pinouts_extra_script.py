import os
from SCons.Script import (AlwaysBuild, Builder, DefaultEnvironment, Exit,
                          Environment, Default, Glob, COMMAND_LINE_TARGETS,
                          GetOption)

env = DefaultEnvironment()

piopackages_dir = env.subst('$PLATFORMIO_SRC_DIR');

def before_build(source, target, env):
    print "# Before Build #"
    # Must store pinouts locally and move before each build to prevent overwrite
    # on update.
    print " > Moving Pinouts"
    os.rename("$PIO_SRC_DIR/../pins/m168pb16m/m168pb16m.h", "$PIO_SRC_DIR/../pins/m168pb16m.h")
    #~/.platformio/packages/framework-arduinoavr/variants/
    print "### > Pinouts Moved."

env.AddPreAction("upload", before_build)
env.AddPreAction("buildprog", before_build)
