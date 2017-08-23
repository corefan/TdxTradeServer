#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import shutil
import datetime
import time

qt_version = "5_9_1"

def p(s):
    now = datetime.datetime.now()
    nowstr = str(now)
    print(nowstr + " > " + s)


if __name__ == "__main__":

    archive_name = "TdxTradeServer"

    bpath = os.path.dirname(os.path.abspath(__file__))

    package_build_path = os.path.join(bpath, "package_build_path")
    out_path = os.path.join(package_build_path, "out")
    files_path = os.path.join(package_build_path, "files")
    p("cleaning build path...")
    shutil.rmtree(package_build_path, ignore_errors=True)
    p("done")
    p("making build path...")
    os.mkdir(package_build_path)
    os.mkdir(out_path)
    os.mkdir(files_path)
    p("cp all dependencies...")
    [shutil.copy( os.path.join(os.path.join(bpath, "debug_version_dependencies"), s), files_path) for s in os.listdir( os.path.join(bpath, "debug_version_dependencies"))]
    p("cp exe file...")
    exec_path = os.path.join(os.path.dirname(bpath), "build-TdxTradeServer-Desktop_Qt_" + qt_version + "_MSVC2015_32bit-Debug") + "\\debug\\TdxTradeServer.exe"
    if os.path.exists(exec_path):
        [shutil.copy(exec_path, files_path)]
        p("make zip file...")
        os.chdir(out_path)
        shutil.make_archive(archive_name, "zip", root_dir=files_path, base_dir=files_path)
        p("Done!")
    else:
        p("exe file not found" + exec_path)
        p("FAILED!")





