#!/usr/bin/python3

import os
import subprocess
from shutil import copyfile


def listFiles(dir, method):
    os.chdir(dir)
    if method is True:
        files = filter(lambda file: file.endswith('.aag'), os.listdir(dir))
    else:
        files = filter(
            lambda file: not file.startswith('err') and file.endswith('.aag'),
            os.listdir(dir)
        )
    files = list(files)
    files.sort()
    return files

def makeDoFile(dir, fileName):
    content = "cirr " + dir + fileName
    print("Diff file: {0}".format(fileName))
    open("testDo", "w").close()
    with open("testDo", "a+") as dofile:
        dofile.write(content)
        dofile.write("\n")
        dofile.write("cirp\n")
        dofile.write("q -f\n")


def main():
    print("Start Diff...")
    runDir = os.getcwd()
    doGood = True
    path = runDir + '/tests.fraig' if doGood else runDir + '/tests.err'
    files = listFiles(path, True)
    os.chdir(runDir)

    subprocess.call("make 64", shell=True)
    subprocess.call("make", shell=True)
    for file in files:
        makeDoFile('tests.fraig/', file)
        subprocess.call("./cirTest -f testDo > myOut 2>&1", shell=True)
        subprocess.call("./ref/cirTest -f testDo > refOut 2>&1", shell=True)
        subprocess.call("diff -b myOut refOut", shell=True)
        print("-----" * 20)

    subprocess.call("echo End Diff!!!", shell=True)
    subprocess.call("make clean", shell=True)


main()