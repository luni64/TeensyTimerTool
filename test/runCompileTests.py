
libraryName = "TeensyTimerTool"

boards = [
    "teensylc",  # kinetisL
    "teensy31",  # kinetisK
    "teensy41",  # imxrt62
]

examples = [
    {"folder":"examples/01_basic/HelloPeriodic"},
    {"folder":"examples/01_basic/HelloOneShot"},
    {"folder":"examples/01_basic/ArrayOfTimers"},
    {"folder":"examples/01_basic/MoreTimers",             "exclude":["teensy31","teensylc"]},  # example uses T4x timers
    {"folder":"examples/01_basic/RTC_Timer",              "exclude":["teensy31","teensylc"]},  # RTC only works for T4X
    {"folder":"examples/01_basic/UsingChronoDurations1"},
    {"folder":"examples/01_basic/UsingChronoDurations2"},
    {"folder":"examples/02_Advanced/CallbackWithParams"},
    {"folder":"examples/02_Advanced/UsingLambdas"},
    {"folder":"examples/02_Advanced/CapturingLambdas"},
    {"folder":"examples/03_Applications/DoubleExposure",   "exclude":["teensylc","teensy41"]},  # uses FTMs
    {"folder":"examples/99_Misc/PinInformation",           "exclude":["teensy31","teensylc"]},  # requires T4X
]

dependencies=[
]

#*************************************************************************************************
import subprocess
import tempfile
import os
import distutils.dir_util

ESC    = "\x1B"
red    = ESC + "[91m"
green  = ESC + "[92m"
yellow = ESC + "[93m"
blue = ESC + "[94m"
bold   = ESC + "[1m"
endC   = ESC + "[0m"

# print("1234567890", end='')
# print("\033[1D",end='')
# print ("E")

# exit()


print("\n================================ SETTING UP TEST PROJECT ================================")

targetPrjFolder = os.path.join(tempfile.gettempdir(), libraryName)
targetLibFolder = os.path.join(targetPrjFolder,"lib",libraryName)
targetSrcFolder = os.path.join(targetPrjFolder,"src")

if os.path.isdir(targetPrjFolder): # start with a fresh setup
    distutils.dir_util.remove_tree(targetPrjFolder)

distutils.dir_util.mkpath(targetPrjFolder)

initParams = ["pio","project", "init"]
initParams.append("--project-dir=" + targetPrjFolder)
for board in boards:
    initParams.append("--board=" + board)

opt = "--project-option=lib_deps="
for dep in dependencies:
    opt += (dep + '\n')
initParams.append(opt)

result = subprocess.run(initParams)

print(bold + "\n================================== COMPILING EXAMPLES ===================================" + endC)

distutils.dir_util.copy_tree("./src",targetLibFolder+"/src")

totalExamples = str(len(examples)*len(boards))
currentExample = 1

for board in boards:
    boardDesc = blue + bold + '{:55}'.format(board) + endC
    print(boardDesc + " STATUS" )

    for example in examples:
        exFolder = example['folder']
        exFolderDesc = '-' + '{:55}'.format(exFolder)
        counterDesc =  (f"{currentExample}/{totalExamples}").center(8)
        currentExample += 1
        print(exFolderDesc + counterDesc, end="", flush=True)

        if os.path.isdir(targetSrcFolder):
            distutils.dir_util.remove_tree(targetSrcFolder)
        distutils.dir_util.copy_tree (exFolder,targetSrcFolder)

        if not ("exclude" in example and (board in example['exclude'])):
            result = subprocess.run(["pio", "run",  "-e"+board,  "--project-dir=" + targetPrjFolder],capture_output=True)
            print("\033[8D",end='') # move cursor 8 columns left
            if result.returncode == 0:
                print(green  + "[PASSED]       " + endC)
            else:
                print(red    + "[FAILED]       " + endC)
        else:
                print("\033[8D", end='')
                print(yellow + "[ N/A  ]       " + endC)
    print(' ')