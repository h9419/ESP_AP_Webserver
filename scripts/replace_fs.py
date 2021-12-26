import platform
Import("env")
print("Replace MKSPIFFSTOOL with mklittlefs")
env.Replace(MKSPIFFSTOOL = "./scripts/mklittlefs/" + platform.system() + "_" + platform.machine().lower() + "/mklittlefs")