import os
os.chdir("E:\\GLpygame\\")
if os.path.isdir("Pack"):
    if os.listdir("Pack"):
        print("Folder non-empty")
        try:
            input("Press Enter to clear")
        except KeyboardInterrupt:
            exit(0)
        else:
            os.system("del Pack /S /Q")
elif os.path.exists("Pack"):
    print("ERROR: File with destination name exists")
else:
    os.mkdir("Pack")
os.system("copy 3ddemo.exe Pack")
os.system("copy freetype.dll Pack")
os.system("xcopy rsrc Pack\\rsrc\\ /E")
os.system("xcopy demorsrc Pack\\demorsrc\\ /E")
