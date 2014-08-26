#-*- coding: gbk -*-
############
# 20121003 wcdj
# FindFilev0.1
# 遍历目录和搜索文件中的关键字
############

# import module
import os
import sys
import cmd


# test
#print os.listdir('e:\\')
print "Beg"
print sys.argv

class Tool(cmd.Cmd):
    def __init__(self):
        cmd.Cmd.__init__(self)# initialize the base class
        self.pathname = "e://"
        self.dirname = "test"
        self.prompt = "(FindFile)>"
        self.intro = '''FindFilev0.1 usage:
    set            # set dir and path parameters
    export         # export result file
    find keyword   # set finding keyword
    ?              # help
    exit           # exit current program, or use Ctrl+D(UNIX)|Ctrl+Z(Dos/Windows)
    '''
    
    def help_exit(self):
        print "quits the program"
    def do_exit(self, line):
        print "bye !"
        sys.exit()
    
    def help_set(self):
        print "set parameter program used"
    def do_set(self, choice):
        print "default para is: "
        print "pathname: '%s'" %self.pathname
        print "dirname: '%s'" %self.dirname
        choice = raw_input("Do you wanna change? y/n")
        if choice == "y":
            self.pathname = raw_input("input pathname: ")
            self.dirname = raw_input("input dirname: ")
        print "set pathname as: '%s'" %self.pathname
        print "set dirname as: '%s'" %self.dirname
    
    def help_find(self):
        print "set finding keyword"
    def do_find(self, keyword):
        if keyword == "":
            keyword = raw_input("input finding keyword: ")
        print "set finding keyword as: '%s'" %keyword
        GrepFromTxt(self.pathname + self.dirname, keyword)
        print "find over !"

    def help_export(self):
        print "export result file"
    def do_export(self, para):
        print "pathname: '%s'" %self.pathname
        print "dirname: '%s'" %self.dirname
        WriteDirList(self.pathname + self.dirname, "output.txt")
        print "export over !"
        
        
# write file lists in according path
def WriteDirList(path, file):
    export = ""
    for root, dirs, files in os.walk(path):
        export += "\n%s %s %s" %(root, dirs, files)
    open(file, "w").write(export)

# grep keyword from txt type file only
def GrepFromTxt(path, keyword):
    filelist = os.listdir(path)
    bFind = ""
    print filelist
    for file in filelist:
        if ".txt" in file:
            curfile = open(path + "//" + file)
            print "finding %s..." %(curfile)
            for line in curfile.readlines():
                if keyword in line:
                    print line
                    bFind = "true"
            if bFind != "true":
                print "find nothing !"
                


##########
# start
##########
if __name__ == '__main__':
    cdc = Tool()
    cdc.cmdloop()

  
print "End"




