#!/usr/bin/python3
import os
import sys
import glob
import subprocess

#************* Config *************
threadNum = 16
queueSize = 1024
methodNum = 2
# The final data amount =
#     dataAmountStart + (totalTestCnt - 1) * dataAmountDiff
dataAmountStart = (4 * 1024)
dataAmountDiff = (128 * 1024)
totalTestCnt = 20
# Test counts for each case
testCntPerCase = 3
prog = "../../test/queue/queue_test"
#*********************************

dataAmount = dataAmountStart
dataAmountInKb = dataAmount / 1024.0
dataFile = "data.csv"
imageFile = "perf.png"
intermFileFormat = "test_data_{}_mothod_{}.{}" # The format of intermediary file
methodTitleFormat = "{}Method\t\t\tSemaphore\tLock-free\n"
# Statistics file related
summaryFile = "summary.stat"
statItems = ["Avg", "Max", "Min"]
statValPos = 1      # The position of the value in line in stat file
                    # (e.g. Get number from "Avg: 6.26 ms")
statToPlot = "Avg"  # Which stat to use to do the plot
# Temporary file related
tmpValPos = 2       # The position of the value in line in tmp file
                    # (e.g. Get number from "Execution time: 6.86 ms")
# Commads
progCmdFormat = prog + " -n {} -q {} -d {} -i {}"
plotCmdFormat = "gnuplot -c perf.gp {}"

def remove_file(file):
    fileList = glob.glob(file)
    for filePath in fileList:
        try:
            os.remove(filePath)
        except:
            print("Error while deleting file: ", filePath)

def remove_all_files():
    remove_file("*.tmp")
    remove_file("*.stat")
    remove_file(dataFile)
    remove_file(imageFile)

def do_multi_test(dataAmount, method, tmpFile):
    with open(tmpFile, 'a') as tmpFd:
        for i in range(0, testCntPerCase):
            cmd = progCmdFormat.format(str(threadNum), str(queueSize),
                                       str(dataAmount), str(method))
            subprocess.call(cmd.split(), stdout=tmpFd)

def do_statistics(dataAmount, method, tmpFile, statFile):
    sum = 0.0
    num = 0
    min = sys.float_info.max
    max = 0.0
    statValues = []

    with open(tmpFile, 'r') as tmpFd:
        for line in tmpFd:
            values = line.split(" ")
            val = float(values[tmpValPos])
            sum += val
            if val > max:
                max = val

            if val < min:
                min = val
            
            num += 1

    avg = sum/num
    statValues.append(avg)
    statValues.append(max)
    statValues.append(min)
    #print("avg: {:.3f}, max: {:.3f}, min: {:.3f}".format(statValues[0], statValues[1], statValues[2]))

    with open(statFile, 'w') as statFd:
        for i in range(0, len(statItems)):
            statFd.write("{}: {:.3f} ms\n".format(statItems[i], statValues[i]))

def do_stat_summary(dataAmountInKb):
    statFiles = []
    statListArray = []

    # Save all file names to an array 
    for i in range(0, methodNum):
        statFiles.append(intermFileFormat.format(str(dataAmount), str(i), "stat"))

    with open(summaryFile, 'a') as summaryFd:
        # Save values in each stat file to a list and append it to an array
        for i in range(0, methodNum):
            statList = []
            with open(statFiles[i], 'r') as fd:
                for line in fd:
                    values = line.split(" ")
                    val = values[statValPos]
                    statList.append(val)
            statListArray.append(statList)

        # Write stat values into summary file 
        summaryFd.write("Data: {} KB\n".format(str(dataAmountInKb)))
        for i in range(0, len(statItems)):
            summaryFd.write("{}:\t\t\t".format(str(statItems[i])))
            for j in range(0, methodNum):
                statList = statListArray[j]
                summaryFd.write("{}\t\t".format(statListArray[j][i]))
            summaryFd.write("\n")

def write_data_file(dataFd, statFile):
    with open(statFile, 'r') as statFd:
        for line in statFd:
            if statToPlot in line:
                values = line.split(" ")
                val = float(values[statValPos])
                dataFd.write("{}\t\t".format(val))
                break

def do_plot(xScale):
    cmd = plotCmdFormat.format(str(xScale))
    subprocess.call(cmd.split())

if __name__ == '__main__':
    remove_all_files()

    print("Test parameters - Thread num: {}, Queue size: {}"
          .format(threadNum, queueSize))

    # Write method title to summary file
    with open(summaryFile, 'w') as summaryFd:
        summaryFd.write(methodTitleFormat.format(""))

    with open(dataFile, 'a') as dataFd:
        dataFd.write(methodTitleFormat.format("#"))
        dataFd.write("#Data num\n")

        for i in range(0, totalTestCnt):
            line = "{}\t\t\t".format(dataAmountInKb)
            dataFd.write(line)

            for j in range(0, methodNum):
                tmpFile = intermFileFormat.format(str(dataAmount), str(j), "tmp")
                statFile = intermFileFormat.format(str(dataAmount), str(j), "stat")
                do_multi_test(dataAmount, j, tmpFile)
                do_statistics(dataAmount, j, tmpFile, statFile)
                write_data_file(dataFd, statFile)
                print("Finised - Data amount: {}, Method: {}"
                      .format(dataAmount, j))

            dataFd.write("\n")
            do_stat_summary(dataAmountInKb)
            dataAmount = dataAmount + dataAmountDiff
            dataAmountInKb = dataAmount / 1024.0

    xScale = dataAmountInKb / 10
    do_plot(xScale)



